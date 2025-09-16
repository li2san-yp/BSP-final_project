#ifndef TEMP_MODULE_H
#define TEMP_MODULE_H

#include "ADC.H"

// extern unsigned char xdata temperature[4];  // 当前温度值，改为char节省内存
extern unsigned char xdata tempThresholds[4]; // 温度报警阈值，改为char节省内存
extern struct_ADC  xdata adc_temp;      // ADC温度传感器结构
extern int  xdata rt;                   // 电阻值
extern int  xdata temperature;                 // 温度值
extern code int tempdata[];      // 温度查找表

extern void ShowTemp();

#endif // TEMP_MODULE_H