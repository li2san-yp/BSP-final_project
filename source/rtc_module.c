#include "core.h"
uchar get_speed_res;
unsigned int dist = 1737; // m
struct_DS1302_RTC xdata rtc_time[4];
bit mode = 0; 
TimeInfo tinfo = {0, 0};
void ResetTimer()
{
    unsigned int total_seconds;
	get_speed_res = GetSpeed();
    if (mode == 0) {
        total_seconds = dist * 3.6 / get_speed_res; 
    } 
    else {
        total_seconds = 20; 
    }
    rtc_time[id].minute = total_seconds / 60;
    rtc_time[id].second = total_seconds % 60;
    RTC_Write(rtc_time[id]);
}
void GetETA()
{
    rtc_time[id] = RTC_Read();
    tinfo.seconds = rtc_time[id].minute * 60 + rtc_time[id].second;
    tinfo.time_mode = mode;  // 直接返回当前全局 mode
}
void ShowTime()
{
    rtc_time[id] = RTC_Read();
    if (rtc_time[id].minute == 0 && rtc_time[id].second == 0) {
        mode = !mode;
        ResetTimer();
    } 
    else if (rtc_time[id].second > 60 || rtc_time[id].minute > 2){
        ResetTimer();
    }
	else if (rtc_time[id].second == 0) {
        rtc_time[id].second = 59;
        rtc_time[id].minute--;
    } 
	else {
        rtc_time[id].second--;
	}
    RTC_Write(rtc_time[id]);
    Seg7Print(0, 0, 10, rtc_time[id].minute / 10, rtc_time[id].minute % 10, 10, rtc_time[id].second / 10, rtc_time[id].second % 10);
}
