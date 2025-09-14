#ifndef RTC_MODULE_H
#define RTC_MODULE_H

typedef struct {
    unsigned int seconds; // 剩余秒数
    unsigned char time_mode; // 0 = dist/vv 模式，1 = 20s 模式
} TimeInfo;
extern TimeInfo tinfo;
void GetETA(void);
void ShowTime(void);

#endif // RTC_MODULE_H