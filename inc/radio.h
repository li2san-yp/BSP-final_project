/**
 * @file radio.h
 * @brief 收音机模块头文件
 * @version 1.0
 * @date 2025-09-15
 * 
 * 封装收音机初始化和控制功能
 */

#ifndef __RADIO_H
#define __RADIO_H
#include "FM_Radio.h"
extern struct_FMRadio radio_config;
extern unsigned char radio_initialized;

/**
 * @brief 初始化收音机模块
 * @note 设置默认频率90.5MHz，音量10，启用GP1指示灯
 */
void Radio_Init(void);


#endif // __RADIO_H