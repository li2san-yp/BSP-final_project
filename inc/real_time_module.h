#include "core.h"

#ifndef _GET_REAL_TIME_H_
#define _GET_REAL_TIME_H_

extern unsigned int xdata real_hour;
extern unsigned int xdata real_minute;
extern unsigned int xdata real_second;

extern void ShowRealTime();
extern void UpdateRealTimePerSecond();

#endif // _GET_REAL_TIME_H_