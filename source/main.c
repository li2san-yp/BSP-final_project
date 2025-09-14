#include "STC15F2K60S2.H"  //���룬"STC-Bѧϰ��"ʹ��MCUָ����ͷ�ļ�
#include "sys.H"           //���룬sys��MySTC_OS���ȳ���ͷ�ļ�
#include "displayer.H"     //��ѡ��display����ʾģ�飩ͷ�ļ���
#include "key.H"           //��ѡ��key������ģ�飩ͷ�ļ���
#include "IR.h"            //��ѡ��38KHz����ͨ��
#include "Vib.h"           //��ѡ��Vib���񶯴�����ģ�飩ͷ�ļ���
#include "adc.h"           //��ѡ��adc��������������������������չ�ӿ�ADC��ͷ�ļ�
#include "beep.H"          //��ѡ��beep��������ģ�飩ͷ�ļ���
#include "hall.H"          //��ѡ��hall������������ģ�飩ͷ�ļ���
#include "music.h"         //��ѡ��music�����ֲ��ţ�ͷ�ļ���
#include "stepmotor.h"     //��ѡ���������
#include "uart1.h"         //��ѡ��uart1������1ͨ�ţ�ͷ�ļ���
#include "uart2.h"         //��ѡ��uart2������2ͨ�ţ�ͷ�ļ���
#include "Nav_handler.h"   // 导航键处理模块头文件
#include "rtc_module.h"
#include "voice_module.h" // 语音模块头文件


code unsigned long SysClock = 11059200;  // ���롣����ϵͳ����ʱ��Ƶ��(Hz)���û������޸ĳ���ʵ�ʹ���Ƶ�ʣ�����ʱѡ��ģ�һ��
#ifdef _displayer_H_                     // ��ʾģ��ѡ��ʱ���롣���������ʾ��������Ñ����޸ġ����ӵȣ�
code char decode_table[] = {
    // [0]-[9] ����
    0x3f, 0x06, 0x5b, 0x4f, 0x66,
    0x6d, 0x7d, 0x07, 0x7f, 0x6f,

    0x00,  // ��- index = 10
    0x08,  // ��- index = 11
    0x40,  // ��- index = 12
    0x01,  // ��- index = 13
    0x41,  // ����- index = 14
    0x48,  // ����- index = 15

    // [16]-[25] ��С���������
    0x3f | 0x80, 0x06 | 0x80, 0x5b | 0x80, 0x4f | 0x80, 0x66 | 0x80, 0x6d | 0x80, 0x7d | 0x80, 0x07 | 0x80, 0x7f | 0x80, 0x6f | 0x80,
    // 'A' (��д A)��index = 26
    0x77,
    // 'B' (��д B)��index = 27
    0x7f,
    // 'C' (��д C)��index = 28
    0x39,
    // 'D' (��д D)��index = 29
    0x3f,
    // 'E' (��д E)��index = 30
    0x79,
    // 'F' (��д F)��index = 31
    0x71,
    // 'G' (��д G)��index = 32
    0x7D,
    // 'H' (��д H)��index = 33
    0x76,
    // 'I' (��д I)��index = 34
    0x06,
    // 'J' (��д J)��index = 35
    0x1E,
    // 'L' (��д L)��index = 36
    0x38,
    // 'O' (��д O)��index = 37
    0x3F,
    // 'P' (��д P)��index = 38
    0x73,
    // 'S' (��д S)��index = 39
    0x6D,
    // 'U' (��д U)��index = 40
    0x3E,
    // 'Y' (��д Y)��index = 41
    0x6E,

    // 'a' (Сд a)��index = 42
    0x5F,
    // 'b' (Сд b)��index = 43
    0x7C,
    // 'c' (Сд c)��index = 44
    0x58,
    // 'd' (Сд d)��index = 45
    0x5E,
    // 'e' (Сд e)��index = 46
    0x7B,
    // 'f' (Сд f)��index = 47
    0x71,
    // 'g' (Сд g)��index = 48
    0x6F,
    // 'h' (Сд h)��index = 49
    0x74,
    // 'i' (Сд i)��index = 50
    0x10,
    // 'l' (Сд l)��index = 51
    0x38,
    // 'n' (Сд n)��index = 52
    0x54,
    // 'o' (Сд o)��index = 53
    0x5C,
    // 'p' (Сд p)��index = 54
    0x73,
    // 'q' (Сд q)��index = 55
    0x67,
    // 'r' (Сд r)��index = 56
    0x50,
    // 's' (Сд s)��index = 57
    0x6D,
    // 't' (Сд t)��index = 58
    0x78,
    // 'u' (Сд u)��index = 59
    0x1C,
    // 'y' (Сд y)��index = 60
    0x6E};

#endif		

void main() {
	AdcInit(ADCincEXT);         //ģ��ת��ADCģ�����������¶ȡ����ա����������밴��Key3��EXT��չ�ӿ��ϵ�ADC��
	DisplayerInit();      //��ʾģ������ 
	KeyInit();           //����ģ������
	//BeepInit();	      //������ģ������
	//MusicPlayerInit();    //������������������
	//HallInit();          //����������ģ������                      
	//VibInit();          //�񶯴�����ģ������  
	//StepMotorInit();    //�������ģ������	                  
    // IrInit(NEC_R05d);         //38KHz����ͨ��ģ������
    SetDisplayerArea(0, 7);
    Uart2Init(2400, Uart2UsedforEXT); 
    SetUart2Rxd(&Uart2RxBuf, 1, &Uart2RxBuf,0);
    SetEventCallBack(enumEventNav, NavHandler);
	SetEventCallBack(enumEventSys1S, ShowStatus);
    //SetEventCallBack(enumEventKey, KeyHandler);
	MySTC_Init(); // ϵͳ��ʼ�������롣
    while (1) {
        MySTC_OS();
    }
}
