#include "core.h"

// Use data segment for frequently accessed variables
char xdata displayMode = 0;   // 移至xdata段以节省data空间
// 导航键事件回调
void NavHandler()
{
    char xdata navUp = GetAdcNavAct(enumAdcNavKeyUp);
    char xdata navDown = GetAdcNavAct(enumAdcNavKeyDown);
    char xdata navLeft = GetAdcNavAct(enumAdcNavKeyLeft);
    char xdata navRight = GetAdcNavAct(enumAdcNavKeyRight);

    if (navUp == enumKeyPress) {     // 向上键
        displayMode++;
        if (displayMode > 3) displayMode = 0;  // 循环切换
    }
    if (navDown == enumKeyPress) {   // 向下键
        displayMode--;
        if (displayMode < 0) displayMode = 3;  // 循环切换
    }
    if (navLeft == enumKeyPress) {
        if (radio_config.volume > 0) {
            radio_config.volume--;        // 音量减1
            SetFMRadio(radio_config);     // 应用新音量
        }
    }
    
    // 检查右键（音量升高）
    if (navRight == enumKeyPress) {
        if (radio_config.volume < 15) {
            radio_config.volume++;        // 音量加1
            SetFMRadio(radio_config);     // 应用新音量
        }
    }
}

void ShowStatus()
{
    switch (displayMode) {
      case 0: ShowTime();  break;
      case 1: ShowTemp();  break;
      case 2: ShowSpeed(); break;
      case 3: ShowMode();  break;
      default: break;
  }
}