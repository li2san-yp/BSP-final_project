#include "core.h"

//#define VOICE_STRING_1 "溁湾镇到了，本站为换乘站，可换乘2号线。下车时，请注意列车与站台之间的间隙。"
#define VOICE_STRING_2 "车门即将关闭，谨防夹伤。"
#define VOICE_STRING_3 "本次列车开往，杜家坪。下一站:溁湾镇。需换乘2号线的乘客请在溁湾镇下车。"
// 串口2接收字节缓冲区 - 按照标准示例使用单字节
unsigned char Uart2RxBuf=0;      // 串口2接收字节
unsigned char Uart2Busy = 0;   // =0：语言合成模块空闲  =1：语言合成模块正在播放语音中

code unsigned char VOICE_STRING_1[] = {
  0xE6,0xF9,0xCD,0xE5,0xD5,0xF2,0xB5,0xBD,0xC1,0xCB,0xA3,0xAC,
  0xB1,0xBE,0xD5,0xBE,0xCE,0xAA,0xBB,0xBB,0xB3,0xD6,0xD5,0xBE,
  0xA3,0xAC,0xBF,0xC9,0xBB,0xBB,0xB3,0xD6,0xBA,0xC5,0xCF,0xDF,
  0xA1,0xA3,0xCF,0xC2,0xB3,0xB5,0xCA,0xB1,0xA3,0xAC,0xC7,0xEB,
  0xD7,0xA2,0xD2,0xE2,0xC1,0xF7,0xB3,0xCC,0xD3,0xEB,0xD5,0xBE,
  0xCC,0xA8,0xD6,0xAE,0xBC,0xE4,0xB5,0xC4,0xBC,0xE4,0xCF,0xDF,
  0xA1,0xA3,0x00
};

// 串口2接收字节回调函数 - 简化调试版本
void myUart2Rxd_callback()
{
    if (Uart2RxBuf == 0x41)  
        Uart2Busy = 1;  // 模块忙碌
    else
        Uart2Busy = 0;  // 默认为空闲
}

// 检查倒计时事件并播放对应语音
void CheckCountdownEvent()
{
    unsigned int rest_total_seconds;
    // 如果语音模块正忙，直接返回，不进行新的播放
    if (Uart2Busy == 1)
        return;
    GetETA(); // 获取剩余时间 + 模式
    rest_total_seconds = tinfo.seconds;
    
    // 在特定时间点播放对应语音 - 使用字符串字面量与正确示例保持一致
    if (rest_total_seconds == 15 && tinfo.time_mode == 0)
    {
        Uart2Print(VOICE_STRING_1,sizeof(VOICE_STRING_1));
    }
    else if (rest_total_seconds == 7 && tinfo.time_mode == 1)
    {
        Uart2Print(VOICE_STRING_2, sizeof(VOICE_STRING_2));
    }
    else if (rest_total_seconds == 0 && tinfo.time_mode == 1)
    {
        Uart2Print(VOICE_STRING_3, sizeof(VOICE_STRING_3));
    }
}
