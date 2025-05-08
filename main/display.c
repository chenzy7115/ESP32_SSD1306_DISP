#include <string.h>
#include "lvgl.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
#include "esp_err.h"
#include "esp_log.h"
#include "driver/i2c_master.h"
#include "esp_lvgl_port.h"
#include "esp_lcd_panel_vendor.h"
#include "display.h"

// 定义日志标签，方便在日志中区分不同模块的输出
static const char *TAG = "Display";




// 定义I2C总线端口号
#define I2C_BUS_PORT 0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////// Please update the following configuration according to your LCD spec //////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 定义LCD像素时钟频率，单位为Hz
#define EXAMPLE_LCD_PIXEL_CLOCK_HZ (400 * 1000)
// 定义I2C数据线引脚号
#define EXAMPLE_PIN_NUM_SDA 22
// 定义I2C时钟线引脚号
#define EXAMPLE_PIN_NUM_SCL 23
// 定义复位引脚号，-1表示不使用复位引脚
#define EXAMPLE_PIN_NUM_RST -1
// 定义I2C设备硬件地址
#define EXAMPLE_I2C_HW_ADDR 0x3C
// 水平像素数量
#define EXAMPLE_LCD_H_RES 128
// 垂直像素数量，从配置中获取
#define EXAMPLE_LCD_V_RES CONFIG_EXAMPLE_SSD1306_HEIGHT
// 定义用于表示命令的位数
#define EXAMPLE_LCD_CMD_BITS 8
// 定义用于表示参数的位数
#define EXAMPLE_LCD_PARAM_BITS 8

// 为屏幕对象赋值
//  lv_disp_t *disp = NULL;

lv_disp_t *lvgl_init(void)
{
    // 定义I2C总线句柄，用于后续操作
    i2c_master_bus_handle_t i2c_bus = NULL;

    // i2c_master_bus_handle_t i2c_bus = NULL;
    // 定义I2C总线配置结构体
    i2c_master_bus_config_t bus_config = {
        // 时钟源选择默认值
        .clk_source = I2C_CLK_SRC_DEFAULT,
        // 毛刺忽略计数
        .glitch_ignore_cnt = 7,
        // I2C总线端口号
        .i2c_port = I2C_BUS_PORT,
        // I2C数据线引脚号
        .sda_io_num = EXAMPLE_PIN_NUM_SDA,
        // I2C时钟线引脚号
        .scl_io_num = EXAMPLE_PIN_NUM_SCL,
        // 启用内部上拉电阻
        .flags.enable_internal_pullup = true,
    }; // init_i2c();
    // 打印日志，提示开始初始化I2C总线
    ESP_LOGI(TAG, "Initialize I2C bus");
    // 创建一个I2C总线
    ESP_ERROR_CHECK(i2c_new_master_bus(&bus_config, &i2c_bus));

    // 打印日志，提示开始安装面板IO
    ESP_LOGI(TAG, "Install panel IO");
    // 定义面板IO句柄，用于后续操作
    esp_lcd_panel_io_handle_t io_handle = NULL;
    // 定义I2C面板IO配置结构体
    esp_lcd_panel_io_i2c_config_t io_config = {
        // I2C设备硬件地址
        .dev_addr = EXAMPLE_I2C_HW_ADDR,
        // I2C时钟线速度，单位为Hz
        .scl_speed_hz = EXAMPLE_LCD_PIXEL_CLOCK_HZ,
        // 控制阶段字节数，根据SSD1306数据手册设置
        .control_phase_bytes = 1,
        // 用于表示命令的位数，根据SSD1306数据手册设置
        .lcd_cmd_bits = EXAMPLE_LCD_CMD_BITS,
        // 用于表示参数的位数，根据SSD1306数据手册设置
        .lcd_param_bits = EXAMPLE_LCD_CMD_BITS,
        // 如果配置为SSD1306控制器

        .dc_bit_offset = 6,
        // 如果配置为SH1107控制器

    };
    // 创建新的I2C面板IO，并检查是否成功
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_i2c(i2c_bus, &io_config, &io_handle));

    // 打印日志，提示开始安装SSD1306面板驱动
    ESP_LOGI(TAG, "Install SSD1306 panel driver");
    // 定义面板句柄，用于后续操作
    esp_lcd_panel_handle_t panel_handle = NULL;
    // 定义面板设备配置结构体
    esp_lcd_panel_dev_config_t panel_config = {
        // 每个像素的位数
        .bits_per_pixel = 1,
        // 复位引脚号
        .reset_gpio_num = EXAMPLE_PIN_NUM_RST,
    };
    // 如果配置为SSD1306控制器
    esp_lcd_panel_ssd1306_config_t ssd1306_config = {
        // 屏幕高度
        .height = EXAMPLE_LCD_V_RES,
    };
    // 将SSD1306配置结构体指针赋值给面板配置的供应商配置字段
    panel_config.vendor_config = &ssd1306_config;
    // 创建新的SSD1306面板驱动，并检查是否成功
    ESP_ERROR_CHECK(esp_lcd_new_panel_ssd1306(io_handle, &panel_config, &panel_handle));

    // 复位面板，并检查是否成功
    ESP_ERROR_CHECK(esp_lcd_panel_reset(panel_handle));
    // 初始化面板，并检查是否成功
    ESP_ERROR_CHECK(esp_lcd_panel_init(panel_handle));
    // 打开面板显示，并检查是否成功
    ESP_ERROR_CHECK(esp_lcd_panel_disp_on_off(panel_handle, true));

    // 打印日志，提示开始初始化LVGL
    ESP_LOGI(TAG, "Initialize LVGL");
    // 定义LVGL端口配置结构体，使用默认初始化配置
    static const lvgl_port_cfg_t lvgl_cfg = ESP_LVGL_PORT_INIT_CONFIG();
    // 初始化LVGL端口
    lvgl_port_init(&lvgl_cfg);

    // 定义LVGL显示配置结构体
    const lvgl_port_display_cfg_t disp_cfg = {
        // 面板IO句柄
        .io_handle = io_handle,
        // 面板句柄
        .panel_handle = panel_handle,
        // 缓冲区大小，根据屏幕分辨率计算
        .buffer_size = EXAMPLE_LCD_H_RES * EXAMPLE_LCD_V_RES,
        // 启用双缓冲
        .double_buffer = true,
        // 屏幕水平分辨率
        .hres = EXAMPLE_LCD_H_RES,
        // 屏幕垂直分辨率
        .vres = EXAMPLE_LCD_V_RES,
        // 单色显示
        .monochrome = true,
        // 屏幕旋转配置
        .rotation = {
            // 是否交换XY轴
            .swap_xy = false,
            // 是否水平镜像
            .mirror_x = false,
            // 是否垂直镜像
            .mirror_y = false,
        }};
    // 添加LVGL显示设备，并返回显示设备句柄
    lv_disp_t *disp = lvgl_port_add_disp(&disp_cfg);

    // 屏幕旋转，设置为旋转180度
    lv_disp_set_rotation(disp, LV_DISP_ROT_180);

    // 打印日志，提示开始显示LVGL滚动文本
    ESP_LOGI(TAG, "Display LVGL Scroll Text");

    return disp;
}

