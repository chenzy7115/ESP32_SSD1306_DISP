
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "display.h"
#include "dht11.h"
#include "esp_random.h"
#include <nvs_flash.h>
#include "esp_log.h"

// 全局定义互斥锁
static SemaphoreHandle_t lvgl_mutex = NULL;
// 温度 湿度变量
static int temp = 0, hum = 0;

void dht11_get_valu_task(void *pt)
{

    DHT11_Init(DHT11_GPIO);
    while (1)
    {
        // 获取互斥锁
        if (xSemaphoreTake(lvgl_mutex, portMAX_DELAY) == pdTRUE)
        {
            // 临界区代码，访问共享资源
            if (DHT11_StartGet(&temp, &hum))
            {
                ESP_LOGI(TAG, "temp:%i.%i C     hum:%i%%", temp / 10, temp % 10, hum);
            }

            // 释放互斥锁
            xSemaphoreGive(lvgl_mutex);
        }

        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

void display_task(void *pt)
{
    // 需要方法使用的变量采用静态声明，避免在重复调用过程中，占用内存
    // 导致内存溢出的问题。
    static lv_disp_t *disp;

    disp = lvgl_init();
    while (1)
    {
        // 获取互斥锁
        if (xSemaphoreTake(lvgl_mutex, portMAX_DELAY) == pdTRUE)
        {
            // 临界区代码，访问共享资源
            // xSemaphoreTake(lvgl_mutex, portMAX_DELAY); // 加锁
            lvgl_display_ui(disp, temp , hum );
            xSemaphoreGive(lvgl_mutex); // 解锁
            vTaskDelay(pdMS_TO_TICKS(500));

        }
    }
}

void app_main(void)
{
    ESP_ERROR_CHECK(nvs_flash_init());
    vTaskDelay(pdMS_TO_TICKS(100));

    ESP_LOGI(TAG, "[APP] APP Is Start!~\r\n");
    ESP_LOGI(TAG, "[APP] IDF Version is %d.%d.%d", ESP_IDF_VERSION_MAJOR, ESP_IDF_VERSION_MINOR, ESP_IDF_VERSION_PATCH);
    ESP_LOGI(TAG, "[APP] Free memory: %lu bytes", esp_get_free_heap_size());
    ESP_LOGI(TAG, "[APP] IDF version: %s", esp_get_idf_version());

    lvgl_mutex = xSemaphoreCreateMutex(); // 初始化锁
    if (lvgl_mutex == NULL) 
    {
        ESP_LOGE(TAG, "无法创建互斥锁");
        return;
    }

    xTaskCreate(display_task, "display", 1024 * 4, NULL, 2, NULL);
    xTaskCreate(dht11_get_valu_task, "DHT11_Get", 1024 * 2, NULL, 1, NULL);

    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}