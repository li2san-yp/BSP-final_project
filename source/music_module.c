#include "core.h"

code unsigned char sound[]={0x21, 0x10, 0x23, 0x10, 0x25, 0x10, 0x26, 0x20, 0x25, 0x10, 0x23, 0x10, 0x21, 0x20, 0x31, 0x30 };

// 判断是否要播放音乐
void is_play_music() 
{
    unsigned char state = GetPlayerMode();
    if (is_door_open[id] && state == enumModeStop){
        SetPlayerMode(enumModePlay);
    }
    else if (!is_door_open[id] && state == enumModePlay){
        SetPlayerMode(enumModeStop);
    }
}

// 播放音乐
void play_music(){
    SetMusic(135, 0xFA, sound, sizeof(sound)/sizeof(sound[0]), enumMscDrvSeg7andLed);
}