#include "core.h"
bit mode = 0;
unsigned int xdata dist[5] = {0, 1009, 1078, 2037, 1536}; // m
int xdata station_id;                                     // 当前站点ID
unsigned int xdata remain_distance = 0;                   // 剩余距离，单位：米
int xdata rest_total_seconds;
unsigned int xdata rest_arrive_minute;
unsigned int xdata rest_arrive_second;
code unsigned char tmp[]={0xC6, 0xBD, 0xD1, 0xF4, 0xA3, 0xAC, 0xB5, 0xBD, 0xC1, 0xCB, 0xA1, 0xA3, 0xC7, 0xEB, 0xC7, 0xB0, 0xCD, 0xF9, 0xC0, 0xE8, 0xCD, 0xD0, 0xC2, 0xB7, 0xB5, 0xC4, 0xB3, 0xCB, 0xBF, 0xCD, 0xD7, 0xBC, 0xB1, 0xB8, 0xCF, 0xC2, 0xB3, 0xB5, 0xA1, 0xA3, 0xCF, 0xC2, 0xB3, 0xB5, 0xCA, 0xB1, 0xA3, 0xAC, 0xC7, 0xEB, 0xD7, 0xA2, 0xD2, 0xE2, 0xC1, 0xD0, 0xB3, 0xB5, 0xD3, 0xEB, 0xD5, 0xBE, 0xCC, 0xA8, 0xD6, 0xAE, 0xBC, 0xE4, 0xB5, 0xC4, 0xBC, 0xE4, 0xCF, 0xB6, 0xA1, 0xA3, 0x00};
void InitRTC()
{
    station_id = id - 1; // 初始站点为列车ID-1
    mode = 1;
    remain_distance = 0;
    rest_total_seconds = 1;
}

void UpdateTime()
{
    rest_total_seconds--;

    rest_arrive_minute = rest_total_seconds / 60;
    rest_arrive_second = rest_total_seconds % 60;

    // 检查是否需要重置计时器
    if (rest_total_seconds <= 0)
    {
        mode = !mode;
        ResetTimer();
        if (mode == 0 && station_id > 0 && station_id < 5)
        {
            Uart2Print(VOICE_STRING_3[station_id], sizeof(VOICE_STRING_3[station_id])); // yes
        }
        return;
    }
    // 在行驶模式下实时更新时间
}

void ResetTimer()
{

    if (mode == RUNNING_MODE && station_id < 5)
    {
        station_id++;
        remain_distance = dist[station_id]; // 设置当前站点到下一站的距离
        if (speed[id] > 0)
        {
            rest_total_seconds = remain_distance / speed[id]; // 初始预计时间
        }
    }
    else if (mode == RUNNING_MODE && station_id >= 5)
    {
        rest_total_seconds = 1200; // 到终点站，显示距下一次发车的时间
        remain_distance = 0;
    }
    else
    {
        rest_total_seconds = 21; // 停站时间
        // remain_distance = 0;
    }
}


void ShowTime()
{
    Seg7Print(0, 0, 10, rest_arrive_minute / 10, rest_arrive_minute % 10, 10, rest_arrive_second / 10, rest_arrive_second % 10);
}

void ShowMode()
{
    if (mode == 0) { // 行驶模式
        Seg7Print(10, 56, 59, 52, 52, 50, 52, 48); // 显示0表示行驶
        return;
    } else { // 停站模式
        Seg7Print(57, 58, 53, 54, 54, 50, 52, 48); // 显示1表示停站
        return;
    }
}

void onSpeedChange()
{
    if (mode == RUNNING_MODE && remain_distance > 0)
    {
        if (speed[id] > 0)
        {
            // 更新剩余距离
            if (remain_distance > speed[id])
            {
                remain_distance -= speed[id];
            }
            else
            {
                remain_distance = 0;
            }
            // 根据当前速度和剩余距离重新计算时间
            rest_total_seconds = remain_distance / speed[id];
        }
        else
        {
            // 速度为0，出现异常
            // Beep(100,100);
            rest_total_seconds = 3600;
        }
    }
}