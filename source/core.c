#include "core.h"

code unsigned long SysClock = 11059200;
xdata uchar is_door_open[4];   // 门状态，1=开门 0=关门，使用XDATA段
xdata uchar is_alarm[4];       // 报警状态，1=报警 0=正常，使用XDATA段
uchar id=1;

void my1S_callback(){
    // SetBeep(1500,50);
    ShowStatus();
    CheckCountdownEvent(); 
    //MyUart1SendCurrentStatus(); // 每1s发送地铁当前的状态
}
