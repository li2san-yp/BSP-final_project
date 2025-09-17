#include "core.h"

void main() {
    AdcInit(ADCexpEXT);
    DisplayerInit();
    KeyInit();
    BeepInit();
    SetDisplayerArea(0, 7);
    Uart2Init(9600, Uart2UsedforEXT);
    MyUart1Init();  
    NVTempThresholdInit();
    SetTrainID(1);
    InitTrainID();                // 初始化列车ID
    InitRTC();
    //EXTInit(enumEXTPWM);
    //UltrasonicInit();               // 超声波模块   
    Radio_Init();               // 收音机模块
    SetUart2Rxd(&Uart2RxBuf, 1, &Uart2RxBuf,0);  // 单字节接收，不匹配包头
    SetEventCallBack(enumEventUart2Rxd, myUart2Rxd_callback);
    SetEventCallBack(enumEventSys1S, my1S_callback);
    SetEventCallBack(enumEventSys100mS, my100ms_callback);
    SetEventCallBack(enumEventNav, NavHandler);
    MySTC_Init();
    // LedPrint(0);
    while (1) {
        MySTC_OS();
    }
}
