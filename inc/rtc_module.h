#ifndef RTC_MODULE_H
#define RTC_MODULE_H

#include "DS1302.H"

typedef struct {
    unsigned int seconds;     // 剩余秒数
    unsigned char time_mode;  // 0 = dist/get_speed_res 模式，1 = 20s 模式
} TimeInfo;

extern TimeInfo xdata tinfo;
extern unsigned char xdata get_speed_res;    // 速度变量，使用unsigned char类型以节省内存
extern unsigned int xdata dist[6];              // 距离，单位m
extern struct_DS1302_RTC xdata rtc_time[4];  // RTC时间结构
extern bit mode;                             // 时间模式：0=计算模式，1=固定模式

extern void ShowTime(void);
extern void UpdateTime(void);
extern void ResetTimer(void);  // 重置计时器

#endif  // RTC_MODULE_H