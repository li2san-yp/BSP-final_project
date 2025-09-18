#include "core.h"

code unsigned long SysClock = 11059200;
xdata uchar is_door_open[4] = {1,1,1,1}; // 门状态，1=开门 0=关门，使用XDATA段
xdata uchar is_alarm[4] = {0, 0, 0, 0};     // 报警状态，1=报警 0=正常，使用XDATA段
xdata char id;

void my1S_callback()
{
    // Uart2Print(VOICE_STRING_1[1], sizeof(VOICE_STRING_1[1]));

    UpdateTime();          // 更新时间
    CheckCountdownEvent(); // 检查倒计时
    UpdateTemp();          // 更新温度
    // UltrasonicUpdateAndDisplay(); // 更新并显示超声波数据
    is_play_music();
    MyUart1SendCurrentStatus(); // 每1s发送地铁当前的状态
    temp_controlled_motor();
    UpdateRealTimePerSecond(); // 更新时间
}

void my100ms_callback()
{
    ShowStatus(); // 显示当前状态
}