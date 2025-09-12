#include "STC15F2K60S2.H"        //±ØĞë
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
#include "ultrasonic.h"  // ÒıÈë³¬Éù²¨Ä£¿é     

code unsigned long SysClock=11059200;         //å¿…é¡»ã€‚å®šä¹‰ç³»ç»Ÿå·¥ä½œæ—¶é’Ÿé?‘ç‡(Hz)ï¼Œç”¨æˆ·å¿…é¡»ä¿®æ”¹æˆä¸å®é™…å·¥ä½œé?‘ç‡ï¼ˆä¸‹è½½æ—¶é€‰æ‹©çš„ï¼‰ä¸€è‡?

#ifdef _displayer_H_                          //æ˜¾ç¤ºæ¨¡å—é€‰ç”¨æ—¶å¿…é¡»ã€‚ï¼ˆæ•°ç ç®¡æ˜¾ç¤ºè¯‘ç è¡¨ï¼Œç”¨æˆ¶å¯ä¿?æ”¹ã€å?åŠ ç­‰ï¼‰ 
code char decode_table[]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,0x00,0x08,0x40,0x01, 0x41, 0x48, 
	              /* åºå·:   0   1    2	   3    4	    5    6	  7   8	   9	 10	   11		12   13    14     15     */
                /* æ˜¾ç¤º:   0   1    2    3    4     5    6    7   8    9  (æ—?)   ä¸?-  ä¸?-  ä¸?-  ä¸Šä¸­-  ä¸?ä¸?-   */  
	                       0x3f|0x80,0x06|0x80,0x5b|0x80,0x4f|0x80,0x66|0x80,0x6d|0x80,0x7d|0x80,0x07|0x80,0x7f|0x80,0x6f|0x80 };  
             /* å¸¦å°æ•°ç‚¹     0         1         2         3         4         5         6         7         8         9        */
#endif

int distan;

void my1s_callback(){
	// Ê¹ÓÃ³¬Éù²¨Ä£¿é»ñÈ¡¾àÀë
	distan = UltrasonicGetDistance();
	
	// Ê¹ÓÃ³¬Éù²¨Ä£¿éÏÔÊ¾¾àÀë
	UltrasonicDisplayDistance(distan);
}

void main() 
{ 
	// ³õÊ¼»¯³¬Éù²¨Ä£¿é
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