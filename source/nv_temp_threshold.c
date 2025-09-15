/**********************************非易失温度阈值存储模块实现 V1.0（NV 前缀） ************************************************************************
 nv_temp_threshold 模块的具体实现，行为与原 temp_threshold 相同，仅重命名为更明确的 NV 前缀
*/

#include "nv_temp_threshold.h"
#include "sys.h"
#include "M24C02.h"

// 内部变量（放入 xdata）
static unsigned char xdata s_nv_lastWriteCarId = 0xFF;     // 上次写操作的车辆ID
static unsigned char xdata s_nv_isInitialized = 0;         // 模块初始化标志

// 内部函数声明
static char NVIsValidThreshold(unsigned char threshold);

/**
 * @brief 检查温度阈值是否有效
 * @param threshold 温度阈值
 * @return 1有效，0无效
 */
static char NVIsValidThreshold(unsigned char threshold)
{
    return (threshold >= NV_TEMP_THRESHOLD_MIN && threshold <= NV_TEMP_THRESHOLD_MAX);
}

/**
 * @brief 初始化温度阈值非易失存储模块
 */
void NVTempThresholdInit(void)
{
    s_nv_isInitialized = 1;
    s_nv_lastWriteCarId = 0xFF;
}

/**
 * @brief 读取指定车辆的阈值
 * @param carId 车辆ID（0-255）
 * @return 读取的阈值，若为 NV_TEMP_THRESHOLD_INVALID 表示未设置或错误
 */
unsigned char NVTempThresholdRead(unsigned char carId)
{
    unsigned char xdata storedValue;
    
    if (!s_nv_isInitialized) {
        NVTempThresholdInit();
    }

    storedValue = M24C02_Read(carId);

    if (storedValue == NV_TEMP_THRESHOLD_INVALID || !NVIsValidThreshold(storedValue)) {
        return NV_TEMP_THRESHOLD_DEFAULT;
    }

    return storedValue;
}

/**
 * @brief 写入指定车辆的阈值
 * @param carId 车辆ID（0-255）
 * @param threshold 新阈值
 * @return 0成功，-1失败
 */
char NVTempThresholdWrite(unsigned char carId, unsigned char threshold)
{
    if (!s_nv_isInitialized) {
        NVTempThresholdInit();
    }

    if (!NVIsValidThreshold(threshold)) {
        return -1;
    }

    M24C02_Write(carId, threshold);
    s_nv_lastWriteCarId = carId;
    return 0;
}

/**
 * @brief 智能更新：只有当值改变时才写入
 * @return 1=写入，0=未写入，-1=错误
 */
int NVTempThresholdUpdate(unsigned char carId, unsigned char threshold)
{
    unsigned char xdata current;
    
    if (!s_nv_isInitialized) {
        NVTempThresholdInit();
    }

    if (!NVIsValidThreshold(threshold)) {
        return -1;
    }

    current = NVTempThresholdRead(carId);
    if (current == threshold) {
        return 0; // 未写入
    }

    return NVTempThresholdWrite(carId, threshold) == 0 ? 1 : -1;
}

