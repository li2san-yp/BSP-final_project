#include "core.h"
#include "M24C02.h"
#include "displayer.h"

// 在EEPROM中存储本机ID的地址
#define CURRENT_TRAIN_ID_ADDR 0x10

// 初始化本机的列车ID
void InitTrainID(void)
{
    id = NVM_Read(CURRENT_TRAIN_ID_ADDR); // 读取本机的列车ID
}

// 设置本机的列车ID
void SetTrainID(unsigned char new_id)
{
    // 存储新ID到EEPROM
    NVM_Write(CURRENT_TRAIN_ID_ADDR, new_id);
    id = new_id;
}

