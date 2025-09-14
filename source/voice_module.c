#include "STC15F2K60S2.H" 
#include "sys.H"  
#include "key.h"
#include "uart2.h"
#include "displayer.h"
#include "rtc_module.h"

unsigned char Uart2RxBuf; 
unsigned char Uart2Busy = 0; 
code unsigned char str1[] = "溁湾镇到了，本站为换乘站，可换乘2号线。下车时，请注意列车与站台之间的间隙。We are arriving at yingwanzhen, This station is a transfer station, and you can transfer to line two. Please get ready for your arrival and take your belongings. Please mind the platform gap when alighting.";
code unsigned char str2[] = "车门即将关闭，谨防夹伤。The door is closing.";
code unsigned char str3[] = "本次列车开往:杜家坪。This train is bound for dujiaping.下一站:溁湾镇。需换乘2号线的乘客请在溁湾镇下车。The next station is yingwanzhen. For passengers transfering to the line two, please exit at yingwanzhen."; 

void myUart2Rxd_callback() 
{ 
    if(Uart2RxBuf == 0x41) Uart2Busy=1;  
    else Uart2Busy=0; 
}

void CheckCountdownEvent()
{
    unsigned int remain;
    GetETA();  // 获取剩余时间 + 模式
    remain = tinfo.seconds;

    if (Uart2Busy == 0)  // 语音模块空闲才能播报
    {
        if (remain == 15 && tinfo.time_mode == 0) { // 在到站前15秒播放进站语音
            Uart2Print(str1, sizeof(str1));
        }
        else if (remain == 7 && tinfo.time_mode == 1) { // 在出站前7秒播放关门语音
            Uart2Print(str2, sizeof(str2));
        }
        else if (remain == 0 && tinfo.time_mode == 1) { // 在出站时播放出站语音
            Uart2Print(str3, sizeof(str3));
        }
    }
}
