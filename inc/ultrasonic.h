#ifndef _ULTRASONIC_H_
#define _ULTRASONIC_H_

/**
 * @file ultrasonic.h
 * @brief 自动门控制系统头文件
 * @version 2.0
 * @date 2025-09-15
 * 
 * 功能说明：
 * - 超声波距离检测
 * - 距离小于10cm时开门
 * - 倒计时结束时自动关门
 * - FM收音机持续工作
 */

// 自动门系统配置参数
#define DOOR_TRIGGER_DISTANCE   10      // 开门触发距离 (cm)
#define DOOR_OPEN_TIME          5       // 门开启时间 (秒)
#define DISTANCE_INVALID        1024    // 无效距离值

// 门状态枚举
typedef enum {
    DOOR_CLOSED = 0,    // 门关闭
    DOOR_OPEN = 1       // 门开启
} DoorState_t;

// 自动门系统全局变量
extern DoorState_t g_door_state;       // 门状态变量
extern unsigned char g_door_timer;     // 门开启倒计时 (秒)
extern unsigned int g_distance;        // 当前距离值 (cm)

/**
 * @brief 初始化自动门系统
 * @note 初始化超声波模块、FM收音机和相关硬件
 */
void UltrasonicInit(void);

/**
 * @brief 自动门系统主要更新函数
 * @note 在定时器回调中调用，处理距离检测和门控制逻辑
 */
void UltrasonicUpdateAndDisplay(void);

/**
 * @brief 处理按键输入（如果需要手动控制）
 * @note 在按键回调中调用
 */
void UltrasonicHandleKeyInput(unsigned char keycode);

/**
 * @brief 开门操作
 * @note 设置门状态为开启并启动倒计时
 */
void AutoDoor_Open(void);

/**
 * @brief 关门操作
 * @note 设置门状态为关闭并停止倒计时
 */
void AutoDoor_Close(void);

/**
 * @brief 更新显示
 * @note 显示当前距离和门状态
 */
void AutoDoor_UpdateDisplay(void);

/**
 * @brief 倒计时处理
 * @note 处理门开启倒计时逻辑
 */
void AutoDoor_ProcessTimer(void);

#endif /* _ULTRASONIC_H_ */