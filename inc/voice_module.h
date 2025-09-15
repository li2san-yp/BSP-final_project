#ifndef VOICE_MODULE_H
#define VOICE_MODULE_H

extern unsigned char Uart2RxBuf;            // 串口2接收字节 - 改为单字节
extern unsigned char Uart2Busy;             // =0：语言合成模块空闲  =1：语言合成模块正在播放语音中

extern void myUart2Rxd_callback();
extern void CheckCountdownEvent();

#endif // VOICE_MODULE_H