// /**********************************非易失温度阈值存储模块实现 V1.0（NV 前缀） ************************************************************************
//  nv_temp_threshold 模块的具体实现，行为与原 temp_threshold 相同，仅重命名为更明确的 NV 前缀
// */

// #include "nv_temp_threshold.h"
// #include "sys.h"
// #include "M24C02.h"

// // 内部变量（放入 xdata）
// static unsigned char xdata s_nv_lastWriteCarId = 0xFF;     // 上次写操作的车辆ID
// static unsigned char xdata s_nv_isInitialized = 0;         // 模块初始化标志

// // 内部函数声明
// static char NVIsValidThreshold(unsigned char threshold);

// /**
//  * @brief 检查温度阈值是否有效
//  * @param threshold 温度阈值
//  * @return 1有效，0无效
//  */
// static char NVIsValidThreshold(unsigned char threshold)
// {
//     return (threshold >= NV_TEMP_THRESHOLD_MIN && threshold <= NV_TEMP_THRESHOLD_MAX);
// }

// /**
//  * @brief 初始化温度阈值非易失存储模块
//  * 加载当前车辆ID对应的温度阈值到内存中
//  */
// void NVTempThresholdInit(void)
// {
//     unsigned char xdata threshold;
//     char i=0,j;
    
//     s_nv_isInitialized = 1;
//     s_nv_lastWriteCarId = 0xFF;
    
//     // 初始化EEPROM中所有车辆的温度阈值为默认值40
//     // 注意：M24C02每次写操作需要5-10ms延时
//     for(i=0;i<4;i++){
//         M24C02_Write(i, 40);
//         for(j=0;j<2000000;j++); // 简单延时，确保写入完成
//     }
    
//     // 强制更新内存数组，确保与EEPROM同步
//     // 先将所有元素设为0，然后从EEPROM读取并更新
//     for(i=0;i<4;i++){
//         tempThresholds[i] = 0;  // 先清零，避免编译器优化
//     }
    
//     // 从EEPROM读取所有车辆的温度阈值并强制更新到内存数组中
//     for(i=0;i<4;i++){
//         threshold = M24C02_Read(i);
        
//         // 强制赋值，不管原来的值是什么
//         tempThresholds[i] = threshold;
        
//         // 如果读取值无效，则设置为默认值
//         if (threshold == NV_TEMP_THRESHOLD_INVALID || !NVIsValidThreshold(threshold)) {
//             tempThresholds[i] = NV_TEMP_THRESHOLD_DEFAULT;
//         }
//     }
// }

// /**
//  * @brief 读取指定车辆的阈值
//  * @param id 车辆ID（0-255）
//  * @return 读取的阈值，若为 NV_TEMP_THRESHOLD_INVALID 表示未设置或错误
//  */
// unsigned char NVTempThresholdRead()
// {
//     unsigned char xdata storedValue;
    
//     if (!s_nv_isInitialized) {
//         NVTempThresholdInit();
//     }

//     storedValue = M24C02_Read(id);

//     if (storedValue == NV_TEMP_THRESHOLD_INVALID || !NVIsValidThreshold(storedValue)) {
//         return NV_TEMP_THRESHOLD_DEFAULT;
//     }

//     return storedValue;
// }

// /**
//  * @brief 写入指定车辆的阈值
//  * @param id 车辆ID（0-255）
//  * @param threshold 新阈值
//  * @return 0成功，-1失败
//  */
// char NVTempThresholdWrite(unsigned char threshold)
// {
//     if (!s_nv_isInitialized) {
//         NVTempThresholdInit();
//     }

//     if (!NVIsValidThreshold(threshold)) {
//         return -1;
//     }

//     M24C02_Write(id, threshold);
//     s_nv_lastWriteCarId = id;
//     return 0;
// }

// /**
//  * @brief 智能更新：只有当值改变时才写入
//  * @return 1=写入，0=未写入，-1=错误
//  */
// int NVTempThresholdUpdate(unsigned char threshold)
// {
//     unsigned char xdata current;
    
//     if (!s_nv_isInitialized) {
//         NVTempThresholdInit();
//     }

//     if (!NVIsValidThreshold(threshold)) {
//         return -1;
//     }

//     current = NVTempThresholdRead();
//     if (current == threshold) {
//         return 0; // 未写入
//     }

//     return NVTempThresholdWrite(threshold) == 0 ? 1 : -1;
// }

