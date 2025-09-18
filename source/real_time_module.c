#include "core.h"

unsigned int xdata real_hour;
unsigned int xdata real_minute;
unsigned int xdata real_second;

void UpdateRealTimePerSecond(){
    real_second++;
    if(real_second>=60){
        real_second=0;
        real_minute++;
        if(real_minute>=60){
            real_minute=0;
            real_hour++;
            if(real_hour>=24){
                real_hour=0;
            }
        }
    }
}

void ShowRealTime()
{
    Seg7Print(real_hour / 10, real_hour % 10, 12, real_minute / 10, real_minute % 10, 12, real_second / 10, real_second % 10);
}