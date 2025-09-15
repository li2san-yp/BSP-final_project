#include "STC15F2K60S2.H"        //必须
#include "sys.H"
#include "adc.H" 
#include "beep.H"        
#include "displayer.H" 
#include "DS1302.H" 
#include "EXT.H" 
#include "FM_Radio.H"
#include "hall.H"
#include "ir.H" 
#include "key.H"        
#include "m24c02.H" 
#include "music.H" 
#include "vib.H" 
#include "uart1.H"
#include "uart2.h"'
#include "ultrasonic.h"  // 引入超声波模块
#include "radio.h"       // 引入收音机模块     

code unsigned long SysClock=11059200;         //必须。定义系统工作时钟频率(Hz)，用户必须修改成与实际工作频率（下载时选择的）一致

#ifdef _displayer_H_                          //显示模块选用时必须。（数码管显示译码表，用戶可修改、增加等） 
code char decode_table[]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,0x00,0x08,0x40,0x01, 0x41, 0x48, 
	              /* 序号:   0   1    2	   3    4	    5    6	  7   8	   9	 10	   11		12   13    14     15     */
                /* 显示:   0   1    2    3    4     5    6    7   8    9  (无)   下-  中-  上-  上中-  中下-   */  
	                       0x3f|0x80,0x06|0x80,0x5b|0x80,0x4f|0x80,0x66|0x80,0x6d|0x80,0x7d|0x80,0x07|0x80,0x7f|0x80,0x6f|0x80 };  
             /* 带小数点     0         1         2         3         4         5         6         7         8         9        */
#endif

// ===== 简洁的回调函数（只调用封装好的函数）=====

/**
 * @brief 1秒定时回调函数
 * @note 只调用封装的逻辑函数，不包含具体逻辑
 */
void my1s_callback(void) {
    UltrasonicUpdateAndDisplay();  // 调用封装的主逻辑函数
}

/**
 * @brief 按键回调函数
 * @note 只调用封装的按键处理函数，不包含具体逻辑
 */
void myKey_callback(void) {
    // 直接调用超声波模块的按键处理函数
    // 该函数内部会检查所有按键状态
    UltrasonicHandleKeyInput(0);  // 参数在新版本中不使用
}

/**
 * @brief 导航键回调函数
 * @note 只调用封装的收音机导航处理函数
 */
void myNav_callback(void) {
    Radio_HandleNavigation();  // 重新启用收音机导航处理函数
}

void main(void) 
{ 
	MySTC_Init();                    // 系统初始化，必须放在最前面
	
	DisplayerInit();                 // 数码管显示初始化
	KeyInit();                       // 初始化按键模块
	BeepInit();                      // 初始化蜂鸣器模块
	AdcInit(ADCexpEXT);             // 初始化ADC（包含导航键功能）
	
	// 初始化收音机模块（封装调用）
	Radio_Init();  // 重新启用收音机模块
	
	// 初始化超声波自动门模块
	UltrasonicInit();

	SetDisplayerArea(0,7);
	Seg7Print(10,10,10,10,10,10,10,10);
	LedPrint(0x00);
	
	// 注册回调函数
	SetEventCallBack(enumEventSys1S, my1s_callback);   // 1秒定时回调
	SetEventCallBack(enumEventKey, myKey_callback);     // 按键回调
	SetEventCallBack(enumEventNav, myNav_callback);     // 导航键回调（音量控制）
	 
	while(1) {
		MySTC_OS();    
	}	             
}