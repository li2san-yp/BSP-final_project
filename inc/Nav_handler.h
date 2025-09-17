#ifndef NAV_HANDLER_H
#define NAV_HANDLER_H

extern char xdata displayMode;   // 显示模式：0=时间 1=温度 2=速度

extern void NavHandler();
extern void ShowStatus();

#endif // NAV_HANDLER_H