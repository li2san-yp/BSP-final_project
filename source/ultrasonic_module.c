/**
 * @file ultrasonic.c
 * @brief 自动门控制系统实现文件
 * @version 2.0
 * @date 2025-09-15
 */

#include "core.h"

// 全局变量定义
g_distance = 1024;
static unsigned char xdata ultrasonic_valid = 0;                              // 超声波模块有效性标志
static unsigned int xdata last_distances[5] = {1024, 1024, 1024, 1024, 1024}; // 存储最近5次读数
static unsigned char xdata distance_index = 0;                                // 距离数组索引

// 步进电机配置参数
#define DOOR_MOTOR_SPEED 100  // 门电机速度 (步/秒)
#define DOOR_OPEN_STEPS 200   // 开门步数 (正转)
#define DOOR_CLOSE_STEPS -200 // 关门步数 (反转)

/**
 * @brief 初始化自动门系统
 */
void UltrasonicInit(void)
{
    unsigned char xdata i;

    // 初始化超声波模块 (使用EXT外设)
    EXTInit(enumEXTUltraSonic);
    // SetBeep(1000, 200);  // 初始化提示音
    //  初始化步进电机
    StepMotorInit();
    // SetBeep(1000, 200);  // 初始化提示音

    // 强制初始化门状态为关闭
    is_door_open[id] = 0; // 门初始为关闭状态

    // 物理上确保门是关闭的（直接执行关门操作）
    // 即使门已经关闭，这个操作也是安全的
    if (GetStepMotorStatus(enumStepMotor1) == enumStepMotorFree)
    {
        SetStepMotor(enumStepMotor1, DOOR_MOTOR_SPEED, DOOR_CLOSE_STEPS);
    }

    g_distance = DISTANCE_INVALID; // 初始化为无效值
    ultrasonic_valid = 0;          // 初始化时假设模块无效，等待检测

    // 初始化历史距离数组
    for (i = 0; i < 5; i++)
    {
        last_distances[i] = DISTANCE_INVALID;
    }
    distance_index = 0;
}

/**
 * @brief 获取超声波距离
 * @return 距离值 (cm)，1024表示无效
 */
static unsigned int GetUltrasonicDistance(void)
{
    unsigned int distance;
    unsigned char i;
    unsigned char has_variation = 0;

    // 读取EXT外设的超声波数据
    distance = GetUltraSonic();

    // 存储到历史数组
    last_distances[distance_index] = distance;
    distance_index = (distance_index + 1) % 5;

    // 检查最近5次读数是否有变化
    for (i = 1; i < 5; i++)
    {
        if (last_distances[i] != last_distances[0])
        {
            has_variation = 1;
            break;
        }
    }

    // 判断超声波模块有效性
    if (distance >= DISTANCE_INVALID)
    {
        // 明显的无效值
        ultrasonic_valid = 0;
        return DISTANCE_INVALID;
    }
    else if (!has_variation)
    {
        // 连续5次相同值，可能是模块故障
        if (distance == 0 || distance == 1023 || distance == 65535)
        {
            // 这些值很可能是故障值
            ultrasonic_valid = 0;
            return DISTANCE_INVALID;
        }
        else
        {
            // 可能是真实的固定距离，暂时认为有效
            ultrasonic_valid = 1;
        }
    }
    else
    {
        // 有数据变化，模块正常工作
        ultrasonic_valid = 1;
    }

    return distance;
}

/**
 * @brief 自动门系统主要更新函数
 */
void UltrasonicUpdateAndDisplay(void)
{
    unsigned char state = GetPlayerMode();
    // 获取当前距离
    g_distance = GetUltrasonicDistance();
    
    // 调试功能已确认有效，现在关闭
    // 如需重新调试，取消下面注释
    //SetBeep(2000, 20); // 每次更新时短促鸣叫，确认模块工作
    // if (!ultrasonic_valid) {
    //     // 模块无效：低频长鸣
    //     SetBeep(200, 100);
    // } else if (g_distance == 0) {
    //     // 距离为0：高频短鸣
    //     SetBeep(1500, 50);
    // } else if (g_distance < 10) {
    //     // 距离小于10cm：中频短鸣
    //     SetBeep(1000, 100);
    // }
    

    // 强制门控制逻辑：不管是否有超声波模块
    if (mode == 0)
    {
        // 车启动时，强制确保门是关闭的（不依赖超声波模块）
        if (is_door_open[id] == 1)
        {
            AutoDoor_Close();
            if(state == enumModePlay)
            {
                SetPlayerMode(enumModeStop);
            }
        }
    }

    // 超声波模块相关的门控制逻辑 - 只有在超声波模块有效时才工作
    if (ultrasonic_valid && g_distance < DOOR_TRIGGER_DISTANCE && g_distance != DISTANCE_INVALID)
    {
        // 距离小于10cm且门是关闭的，开门
        if (is_door_open[id] == 0 && mode == 1)
        { // 关门且车停止
            AutoDoor_Open();
            if(state == enumModeStop)
            {
                SetPlayerMode(enumModePlay);
            }
        }
    }

    // 超声波报警逻辑：车启动时检测前方障碍物
    if (ultrasonic_valid && mode == 0)
    {
        // 超声波模块有效且车启动（运行模式）
        if (g_distance <= 5 && g_distance != DISTANCE_INVALID)
        {
            // 距离小于等于5cm时报警 - 有障碍物危险
            SetBeep(800, 300); // 紧急报警音
        }
    }

    // 更新显示
    // AutoDoor_UpdateDisplay();
}

/**
 * @brief 开门操作
 */
void AutoDoor_Open(void)
{
    // 检查电机是否空闲
    if (GetStepMotorStatus(enumStepMotor1) == enumStepMotorFree)
    {
        is_door_open[id] = 1; // 设置门状态为开启

        // 启动步进电机开门 - 正转
        if (SetStepMotor(enumStepMotor1, DOOR_MOTOR_SPEED, DOOR_OPEN_STEPS) == enumSetStepMotorOK)
        {
            // 开门电机启动成功
            // SetBeep(800, 200);  // 可选：开门提示音
        }
    }
}

/**
 * @brief 关门操作
 */
void AutoDoor_Close(void)
{
    // 检查电机是否空闲
    if (GetStepMotorStatus(enumStepMotor1) == enumStepMotorFree)
    {
        is_door_open[id] = 0; // 设置门状态为关闭

        // 启动步进电机关门 - 反转
        if (SetStepMotor(enumStepMotor1, DOOR_MOTOR_SPEED, DOOR_CLOSE_STEPS) == enumSetStepMotorOK)
        {
            // 关门电机启动成功
            // SetBeep(600, 200);  // 可选：关门提示音
        }
    }
}

/**
 * @brief 紧急停止电机
 * @note 立即停止门的运动
 */
void AutoDoor_EmergencyStop(void)
{
    EmStop(enumStepMotor1); // 紧急停止步进电机
}

/**
 * @brief 获取门电机状态
 * @return 1:电机空闲, 0:电机运行中
 */
unsigned char AutoDoor_IsMotorFree(void)
{
    return (GetStepMotorStatus(enumStepMotor1) == enumStepMotorFree) ? 1 : 0;
}

/**
 * @brief 获取超声波模块状态
 * @return 1:模块有效, 0:模块无效或未连接
 */
unsigned char IsUltrasonicValid(void)
{
    return ultrasonic_valid;
}
