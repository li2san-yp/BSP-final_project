#include "STC15F2K60S2.H"
#include "sys.H"
#include "displayer.H"

int v = 56; // km/h
int GetSpeed()
{
	return v;
}
void ShowSpeed()
{
	Seg7Print(39, 54, 46, 46, 45, v/100, v/10%10, v%10);
}