#include "IR.h"
#include "Nav_handler.h"
#include "STC15F2K60S2.H"
#include "Vib.h"
#include "adc.h"
#include "beep.H"
#include "displayer.H"
#include "hall.H"
#include "key.H"
#include "music.h"
#include "rtc_module.h"
#include "stepmotor.h"
#include "sys.H"
#include "uart1.h"
#include "uart2.h"
#include "voice_module.h"
code unsigned long SysClock = 11059200;
#ifdef _displayer_H_
code char decode_table[] = {
    0x3f, 0x06, 0x5b, 0x4f, 0x66,
    0x6d, 0x7d, 0x07, 0x7f, 0x6f,
    0x00,
    0x08,
    0x40,
    0x01,
    0x41,
    0x48,
    0x3f | 0x80, 0x06 | 0x80, 0x5b | 0x80, 0x4f | 0x80, 0x66 | 0x80, 0x6d | 0x80, 0x7d | 0x80, 0x07 | 0x80, 0x7f | 0x80, 0x6f | 0x80,
    // 大写
    0x77,  // A
    0x7C,  // B
    0x39,  // C
    0x5E,  // D
    0x79,  // E
    0x71,  // F
    0x3D,  // G
    0x76,  // H
    0x06,  // I
    0x38,  // L
    0x3F,  // O
    0x73,  // P
    0x6D,  // S
    0x3E,  // U
    0x6E,  // Y

    // 小写
    0x5F,  // a
    0x7C,  // b
    0x58,  // c
    0x5E,  // d
    0x7B,  // e
    0x71,  // f
    0x6F,  // g
    0x74,  // h
    0x10,  // i
    0x38,  // l
    0x54,  // n
    0x5C,  // o
    0x73,  // p
    0x67,  // q
    0x50,  // r
    0x6D,  // s
    0x78,  // t
    0x1C,  // u
    0x6E,  // y
};
#endif
void main() {
    AdcInit(ADCincEXT);
    DisplayerInit();
    KeyInit();
    SetDisplayerArea(0, 7);
    Uart2Init(2400, Uart2UsedforEXT);
    SetUart2Rxd(&Uart2RxBuf, 1, &Uart2RxBuf, 0);
    SetEventCallBack(enumEventNav, NavHandler);
    SetEventCallBack(enumEventSys1S, ShowStatus);
    MySTC_Init();
    while (1) {
        MySTC_OS();
    }
}
