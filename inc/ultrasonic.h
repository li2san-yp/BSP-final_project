#ifndef _ULTRASONIC_H_
#define _ULTRASONIC_H_

// 超声波测距模块头文件

/**
 * @brief 初始化超声波模块
 * @param 无
 * @return 无
 */
void UltrasonicInit(void);

/**
 * @brief 获取超声波测距值
 * @param 无
 * @return 距离值(cm)，返回值说明：
 *         0-999: 有效距离(cm)
 *         1024/4096: 模块无响应或硬件错误
 *         其他大值: 超出测量范围
 */
int UltrasonicGetDistance(void);

/**
 * @brief 显示距离到数码管
 * @param distance 要显示的距离值
 * @return 无
 * @note 自动处理不同距离范围的显示格式
 */
void UltrasonicDisplayDistance(int distance);

/**
 * @brief 检查距离值是否有效
 * @param distance 距离值
 * @return 1: 有效距离  0: 无效距离(错误值)
 */
int UltrasonicIsValidDistance(int distance);

#endif /* _ULTRASONIC_H_ */