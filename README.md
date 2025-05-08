| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-C61 | ESP32-H2 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | --------- | -------- | -------- | -------- | -------- |

# I2C OLED SSD1306芯片

这个项目是使用ESP-IDF框架，开发版是ESP32，对乐鑫的官方例程进行修改。使用freeRTOS的多任务。比较官方例程，可以通过UI接口可以直接传入2个int值，并在

### 需要的硬件

* An ESP development board
* ESP32开发板，只要是基于EPS32芯片的既可
* An SSD1306 OLED LCD, with I2C interface
* OLED的驱动芯片是SSD1306的，如果是使用SH1107的OLED，在IIC驱动上要修改，具体看官方例程
* An USB cable for power supply and programming

### 硬件连接的配置

The connection between ESP Board and the LCD is as follows:
可以在display.c文件中对SDA和SCL的pin直接进行定义后，将开发板的按下图的方式连接到OLED

```text
      ESP Board                       OLED LCD (I2C)
+------------------+              +-------------------+
|               GND+--------------+GND                |
|                  |              |                   |
|               3V3+--------------+VCC                |
|                  |              |                   |
|               SDA+--------------+SDA                |
|                  |              |                   |
|               SCL+--------------+SCL                |
+------------------+              +-------------------+
```





### serial Output
### 串口中输出的
```bash
...
I (308) main_task: Started on CPU0
I (318) main_task: Calling app_main()
I (318) example: Initialize I2C bus
I (318) gpio: GPIO[3]| InputEn: 1| OutputEn: 1| OpenDrain: 1| Pullup: 1| Pulldown: 0| Intr:0
I (328) gpio: GPIO[4]| InputEn: 1| OutputEn: 1| OpenDrain: 1| Pullup: 1| Pulldown: 0| Intr:0
I (338) example: Install panel IO
I (338) example: Install SSD1306 panel driver
I (448) example: Initialize LVGL
I (448) LVGL: Starting LVGL task
I (448) example: Display LVGL Scroll Text
I (448) main_task: Returned from app_main()
...
```

## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
