#include "core.h"

unsigned char xdata speed[4]={0,25,30,30}; // 每辆地铁的当前速度，改为char节省内存

void ShowSpeed()
{
	Seg7Print(10, 10, 10, 10, 10, speed[id]/100, speed[id]/10%10, speed[id]%10);
}