/**
 * @file ultrasonic.c
 * @brief 自动门控制系统实现文件
 * @version 2.0
 * @date 2025-09-15
 */

#include "core.h"
#include "StepMotor.h"      // 添加步进电机头文件

// 全局变量定义
unsigned int g_distance = 0;               // 当前距离值

// 步进电机配置参数
#define DOOR_MOTOR_SPEED        100     // 门电机速度 (步/秒)
#define DOOR_OPEN_STEPS         200     // 开门步数 (正转)
#define DOOR_CLOSE_STEPS        -200    // 关门步数 (反转)

/**
 * @brief 初始化自动门系统
 */
void UltrasonicInit(void) {
    // 初始化超声波模块 (使用EXT外设)
    EXTInit(enumEXTUltraSonic);
    
    // 初始化步进电机
    StepMotorInit();
    
    // 初始化门状态
    is_door_open[id] = 0;  // 门初始为关闭状态 
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
    
    // 获取当前距离
    g_distance = GetUltrasonicDistance();
    
    // 处理门控制逻辑
    if (g_distance < DOOR_TRIGGER_DISTANCE && g_distance != DISTANCE_INVALID) {
        // 距离小于10cm且门是关闭的，开门
        if (is_door_open[id] == 0 && tinfo.time_mode == 1) { //关门且车停止
            AutoDoor_Open();
            // SetBeep(800, 500);  // 关闭开门蜂鸣声
        }
    }
    
    // 更新显示
    //AutoDoor_UpdateDisplay();
}

/**
 * @brief 开门操作
 */
void AutoDoor_Open(void) {
    // 检查电机是否空闲
    if (GetStepMotorStatus(enumStepMotor1) == enumStepMotorFree) {
        is_door_open[id] = 1;           // 设置门状态为开启
        
        // 启动步进电机开门 - 正转
        if (SetStepMotor(enumStepMotor1, DOOR_MOTOR_SPEED, DOOR_OPEN_STEPS) == enumSetStepMotorOK) {
            // 开门电机启动成功
            // SetBeep(800, 200);  // 可选：开门提示音
        }
    }
}

/**
 * @brief 关门操作
 */
void AutoDoor_Close(void) {
    // 检查电机是否空闲
    if (GetStepMotorStatus(enumStepMotor1) == enumStepMotorFree) {
        is_door_open[id] = 0;         // 设置门状态为关闭
        
        // 启动步进电机关门 - 反转
        if (SetStepMotor(enumStepMotor1, DOOR_MOTOR_SPEED, DOOR_CLOSE_STEPS) == enumSetStepMotorOK) {
            // 关门电机启动成功
            // SetBeep(600, 200);  // 可选：关门提示音
        }
    }
}

/**
 * @brief 紧急停止电机
 * @note 立即停止门的运动
 */
void AutoDoor_EmergencyStop(void) {
    EmStop(enumStepMotor1);  // 紧急停止步进电机
}

/**
 * @brief 获取门电机状态
 * @return 1:电机空闲, 0:电机运行中
 */
unsigned char AutoDoor_IsMotorFree(void) {
    return (GetStepMotorStatus(enumStepMotor1) == enumStepMotorFree) ? 1 : 0;
}

