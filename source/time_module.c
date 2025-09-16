#include "core.h"
uchar xdata get_speed_res;
unsigned int xdata dist[5] = {0, 1009, 1078, 2037, 1536};  // m
struct_DS1302_RTC xdata rtc_time;
bit mode = 0;
TimeInfo xdata tinfo = {0, 0};
unsigned char xdata station_id = 1; // 当前站点ID，初始为列车id

void InitRTC() {
    DS1302Init(rtc_time);
    station_id = id; 
    mode = 1;
    rtc_time.minute = 0;
    rtc_time.second = 1;
}
void ResetTimer() {
    unsigned int xdata total_seconds;
    get_speed_res = GetSpeed();
    if (mode == 0 && station_id < 6) {
        total_seconds = dist[station_id++] * 3.6 / get_speed_res; // 正常行驶
    } else if (mode == 0 && station_id >= 6) {
        total_seconds = 1200; // 到终点站，显示距下一次发车的时间
    } else {
        total_seconds = 20;
    }
    rtc_time.minute = total_seconds / 60;
    rtc_time.second = total_seconds % 60;
    RTC_Write(rtc_time);
}
void UpdateTime() {
    // 计算总秒数，以确保连续性
    unsigned int total_seconds = rtc_time.minute * 60 + rtc_time.second;
    
    rtc_time = RTC_Read();
    // 检查是否需要重置计时器
    if (total_seconds == 0) {
        mode = !mode;
        ResetTimer();
    } else {
        // 减少一秒
        total_seconds--;
        // 重新计算分和秒
        rtc_time.minute = total_seconds / 60;
        rtc_time.second = total_seconds % 60;
        // 写入新的时间
        RTC_Write(rtc_time);
    }
    // 更新全局时间信息
    tinfo.seconds = total_seconds;
    tinfo.time_mode = mode;
}
void ShowTime() {
    rtc_time = RTC_Read();
    Seg7Print(0, 0, 10, rtc_time.minute / 10, rtc_time.minute % 10, 10, rtc_time.second / 10, rtc_time.second % 10);
}
