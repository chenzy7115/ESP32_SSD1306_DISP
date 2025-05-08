| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-C61 | ESP32-H2 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | --------- | -------- | -------- | -------- | -------- |


#  TEMP & HUMI sensor that are DHT11
#  DISPLAY I2C OLED SSD1306芯片

* 这个项目是使用ESP-IDF框架，开发版是ESP32，对乐鑫的官方例程进行修改。
* 使用freeRTOS的多任务。
* DHT11采集到的温度和湿度信息，在OLED上显示。

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

      ESP Board                       DHT11 1wire
+------------------+              +-------------------+
|               GND+--------------+GND                |
|                  |              |                   |
|               3V3+--------------+VCC                |
|                  |              |                   |
|                 DA+--------------+DA                |
|                  |              |                   |
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

## 调试过程中遇到过的问题

## 1.栈空间不足

如果将display_task的优先级由2降为1时将出现任务栈空间不足的问题，如下提示
***ERROR*** A stack overflow in task display has been detected.
栈空间不足的直接原因‌
display_task当原分配栈大小：2048 bytes
LVGL初始化(lvgl_init())和刷新(lvgl_display_ui())需要大量栈空间（通常≥4096 bytes）
优先级降低后，任务调度导致栈使用模式变化，暴露原有临界空间问题
调试后将栈分配空间调整为4096。同时优先级改为2,

## 2.时序控制‌

DHT11读取间隔 ≥1s，避免传感器无响应26
显示屏刷新率建议 ≤1Hz，防止LVGL任务过载

实际调试中将DHT11和display的时间都调整为500ms，同时display优先级为2，DHT11优先级为1

测试通过

## 3.内存溢出问题

lvgl的对象，如果没有必要不要一直创建
反复创建的对象，需要采样静态的方式来创建，通常在头文件中进行创建，
避免反复创建，哪怕删除对象，由于内存管理的问题，也会出现无内存可用的情况。


