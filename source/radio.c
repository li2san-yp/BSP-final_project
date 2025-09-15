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
static struct_FMRadio radio_config;
static unsigned char radio_initialized = 0;

/**
 * @brief 初始化收音机模块
 */
void Radio_Init(void) {
    if (!radio_initialized) {
        // 设置默认配置
        radio_config.frequency = 905;   // 90.5MHz
        radio_config.volume = 10;       // 音量10
        radio_config.GP1 = 1;          // 启用GP1指示灯
        radio_config.GP2 = 0;          // 关闭GP2指示灯
        radio_config.GP3 = 0;          // 关闭GP3指示灯
        
        // 初始化收音机
        FMRadioInit(radio_config);
        SetFMRadio(radio_config);      // 确保设置生效
        
        radio_initialized = 1;
    }
}

/**
 * @brief 处理导航键音量控制
 */
void Radio_HandleNavigation(void) {
    // 检查左键（音量降低）
    if (GetAdcNavAct(enumAdcNavKeyLeft) == enumKeyPress) {
        SetBeep(1000, 100);  // 调试：按键时发出短促蜂鸣声
        if (radio_config.volume > 0) {
            radio_config.volume--;        // 音量减1
            SetFMRadio(radio_config);     // 应用新音量
        }
    }
    
    // 检查右键（音量升高）
    if (GetAdcNavAct(enumAdcNavKeyRight) == enumKeyPress) {
        SetBeep(1200, 100);  // 调试：按键时发出短促蜂鸣声（频率稍高）
        if (radio_config.volume < 15) {
            radio_config.volume++;        // 音量加1
            SetFMRadio(radio_config);     // 应用新音量
        }
    }
    
    // 检查上键和下键（可选，用于调试）
    if (GetAdcNavAct(enumAdcNavKeyUp) == enumKeyPress) {
        SetBeep(800, 100);   // 调试：确认导航键工作
    }
    
    if (GetAdcNavAct(enumAdcNavKeyDown) == enumKeyPress) {
        SetBeep(600, 100);   // 调试：确认导航键工作
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