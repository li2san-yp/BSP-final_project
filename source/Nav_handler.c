#include "STC15F2K60S2.H"  //���룬"STC-Bѧϰ��"ʹ��MCUָ����ͷ�ļ�
#include "sys.H"           //���룬sys��MySTC_OS���ȳ���ͷ�ļ�
#include "adc.h"           //��ѡ��adc��������������������������չ�ӿ�ADC��ͷ�ļ�
#include "Key.H"
#include "temp_module.h"   // 温度显示模块头文件
#include "rtc_module.h"
#include "speed_module.h"

// Use data segment for frequently accessed variables
unsigned char xdata displayMode = 0;   // 移至xdata段以节省data空间
// 导航键事件回调
void NavHandler()
{
    char xdata navUp = GetAdcNavAct(enumAdcNavKeyUp);
    char xdata navDown = GetAdcNavAct(enumAdcNavKeyDown);

    if (navUp == enumKeyPress) {     // 向上键
        displayMode++;
        if (displayMode > 2) displayMode = 0;  // 循环切换
    }
    if (navDown == enumKeyPress) {   // 向下键
        displayMode--;
        if (displayMode < 0) displayMode = 2;  // 循环切换
    }
}

void ShowStatus()
{
	switch (displayMode) {
      case 0: ShowTime();  break;
      case 1: ShowTemp();  break;
      case 2: ShowSpeed(); break;
  }
}

