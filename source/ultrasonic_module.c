#include "core.h"
// 静态变量存储当前距离值（放入 xdata 减少 data 段占用）
static int xdata current_distance = 0;
static unsigned char xdata ultrasonic_initialized = 0;

/**
 * @brief 初始化超声波模块
 */
void UltrasonicInit(void) {
    if (!ultrasonic_initialized) {
        EXTInit(enumEXTUltraSonic);
        ultrasonic_initialized = 1;
        current_distance = 0;
    }
}

/**
 * @brief 获取超声波测距值
 */
int UltrasonicGetDistance(void) {
    if (!ultrasonic_initialized) {
        UltrasonicInit();
    }
    
    current_distance = GetUltraSonic();
    return current_distance;
}

/**
 * @brief 显示距离到数码管
 */
void UltrasonicDisplayDistance(int distance) {
    // 错误值显示
    if (distance == 4096) {
        Seg7Print(4,0,9,6,10,10,10,10);  // 显示 "4096"
        return;
    }
    if (distance == 1024) {
        Seg7Print(1,0,2,4,10,10,10,10);  // 显示 "1024"
        return;
    }
    
    // 正常距离显示
    if (distance < 0) {
        // 负值显示为 "----"
        Seg7Print(12,12,12,12,10,10,10,10);
    }
    else if (distance == 0) {
        // 零值显示为 "   0"
        Seg7Print(10,10,10,0,10,10,10,10);
    }
    else if (distance < 10) {
        // 个位数: "   X"
        Seg7Print(10,10,10,distance,10,10,10,10);
    }
    else if (distance < 100) {
        // 两位数: "  XX"
        Seg7Print(10,10,distance/10,distance%10,10,10,10,10);
    }
    else if (distance < 1000) {
        // 三位数: " XXX"
        Seg7Print(10,distance/100,(distance/10)%10,distance%10,10,10,10,10);
    }
    else {
        // 超大值显示为 "----"
        Seg7Print(12,12,12,12,10,10,10,10);
    }
}

/**
 * @brief 检查距离值是否有效
 */
int UltrasonicIsValidDistance(int distance) {
    // 常见的错误返回值
    if (distance == 1024 || distance == 4096) {
        return 0;  // 无效
    }
    
    // 合理的距离范围 (0-500cm)
    if (distance >= 0 && distance <= 500) {
        return 1;  // 有效
    }
    
    return 0;  // 其他情况视为无效
}

/**
 * @brief 获取当前存储的距离值
 */
int UltrasonicGetCurrentDistance(void) {
    return current_distance;
}