/**
 * @file radio.c
 * @brief 收音机模块实现文件
 * @version 1.0
 * @date 2025-09-15
 * 
 * 封装收音机初始化和控制功能
 */

#include "sys.h"
#include "FM_Radio.h"
#include "adc.h"
#include "Key.h"       // 添加按键枚举定义
#include "Beep.h"      // 添加蜂鸣器用于调试
#include "radio.h"

// 收音机配置变量
 struct_FMRadio radio_config;
 unsigned char radio_initialized = 0;

/**
 * @brief 初始化收音机模块
 */
void Radio_Init(void) {
    BeepInit();
    SetBeep(1000, 100); // 调试：短暂蜂鸣提示初始化开始
    if (!radio_initialized) {
        // 设置默认配置
        radio_config.frequency = 1005;  // 100.5MHz - 尝试一个常见的FM频率
        radio_config.volume = 12;       // 音量12 (增加音量)
        radio_config.GP1 = 1;          // 启用GP1指示灯
        radio_config.GP2 = 1;          // 启用GP2指示灯用于调试
        radio_config.GP3 = 0;          // 关闭GP3指示灯
        
        // 初始化收音机
        FMRadioInit(radio_config);
        SetFMRadio(radio_config);      // 确保设置生效
        
        radio_initialized = 1;
        
        // 调试：短暂蜂鸣提示收音机已初始化
        SetBeep(1000, 200);
    }
}

/**
 * @brief 设置收音机频率
 */
void Radio_SetFrequency(unsigned int frequency) {
    if (frequency >= 887 && frequency <= 1080) {
        radio_config.frequency = frequency;
        if (radio_initialized) {
            SetFMRadio(radio_config);
        }
    }
}

/**
 * @brief 设置收音机音量
 */
void Radio_SetVolume(unsigned char volume) {
    if (volume <= 15) {
        radio_config.volume = volume;
        if (radio_initialized) {
            SetFMRadio(radio_config);
        }
    }
}

/**
 * @brief 获取当前音量
 */
unsigned char Radio_GetVolume(void) {
    return radio_config.volume;
}

/**
 * @brief 获取当前频率
 */
unsigned int Radio_GetFrequency(void) {
    return radio_config.frequency;
}

/**
 * @brief 测试收音机功能
 * @note 循环测试不同频率和音量，用于调试
 */
void Radio_Test(void) {
        unsigned int test_frequencies[] = {905, 1005, 1035, 1077};  // 90.5, 100.5, 103.5, 107.7 MHz
    unsigned char i;
    if (!radio_initialized) {
        Radio_Init();
    }
    
    // 测试几个常见的FM频率

    
    for (i = 0; i < 4; i++) {
        Radio_SetFrequency(test_frequencies[i]);
        Radio_SetVolume(15);  // 最大音量测试
        SetBeep(500 + i * 100, 100);  // 不同音调提示当前频率
        // 这里可以添加延时让用户听收音机声音
    }
}