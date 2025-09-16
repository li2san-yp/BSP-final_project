#include "core.h"

code unsigned long SysClock = 11059200;
xdata uchar is_door_open[4]={0,0,0,0};   // 门状态，1=开门 0=关门，使用XDATA段
xdata uchar is_alarm[4]={0,0,0,0};       // 报警状态，1=报警 0=正常，使用XDATA段
xdata uchar id=1;

void my1S_callback(){
    UpdateTime();                     // 更新时间
    UpdateTemp();                     // 更新温度
    GetSpeed();                     // 更新速度
    ShowStatus();                     // 显示当前状态
    CheckCountdownEvent();               // 检查倒计时

    // 发送状态
    MyUart1SendCurrentStatus();          // 每1s发送地铁当前的状态
}
