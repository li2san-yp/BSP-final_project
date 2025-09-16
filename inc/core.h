#ifndef CORE_H
#define CORE_H

#define uchar unsigned char
#define uint unsigned int

#include "IR.h"
#include "STC15F2K60S2.H"
#include "Vib.h"
#include "adc.h"
#include "beep.h"
#include "displayer.H"
#include "hall.H"
#include "key.H"
#include "music.h"
#include "stepmotor.h"
#include "sys.H"
#include "uart1.h"
#include "uart2.h"
#include "EXT.H"
#include "M24C02.h"

#include "temp_module.h"
#include "speed_module.h"
#include "Nav_handler.h"
#include "rtc_module.h"
#include "temp_controlled_fan.h"
#include "voice_module.h"
#include "ultrasonic_module.h"
#include "radio.h"
#include "my_uart1.h"
#include "nv_temp_threshold.h"

// 添加直流电机温控模块

extern code unsigned long SysClock;
extern xdata uchar is_door_open[4];   // 门状态，1=开门 0=关门
extern xdata uchar is_alarm[4];        // 报警状态，1=报警 0=正常
extern xdata unsigned char id;

void my1S_callback();

#endif // CORE_H