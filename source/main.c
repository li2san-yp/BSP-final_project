#include "STC15F2K60S2.H"        //����
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
#include "uart2.h"
#include "ultrasonic.h"  // ���볬����ģ��     

code unsigned long SysClock=11059200;         //必须。定义系统工作时钟�?�率(Hz)，用户必须修改成与实际工作�?�率（下载时选择的）一�?

#ifdef _displayer_H_                          //显示模块选用时必须。（数码管显示译码表，用戶可�?改、�?�加等） 
code char decode_table[]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,0x00,0x08,0x40,0x01, 0x41, 0x48, 
	              /* 序号:   0   1    2	   3    4	    5    6	  7   8	   9	 10	   11		12   13    14     15     */
                /* 显示:   0   1    2    3    4     5    6    7   8    9  (�?)   �?-  �?-  �?-  上中-  �?�?-   */  
	                       0x3f|0x80,0x06|0x80,0x5b|0x80,0x4f|0x80,0x66|0x80,0x6d|0x80,0x7d|0x80,0x07|0x80,0x7f|0x80,0x6f|0x80 };  
             /* 带小数点     0         1         2         3         4         5         6         7         8         9        */
#endif

int distan;

void my1s_callback(){
	// ʹ�ó�����ģ���ȡ����
	distan = UltrasonicGetDistance();
	
	// ʹ�ó�����ģ����ʾ����
	UltrasonicDisplayDistance(distan);
}

void main() 
{ 
	// ��ʼ��������ģ��
	UltrasonicInit();
	
	DisplayerInit();
	SetDisplayerArea(0,7);
	Seg7Print(10,10,10,10,10,10,10,10);
	LedPrint(0x00);
	
	SetEventCallBack(enumEventSys1S, my1s_callback);
  MySTC_Init();	 
	while(1)             	
		{ MySTC_OS();    
		}	             
}    