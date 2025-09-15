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

/**
 * @brief 初始化收音机模块
 * @note 设置默认频率90.5MHz，音量10，启用GP1指示灯
 */
void Radio_Init(void);

/**
 * @brief 处理导航键音量控制
 * @note 左键降低音量，右键升高音量
 */
void Radio_HandleNavigation(void);

/**
 * @brief 设置收音机频率
 * @param frequency 频率值 (887-1080, 单位0.1MHz)
 */
void Radio_SetFrequency(unsigned int frequency);

/**
 * @brief 设置收音机音量
 * @param volume 音量值 (0-15)
 */
void Radio_SetVolume(unsigned char volume);

/**
 * @brief 获取当前音量
 * @return 当前音量值
 */
unsigned char Radio_GetVolume(void);

/**
 * @brief 获取当前频率
 * @return 当前频率值
 */
unsigned int Radio_GetFrequency(void);

#endif // __RADIO_H