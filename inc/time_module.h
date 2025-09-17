#ifndef RTC_MODULE_H
#define RTC_MODULE_H

#include "DS1302.H"

#define RUNNING_MODE 0
#define STOP_MODE 1

extern int xdata station_id;
extern unsigned int xdata remain_distance;
extern unsigned int xdata dist[5];              // 距离，单位m
extern bit mode;                  
extern int xdata rest_total_seconds;
extern unsigned int xdata rest_arrive_minute;
extern unsigned int xdata rest_arrive_second;

extern void InitRTC(void);
extern void ShowTime(void);
extern void ShowMode(void);
extern void UpdateTime(void);
extern void ResetTimer(void);  // 重置计时器
extern void onSpeedChange();

#endif  // RTC_MODULE_H