#ifndef SPEED_MODULE_H
#define SPEED_MODULE_H

extern unsigned char xdata speed[4];   // 每辆地铁的当前速度，改为char节省内存
extern int GetSpeed();

#endif // SPEED_MODULE_H