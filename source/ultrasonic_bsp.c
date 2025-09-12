#include "EXT.h"

// 获取超声波距离（单位：cm）
int ultrasonic_get_distance(void) {
    return GetUltraSonic();
}