/**
 * @brief 显示函数，用于创建并显示一个带有循环滚动文本的LVGL界面。
 *
 * 此函数在指定的显示设备上创建一个屏幕对象，然后在该屏幕上创建一个标签对象，
 * 并将标签文本设置为循环滚动显示。
 *
 * @param disp 指向LVGL显示设备的指针，指定要在哪个显示设备上创建界面。
 */

void lvgl_display_ui(lv_disp_t *disp, int tempe, int humid)
{
    lv_obj_t *scr = lv_disp_get_scr_act(disp);

    // 首次调用时创建对象
    if (label_room == NULL) {
        label_room = lv_label_create(scr);
        label_temp_str = lv_label_create(scr);
        label_humi_str = lv_label_create(scr);
        label_temp_val = lv_label_create(scr);
        label_humi_val = lv_label_create(scr);

        // 初始布局设置
        lv_obj_align(label_room, LV_ALIGN_TOP_LEFT, 0, 0);
        lv_obj_align(label_temp_str, LV_ALIGN_CENTER, -30, -2);
        lv_obj_align(label_humi_str, LV_ALIGN_CENTER, -30, 20);
        lv_obj_align(label_temp_val, LV_ALIGN_CENTER, 25, -2);
        lv_obj_align(label_humi_val, LV_ALIGN_CENTER, 25, 20);

        lv_label_set_text(label_room, "ROOM ENV :");
        lv_label_set_text(label_temp_str, "TEMP :");
        lv_label_set_text(label_humi_str, "HUMI :");
    }

    // 仅更新数值标签
    // char temp[8], humi[8];
    // snprintf(temp, sizeof(temp), "%d", tempe);
    // snprintf(humi, sizeof(humi), "%d", humid);
    lv_label_set_text_fmt(label_temp_val, "%d.%d C",tempe/10,tempe%10);
    lv_label_set_text_fmt(label_humi_val, "%d %%",humid);
}
