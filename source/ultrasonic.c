/**
 * @file ultrasonic.c
 * @brief 自动门控制系统实现文件
 * @version 2.0
 * @date 2025-09-15
 */

#include "STC15F2K60S2.H"
#include "sys.H"
#include "displayer.H"
#include "EXT.H"
#include "Key.H"
#include "Beep.H"     // 添加蜂鸣器支持用于调试
#include "ultrasonic.h"

// 全局变量定义
DoorState_t g_door_state = DOOR_CLOSED;     // 门状态，初始为关闭
unsigned char g_door_timer = 0;             // 门开启倒计时
unsigned int g_distance = 0;               // 当前距离值

/**
 * @brief 初始化自动门系统
 */
void UltrasonicInit(void) {
    // 初始化超声波模块 (使用EXT外设)
    EXTInit(enumEXTUltraSonic);
    
    // 初始化门状态
    g_door_state = DOOR_CLOSED;
    g_door_timer = 0;
    g_distance = 0;
}

/**
 * @brief 获取超声波距离
 * @return 距离值 (cm)，1024表示无效
 */
static unsigned int GetUltrasonicDistance(void) {
    unsigned int distance;
    
    // 读取EXT外设的超声波数据
    distance = GetUltraSonic();
    
    // 检查数据有效性
    if (distance >= DISTANCE_INVALID) {
        return DISTANCE_INVALID;
    }
    
    return distance;
}

/**
 * @brief 自动门系统主要更新函数
 */
void UltrasonicUpdateAndDisplay(void) {
    static unsigned int last_distance = 0xFFFF;  // 记录上次距离，用于调试
    
    // 获取当前距离
    g_distance = GetUltrasonicDistance();
    
    // 调试：当距离发生变化时（不再使用蜂鸣声）
    if (g_distance != last_distance && g_distance != DISTANCE_INVALID) {
        // SetBeep(1000, 100);  // 关闭蜂鸣器调试声音
        last_distance = g_distance;
    }
    
    // 处理门控制逻辑
    if (g_distance < DOOR_TRIGGER_DISTANCE && g_distance != DISTANCE_INVALID) {
        // 距离小于10cm且门是关闭的，开门
        if (g_door_state == DOOR_CLOSED) {
            AutoDoor_Open();
            // SetBeep(800, 500);  // 关闭开门蜂鸣声
        }
    }
    
    // 处理倒计时
    AutoDoor_ProcessTimer();
    
    // 更新显示
    AutoDoor_UpdateDisplay();
}

/**
 * @brief 开门操作
 */
void AutoDoor_Open(void) {
    g_door_state = DOOR_OPEN;           // 设置门状态为开启
    g_door_timer = DOOR_OPEN_TIME;      // 启动倒计时
}

/**
 * @brief 关门操作
 */
void AutoDoor_Close(void) {
    g_door_state = DOOR_CLOSED;         // 设置门状态为关闭
    g_door_timer = 0;                   // 停止倒计时
    // SetBeep(600, 200);               // 关闭关门蜂鸣声
}

/**
 * @brief 倒计时处理
 */
void AutoDoor_ProcessTimer(void) {
    // 只有门开启时才处理倒计时
    if (g_door_state == DOOR_OPEN && g_door_timer > 0) {
        g_door_timer--;
        
        // 倒计时结束，关门
        if (g_door_timer == 0) {
            AutoDoor_Close();
        }
    }
}

/**
 * @brief 更新显示
 */
void AutoDoor_UpdateDisplay(void) {
    unsigned char d0, d1, d2, d3;
    
    if (g_distance == DISTANCE_INVALID) {
        // 显示错误状态 "----"
        Seg7Print(12, 12, 12, 12, 10, 10, 10, 10);  // 前4位显示横线，后4位空白
    } else {
        // 将距离值分解为各个数位显示
        if (g_distance >= 1000) {
            // 大于等于1000cm，显示前4位数字
            d3 = g_distance / 1000;
            d2 = (g_distance % 1000) / 100;
            d1 = (g_distance % 100) / 10;
            d0 = g_distance % 10;
        } else if (g_distance >= 100) {
            // 100-999cm，显示3位数字
            d3 = 10;  // 空白
            d2 = g_distance / 100;
            d1 = (g_distance % 100) / 10;
            d0 = g_distance % 10;
        } else if (g_distance >= 10) {
            // 10-99cm，显示2位数字
            d3 = 10;  // 空白
            d2 = 10;  // 空白
            d1 = g_distance / 10;
            d0 = g_distance % 10;
        } else {
            // 0-9cm，显示1位数字
            d3 = 10;  // 空白
            d2 = 10;  // 空白
            d1 = 10;  // 空白
            d0 = g_distance;
        }
        
        // 显示: 距离值cm + 门状态标识
        if (g_door_state == DOOR_OPEN) {
            // 门开启状态，后4位显示 "oPEn"
            Seg7Print(d3, d2, d1, d0, 0, 15, 8, 11);  // 距离 + o P E n
        } else {
            // 门关闭状态，后4位显示距离的cm单位或空白
            Seg7Print(d3, d2, d1, d0, 10, 10, 10, 10);  // 距离 + 空白
        }
    }
    
    // 使用LED显示门状态
    if (g_door_state == DOOR_OPEN) {
        // 门开启：点亮LED
        LedPrint(0x01);     // 点亮第一个LED表示门开启
    } else {
        // 门关闭：熄灭LED
        LedPrint(0x00);     // 熄灭所有LED表示门关闭
    }
}

/**
 * @brief 处理按键输入（手动控制）
 */
void UltrasonicHandleKeyInput(unsigned char keycode) {
    // 检查按键1：手动开门
    if (GetKeyAct(enumKey1) == enumKeyPress) {
        if (g_door_state == DOOR_CLOSED) {
            AutoDoor_Open();
        }
    }
    
    // 检查按键2：手动关门
    if (GetKeyAct(enumKey2) == enumKeyPress) {
        if (g_door_state == DOOR_OPEN) {
            AutoDoor_Close();
        }
    }
    
    // 检查按键3：重置系统
    if (GetKeyAct(enumKey3) == enumKeyPress) {
        AutoDoor_Close();
        g_distance = 0;
    }
}