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
    // SetBeep(1000, 100); // 调试：短暂蜂鸣提示初始化开始
    if (!radio_initialized) {
        // 设置默认配置
        radio_config.frequency = 918;  // 100.5MHz - 尝试一个常见的FM频率
        radio_config.volume = 10;       // 音量12 (增加音量)
        radio_config.GP1 = 1;          // 启用GP1指示灯
        radio_config.GP2 = 1;          // 启用GP2指示灯用于调试
        radio_config.GP3 = 0;          // 关闭GP3指示灯
        
        // 初始化收音机
        FMRadioInit(radio_config);
        SetFMRadio(radio_config);      // 确保设置生效
        
        radio_initialized = 1;
        
        // 调试：短暂蜂鸣提示收音机已初始化
        // SetBeep(1000, 200);
    }
}
