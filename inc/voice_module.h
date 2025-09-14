#ifndef VOICE_MODULE_H
#define VOICE_MODULE_H
extern unsigned char Uart2RxBuf; 
extern unsigned char Uart2Busy;
void myUart2Rxd_callback();
void CheckCountdownEvent();
#endif // VOICE_MODULE_H