#include "core.h"

#define TEMP_THRESHOLD 30  // 温度阈值，30度

void temp_controlled_motor()
{
    // 获取当前温度
    if (temp > TEMP_THRESHOLD) {
        // 温度超过阈值，开启风扇
        SetPWM(50, 10, 0, 10);  // 设置PWM输出，启动风扇
    } else {
        // 温度低于阈值，关闭风扇
        SetPWM(0, 0, 0, 0);     // 停止PWM输出，关闭风扇
    }
}