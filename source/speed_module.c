#include "core.h"

unsigned char xdata speed[4]={0,150,50,55}; // 每辆地铁的当前速度，改为char节省内存

int GetSpeed()
{
	return speed[id];
}
void ShowSpeed()
{
	Seg7Print(10, 10, 10, 10, 10, speed[id]/100, speed[id]/10%10, speed[id]%10);
}