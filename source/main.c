#include "STC15F2K60S2.H"  //必须，"STC-B学习板"使用MCU指定的头文件
#include "sys.H"           //必须，sys（MySTC_OS调度程序）头文件
#include "displayer.H"     //可选，display（显示模块）头文件。
#include "key.H"           //可选，key（按键模块）头文件。
#include "IR.h"            //可选，38KHz红外通信
#include "Vib.h"           //可选，Vib（振动传感器模块）头文件。
#include "adc.h"           //可选，adc（热敏、光敏、导航按键、扩展接口ADC）头文件
#include "beep.H"          //可选，beep（蜂鸣器模块）头文件。
#include "hall.H"          //可选，hall（霍尔传感器模块）头文件。
#include "music.h"         //可选，music（音乐播放）头文件。
#include "stepmotor.h"     //可选，步进电机
#include "uart1.h"         //可选，uart1（串口1通信）头文件。
#include "uart2.h"         //可选，uart2（串口2通信）头文件。
#include "EXT.h"           //EXT（扩展接口）头文件。
#include "ultrasonic_bsp.h" //自定义，超声波测距模块头文件


code unsigned long SysClock = 11059200;  // 必须。定义系统工作时钟频率(Hz)，用户必须修改成与实际工作频率（下载时选择的）一致
#ifdef _displayer_H_                     // 显示模块选用时必须。（数码管显示译码表，用戶可修改、增加等）
code char decode_table[] = {
    // [0]-[9] 数字
    0x3f, 0x06, 0x5b, 0x4f, 0x66,
    0x6d, 0x7d, 0x07, 0x7f, 0x6f,

    0x00,  // 无- index = 10
    0x08,  // 下- index = 11
    0x40,  // 中- index = 12
    0x01,  // 上- index = 13
    0x41,  // 上中- index = 14
    0x48,  // 中下- index = 15

    // [16]-[25] 带小数点的数字
    0x3f | 0x80, 0x06 | 0x80, 0x5b | 0x80, 0x4f | 0x80, 0x66 | 0x80, 0x6d | 0x80, 0x7d | 0x80, 0x07 | 0x80, 0x7f | 0x80, 0x6f | 0x80,
    // 'A' (大写 A)，index = 26
    0x77,
    // 'B' (大写 B)，index = 27
    0x7f,
    // 'C' (大写 C)，index = 28
    0x39,
    // 'D' (大写 D)，index = 29
    0x3f,
    // 'E' (大写 E)，index = 30
    0x79,
    // 'F' (大写 F)，index = 31
    0x71,
    // 'G' (大写 G)，index = 32
    0x7D,
    // 'H' (大写 H)，index = 33
    0x76,
    // 'I' (大写 I)，index = 34
    0x06,
    // 'J' (大写 J)，index = 35
    0x1E,
    // 'L' (大写 L)，index = 36
    0x38,
    // 'O' (大写 O)，index = 37
    0x3F,
    // 'P' (大写 P)，index = 38
    0x73,
    // 'S' (大写 S)，index = 39
    0x6D,
    // 'U' (大写 U)，index = 40
    0x3E,
    // 'Y' (大写 Y)，index = 41
    0x6E,

    // 'a' (小写 a)，index = 42
    0x5F,
    // 'b' (小写 b)，index = 43
    0x7C,
    // 'c' (小写 c)，index = 44
    0x58,
    // 'd' (小写 d)，index = 45
    0x5E,
    // 'e' (小写 e)，index = 46
    0x7B,
    // 'f' (小写 f)，index = 47
    0x71,
    // 'g' (小写 g)，index = 48
    0x6F,
    // 'h' (小写 h)，index = 49
    0x74,
    // 'i' (小写 i)，index = 50
    0x10,
    // 'l' (小写 l)，index = 51
    0x38,
    // 'n' (小写 n)，index = 52
    0x54,
    // 'o' (小写 o)，index = 53
    0x5C,
    // 'p' (小写 p)，index = 54
    0x73,
    // 'q' (小写 q)，index = 55
    0x67,
    // 'r' (小写 r)，index = 56
    0x50,
    // 's' (小写 s)，index = 57
    0x6D,
    // 't' (小写 t)，index = 58
    0x78,
    // 'u' (小写 u)，index = 59
    0x1C,
    // 'y' (小写 y)，index = 60
    0x6E};

#endif

void my1s_callback(void) {
    static unsigned char led_state = 0;
    
    // LED 闪烁指示回调正在运行
    led_state = ~led_state;
    LedPrint(led_state ? 0x01 : 0x00);
    
    int distan = ultrasonic_get_distance();
    if (distan < 10) {
        Seg7Print(10,10,10,10,distan,10,10,10);
    } else if (distan < 100) {
        Seg7Print(10,10,10,distan/10,distan%10,10,10,10);
    } else if (distan < 1000) {
        Seg7Print(10,10,10,distan/100,distan/10%10,distan%10,10,10);
    } else {
        Seg7Print(10,10,10,12,12,10,10,10); // 超出范围显示特殊符号
    }
}

void myKey_callback(void) {
    
}

void main() {
    MySTC_Init(); // 系统初始化，必须。
	DisplayerInit();      //显示模块驱动 
	KeyInit();           //按键模块驱动
	BeepInit();	      //蜂鸣器模块驱动
	MusicPlayerInit();    //蜂鸣器播放音乐驱动
	HallInit();          //霍尔传感器模块驱动                      
	VibInit();          //振动传感器模块驱动          
	AdcInit(ADCincEXT);          //模数转换ADC模块驱动（含温度、光照、导航按键与按键Key3、EXT扩展接口上的ADC）
	StepMotorInit();    //步进电机模块驱动	                  
	// IrInit(NEC_R05d);         //38KHz红外通信模块驱动
    EXTInit(enumEXTUltraSonic);  //扩展接口模块驱动（含超声波、舵机等）
    SetDisplayerArea(0, 7);
    
    // 初始显示测试
    Seg7Print(1,2,3,4,5,6,7,8);  // 显示12345678测试数码管
    LedPrint(0xFF);  // 点亮所有LED测试
    
    SetEventCallBack(enumEventSys1S, my1s_callback);
    SetEventCallBack(enumEventKey, myKey_callback);
    while (1) {
        MySTC_OS();
    }
}