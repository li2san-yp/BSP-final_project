/**********************************温度阈值非易失存储模块 V1.0 说明 ************************************************************************
温度阈值存储模块提供温度报警阈值的非易失存储功能：
(1) 使用M24C02芯片存储每辆车的温度阈值设置
(2) 车辆ID作为存储地址，支持最多256辆车（ID范围：0-255）
(3) 提供初始化、读取、写入温度阈值的接口函数

存储地址分配：
- 地址0x00-0xFF：对应车辆ID 0-255的温度阈值（单位：摄氏度，整数型）
- 默认阈值：40摄氏度（当存储单元为0xFF或未初始化时）

注意事项：
- M24C02写操作需要5-10ms时间，两次写操作间需间隔足够时间
- 有写寿命限制（约10万次），避免频繁写入
- 仅在温度阈值确实改变时才执行写操作

编写：BSP项目组   2025年9月15日设计
*/

#ifndef _TEMP_THRESHOLD_H_
#define _TEMP_THRESHOLD_H_

/*
 * 兼容性头文件：保留旧 API 名称（TempThreshold*），但将宏常量与新的
 * 非易失存储模块 `nv_temp_threshold.h` 对齐。源码中仍可使用旧名称，
 * 实现体由 `source/temp_threshold.c` 调用新实现（thin wrapper）。
 */

#include "nv_temp_threshold.h"

// 将旧宏映射到新的 NV 宏（保持兼容）
#define TEMP_THRESHOLD_DEFAULT      NV_TEMP_THRESHOLD_DEFAULT
#define TEMP_THRESHOLD_MIN          NV_TEMP_THRESHOLD_MIN
#define TEMP_THRESHOLD_MAX          NV_TEMP_THRESHOLD_MAX
#define TEMP_THRESHOLD_INVALID      NV_TEMP_THRESHOLD_INVALID

// 旧函数声明（实现为对 NV API 的包装，见 source/temp_threshold.c）
extern void TempThresholdInit(void);                                        // 初始化温度阈值存储模块（兼容）
extern unsigned char TempThresholdRead(unsigned char carId);                // 读取指定车辆的温度阈值（兼容）
extern char TempThresholdWrite(unsigned char carId, unsigned char threshold); // 写入指定车辆的温度阈值（兼容）
extern char TempThresholdUpdate(unsigned char carId, unsigned char newThreshold); // 更新温度阈值（兼容）

#endif