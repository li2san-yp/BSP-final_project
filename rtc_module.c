#include "STC15F2K60S2.H"  //必须，"STC-B学习板"使用MCU指定的头文件
#include "sys.H"           //必须，sys（MySTC_OS调度程序）头文件
#include "DS1302.H"
#include "displayer.H"

struct_DS1302_RTC rtc_time = {0, 3, 0, 0, 0, 0};
void GetTime()
{
    rtc_time = RTC_Read();
		if (rtc_time.second == 0) {
        rtc_time.second = 0x59;
        if (rtc_time.minute == 0) {
            rtc_time.second = 0;
            rtc_time.minute = 0;
        } 
				else {
            rtc_time.minute--;
            if (rtc_time.minute % 16 == 0xF) {
                rtc_time.minute -= 6;
            }
        }
    } 
		else {
        rtc_time.second--;
        if (rtc_time.second % 16 == 0xF) {
            rtc_time.second -= 6;
        }
    }
    RTC_Write(rtc_time);
    Seg7Print(0, 0, 10, rtc_time.minute/16, rtc_time.minute%16, 10, rtc_time.second/16, rtc_time.second%16);
}

