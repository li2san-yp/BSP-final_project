/*
 * 非易失温度阈值存储模块头文件（NV = Non-Volatile）
 */

#ifndef _NV_TEMP_THRESHOLD_H_
#define _NV_TEMP_THRESHOLD_H_

#include "core.h"

#define NV_TEMP_THRESHOLD_DEFAULT      40      // 默认温度阈值（摄氏度）
#define NV_TEMP_THRESHOLD_MIN          0       // 最低温度阈值
#define NV_TEMP_THRESHOLD_MAX          100     // 最高温度阈值
#define NV_TEMP_THRESHOLD_INVALID      0xFF    // 无效/未初始化标志


// 初始化非易失存储模块，加载当前车辆ID的温度阈值到内存（建议在main函数中调用）
void NVTempThresholdInit(void);

// 读取指定车辆的阈值（返回字节；若返回 NV_TEMP_THRESHOLD_INVALID 表示未设置/错误）
unsigned char NVTempThresholdRead();

// 直接写入指定车辆的阈值到EEPROM（阻塞，写前不检查是否相同）
char NVTempThresholdWrite(unsigned char threshold);

// 智能更新：只有当新值与旧值不同时才写入，返回写入是否发生（1=写入，0=未写入，-1=错误）
int NVTempThresholdUpdate(unsigned char threshold);

#endif
