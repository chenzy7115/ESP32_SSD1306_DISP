
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "display.h"
#include "esp_random.h"

// 全局定义互斥锁
static SemaphoreHandle_t lvgl_mutex = NULL;

void display_task(void *pt)
{
    //需要方法使用的变量采用静态声明，避免在重复调用过程中，占用内存
    //导致内存溢出的问题。
    static lv_disp_t *disp;

    lvgl_mutex = xSemaphoreCreateMutex(); // 初始化锁

    disp = lvgl_init();
    while (1)
    {
        xSemaphoreTake(lvgl_mutex, portMAX_DELAY); // 加锁
        lvgl_display_ui(disp, esp_random() % 101, esp_random() % 101);
        xSemaphoreGive(lvgl_mutex); // 解锁
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void app_main(void)
{
    // disp = lvgl_init();
    xTaskCreate(display_task, "display", 1024 * 2, NULL, 2, NULL);
    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}