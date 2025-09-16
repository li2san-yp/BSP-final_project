#include "core.h"

void main() {
    AdcInit(ADCexpEXT);
    DisplayerInit();
    KeyInit();
    SetDisplayerArea(0, 7);
    Uart2Init(9600, Uart2UsedforEXT);
    MyUart1Init();  
    EXTInit(enumEXTPWM);
    SetUart2Rxd(&Uart2RxBuf, 1, &Uart2RxBuf,0);  // 单字节接收，不匹配包头
    SetEventCallBack(enumEventSys1S, my1S_callback);
    SetEventCallBack(enumEventUart2Rxd, myUart2Rxd_callback);
    SetEventCallBack(enumEventNav, NavHandler);
    MySTC_Init();
    LedPrint(0);
    while (1) {
        MySTC_OS();
        DC_motor();  // 添加温控风扇控制
    }
}
