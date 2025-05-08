#ifndef _DISPLAY_H
#define _DISPLAY_H

#include "lvgl.h"

// 声明一个lvgl屏幕对象
// extern lv_disp_t *disp;

/*初始化屏幕
 *使用了SSD1306，IIC接口，地址OX3C
 *SDA pin22 ,SCL pin23
 *@param   void
 *@return lv_disp_t *disp
 */
extern lv_disp_t *lvgl_init(void);

/* 声明外部函数，
 *@param (lv_disp_t *disp)
 *传入的参数是一个lvgl 屏幕对象
 */
extern void lvgl_display_ui(lv_disp_t *disp,int tempe,int humid);


// 在函数外部定义静态对象（避免重复创建）

static lv_obj_t *label_room = NULL; 
static lv_obj_t *label_temp_str = NULL; 
static lv_obj_t *label_humi_str = NULL; 
static lv_obj_t *label_temp_val = NULL; 
static lv_obj_t *label_humi_val = NULL; 


#endif