#include "core.h"

unsigned int xdata real_hour;
unsigned int xdata real_minute;
unsigned int xdata real_second;
void ShowRealTime()
{
    Seg7Print(real_hour / 10, real_hour % 10, 0, real_minute / 10, real_minute % 10, 0, real_second / 10, real_second % 10);
}