#include "core.h"

#ifndef _GET_REAL_TIME_H_
#define _GET_REAL_TIME_H_

extern unsigned int xdata real_hour;
extern unsigned int xdata real_minute;
extern unsigned int xdata real_second;
extern xdata struct_DS1302_RTC real_time;

extern void ShowRealTime();
extern void UpdateRealTimeFromUart1();

#endif // _GET_REAL_TIME_H_