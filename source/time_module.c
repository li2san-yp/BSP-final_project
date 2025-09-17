#include "core.h"
uchar xdata get_speed_res;
unsigned int xdata dist[5] = {0, 1009, 1078, 2037, 1536};  // m
struct_DS1302_RTC xdata rtc_time;
bit mode = 0;
TimeInfo xdata tinfo = {0, 0};
unsigned char xdata station_id = 1;     // 当前站点ID，初始为列车id
unsigned int xdata remain_distance = 0;     // 剩余距离，单位：米
bit change_speed_flag = 0;  // 速度是否改变

void InitRTC() {
    DS1302Init(rtc_time);
    station_id = id; 
    mode = 1;
    rtc_time.minute = 0;
    rtc_time.second = 1;
    remain_distance = 0;
}
void ResetTimer() {
    unsigned int xdata total_seconds;
    get_speed_res = GetSpeed();
    
    if (mode == 0 && station_id < 5) {
        remain_distance = dist[station_id];  // 设置当前站点到下一站的距离
        if (get_speed_res > 0) {
            total_seconds = remain_distance * 3.6 / get_speed_res;  // 初始预计时间
        } else {
            total_seconds = 3600;  // 如果速度为0，说明出现异常
            Beep(100,100);
        }
        station_id++;
    } else if (mode == 0 && station_id >= 5) {
        total_seconds = 1200;  // 到终点站，显示距下一次发车的时间
        remain_distance = 0;
    } else {
        total_seconds = 20;    // 停站时间
        //remain_distance = 0;
    }
    
    rtc_time.minute = total_seconds / 60;
    rtc_time.second = total_seconds % 60;
    RTC_Write(rtc_time);
}
void UpdateTime() {
    unsigned int xdata total_seconds;
    unsigned char xdata current_speed;
    unsigned int xdata traveled_distance;
    rtc_time = RTC_Read();
    total_seconds = rtc_time.minute * 60 + rtc_time.second;
    
    // 检查是否需要重置计时器
    if (total_seconds == 0) {
        mode = !mode;
        ResetTimer();
        return;
    }
    
    // 在行驶模式下实时更新时间
    if (mode == 0 && remain_distance > 0) {
        if (!change_speed_flag) { 
            // 速度未变化，正常倒计时
            total_seconds--;
        } else {
            // 速度变化，调整剩余距离和时间
            current_speed = GetSpeed();
            traveled_distance = current_speed / 3.6; // 计算已行驶距离（1秒钟的距离）
            
            // 更新剩余距离
            if (remain_distance > traveled_distance) {
                remain_distance -= traveled_distance;
            } else {
                remain_distance = 0;
            }
            
            // 根据当前速度和剩余距离重新计算时间
            if (current_speed > 0) {
                total_seconds = (remain_distance * 3.6) / current_speed;
            }
            change_speed_flag = 0;
        }
    } else {
        // 停站模式或终点站模式，正常倒计时
        total_seconds--;
    }
    
    // 更新RTC时间
    rtc_time.minute = total_seconds / 60;
    rtc_time.second = total_seconds % 60;
    RTC_Write(rtc_time);
    
    // 更新全局时间信息
    tinfo.seconds = total_seconds;
    tinfo.time_mode = mode;
}
void ShowTime() {
    rtc_time = RTC_Read();
    Seg7Print(0, 0, 10, rtc_time.minute / 10, rtc_time.minute % 10, 10, rtc_time.second / 10, rtc_time.second % 10);
}

void ShowMode()
{
    Seg7Print(10, 10, 10, 10, 10, 10, 10, tinfo.time_mode);
}
