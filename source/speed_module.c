#include "STC15F2K60S2.H"
#include "sys.H"
#include "displayer.H"

unsigned char xdata speed[4]={50,45,50,55}; // 每辆地铁的当前速度，改为char节省内存

int v = 200; // km/h
int GetSpeed()
{
	return v;
}
void ShowSpeed()
{
	Seg7Print(39, 54, 46, 46, 45, v/100, v/10%10, v%10);
}