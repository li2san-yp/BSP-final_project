#ifndef SPEED_MODULE_H
#define SPEED_MODULE_H

extern unsigned char xdata speed[4];   // 每辆地铁的当前速度，改为char节省内存
extern int v;                    // 速度值，单位 km/h

extern int GetSpeed();
extern void ShowSpeed();

#endif // SPEED_MODULE_H