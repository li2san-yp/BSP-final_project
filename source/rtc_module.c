#include "STC15F2K60S2.H"
#include "sys.H"
#include "DS1302.H"
#include "displayer.H"

extern int GetSpeed();
int vv;
unsigned int dist = 1737; // m
struct_DS1302_RTC rtc_time = {0, 0, 0, 0, 0, 0};
bit mode = 0; 

void ResetTimer()
{
    unsigned int total_seconds;
		vv = GetSpeed();
    if (mode == 0) {
        total_seconds = dist * 3.6 / vv; 
    } 
    else {
        total_seconds = 20; 
    }
    rtc_time.minute = total_seconds / 60;
    rtc_time.second = total_seconds % 60;
    RTC_Write(rtc_time);
}

void ShowTime()
{
    rtc_time = RTC_Read();
    if (rtc_time.minute == 0 && rtc_time.second == 0) {
        mode = !mode;
        ResetTimer();
    } 
	else if (rtc_time.second == 0) {
        rtc_time.second = 59;
        rtc_time.minute--;
    } 
	else {
        rtc_time.second--;
	}
    RTC_Write(rtc_time);

    Seg7Print(0, 0, 10, rtc_time.minute / 10, rtc_time.minute % 10, 10, rtc_time.second / 10, rtc_time.second % 10);
}
