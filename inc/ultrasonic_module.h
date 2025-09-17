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
#define DISTANCE_INVALID        1024    // 无效距离值


// 自动门系统全局变量
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
 * @brief 紧急停止电机
 * @note 立即停止门的运动
 */
void AutoDoor_EmergencyStop(void);

/**
 * @brief 获取门电机状态
 * @return 1:电机空闲, 0:电机运行中
 */
unsigned char AutoDoor_IsMotorFree(void);

/**
 * @brief 获取超声波模块状态
 * @return 1:模块有效, 0:模块无效或未连接
 */
unsigned char IsUltrasonicValid(void);

#endif /* _ULTRASONIC_H_ */