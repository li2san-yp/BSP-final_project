#include "core.h"

unsigned int xdata real_hour;
unsigned int xdata real_minute;
unsigned int xdata real_second;
xdata struct_DS1302_RTC real_time;

void UpdateRealTimeFromUart1(){
    real_time.second=real_second;
    real_time.minute=real_minute;
    real_time.hour=real_hour;
    RTC_Write(real_time);
}

void ShowRealTime()
{
    Seg7Print(real_time.hour / 10, real_time.hour % 10, 12, real_time.minute / 10, real_time.minute % 10, 12, real_time.second / 10, real_time.second % 10);
}