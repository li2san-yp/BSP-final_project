#include "core.h"
uchar xdata get_speed_res;
unsigned int xdata dist[6] = {0, 1009, 1078, 2037, 1536};  // m
struct_DS1302_RTC xdata rtc_time[4];
bit mode = 0;
TimeInfo xdata tinfo = {0, 0};
unsigned char xdata station_id = 1; // 当前站点ID，初始为列车id

void ResetTimer() {
    unsigned int xdata total_seconds;
    get_speed_res = GetSpeed();
    if (mode == 0 && station_id < 6) {
        total_seconds = dist[station_id++] * 3.6 / get_speed_res; // 正常行驶
    } else if (mode == 0 && station_id >= 6) {
        total_seconds = 1200; // 到终点站，显示据下一次发车的时间
    } else {
        total_seconds = 20;
    }
    rtc_time[id].minute = total_seconds / 60;
    rtc_time[id].second = total_seconds % 60;
    RTC_Write(rtc_time[id]);
}
void UpdateTime() {
    rtc_time[id] = RTC_Read();
     if (rtc_time[id].minute == 0 && rtc_time[id].second == 0) {
        mode = !mode;
        ResetTimer();
    } else if (rtc_time[id].second == 0) {
        rtc_time[id].second = 59;
        rtc_time[id].minute--;
    } else {
        rtc_time[id].second--;
    }
    RTC_Write(rtc_time[id]);
    tinfo.seconds = rtc_time[id].minute * 60 + rtc_time[id].second;
    tinfo.time_mode = mode;
}
void ShowTime() {
    rtc_time[id] = RTC_Read();
    Seg7Print(0, 0, 10, rtc_time[id].minute / 10, rtc_time[id].minute % 10, 10, rtc_time[id].second / 10, rtc_time[id].second % 10);
}
