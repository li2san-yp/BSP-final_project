/**********************************自定义串口1通信模块实现 V1.0 ************************************************************************
自定义串口1模块的具体实现，提供下位机与上位机的通信功能
*/

#include "my_uart1.h"
#include "core.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// 全局变量定义
char g_rxBuffer[MY_UART1_RX_BUFFER_SIZE] = {0};
char g_txBuffer[MY_UART1_TX_BUFFER_SIZE] = {0};
static MyUart1RxStatus_t s_rxStatus = MY_UART1_RX_IDLE;
static char s_headerMatch[MY_UART1_HEADER_SIZE + 1] = MY_UART1_FRAME_HEADER;

// 内部函数声明
static void Uart1RxdCallback(void);
static int ParseCommand(char *cmdStr);
static int my_utoa(unsigned int value, char *buf);

/**
 * @brief 串口1接收事件回调函数
 */
static void Uart1RxdCallback(void)
{
    char *newlinePos = strchr(g_rxBuffer, MY_UART1_FRAME_END);
    
    if (newlinePos != 0) {
        *newlinePos = '\0';
        s_rxStatus = MY_UART1_RX_COMPLETE;
        
        // 立即解析并处理接收到的数据
        ParseCommand(g_rxBuffer);
        
        // 清空接收缓冲区，准备下次接收
        MyUart1ClearRxBuffer();
    } else {
        s_rxStatus = MY_UART1_RX_ERROR;
    }
}

/**
 * @brief 初始化自定义串口1模块
 */
void MyUart1Init(void)
{
    Uart1Init(MY_UART1_BAUD_RATE);
    memset(g_rxBuffer, 0, MY_UART1_RX_BUFFER_SIZE);
    memset(g_txBuffer, 0, MY_UART1_TX_BUFFER_SIZE);
    SetEventCallBack(enumEventUart1Rxd, Uart1RxdCallback);
    SetUart1Rxd(g_rxBuffer, MY_UART1_RX_BUFFER_SIZE, s_headerMatch, MY_UART1_HEADER_SIZE);
    s_rxStatus = MY_UART1_RX_IDLE;
}

/**
 * @brief 清空接收缓冲区
 */
void MyUart1ClearRxBuffer(void)
{
    memset(g_rxBuffer, 0, MY_UART1_RX_BUFFER_SIZE);
    s_rxStatus = MY_UART1_RX_IDLE;
}

/**
 * @brief 处理接收错误状态（可选调用）
 */
void MyUart1ProcessRxData(void)
{
    // 检查是否有接收错误需要处理
    if (s_rxStatus == MY_UART1_RX_ERROR) {
        // 发生错误时清空缓冲区重新开始
        MyUart1ClearRxBuffer();
    }
}

/**
 * @brief 解析接收到的命令字符串
 */
static int ParseCommand(char *cmdStr)
{
    if (cmdStr == 0 || strlen(cmdStr) == 0) {
        return -1;
    }
    
    char tempBuffer[MY_UART1_RX_BUFFER_SIZE];
    strcpy(tempBuffer, cmdStr);
    
    char *token;
    int fieldIndex = 0;
    
    unsigned int tempCarId;
    unsigned int tempTemp, tempTempThresholds;
    unsigned int tempSpeed, tempEtaMin, tempEtaSec;
    unsigned char tempDoor, tempAlarm;
    
    token = strtok(tempBuffer, ",");
    while (token != 0 && fieldIndex < MY_UART1_FIELD_COUNT) {
        switch (fieldIndex) {
            case 0: // 包头BSP，跳过验证
                break;
            case 1: // carId
                tempCarId = (unsigned int)atoi(token);
                break;
            case 2: // temp
                tempTemp = (unsigned int)atoi(token);
                break;
            case 3: // tempThresholds
                tempTempThresholds = (unsigned int)atoi(token);
                break;
            case 4: // speed
                tempSpeed = (unsigned int)atoi(token);
                break;
            case 5: // etaMin
                tempEtaMin = (unsigned int)atoi(token);
                break;
            case 6: // etaSec
                tempEtaSec = (unsigned int)atoi(token);
                break;
            case 7: // door
                tempDoor = (unsigned char)atoi(token);
                if (tempDoor > 1) tempDoor = 0;
                break;
            case 8: // alarm
                tempAlarm = (unsigned char)atoi(token);
                if (tempAlarm > 1) tempAlarm = 0;
                break;
        }
        
        fieldIndex++;
        token = strtok(0, ",");
    }
    
    if (fieldIndex < MY_UART1_FIELD_COUNT) {
        return -1;
    }
    
    // 直接更新各模块的全局变量
    // id = tempCarId;  // 全局车辆ID固定，不需要更新
    
    // temp, etaMin, etaSec 这三个值在现实中不可直接修改，不接受上位机修改
    // temp[id] = tempTemp;  // 注释掉：温度值应由传感器读取
    
    tempThresholds[id] = tempTempThresholds;  // 温度阈值可以设置
    
    speed[id] = tempSpeed;  // 速度可以设置
    
    // rtc_time[id].minute = tempEtaMin;  // 注释掉：ETA由系统计算
    // rtc_time[id].second = tempEtaSec;  // 注释掉：ETA由系统计算
    
    is_door_open[id] = tempDoor;  // 门状态可以控制
    is_alarm[id] = tempAlarm;     // 报警状态可以控制
    
    return 0;
}

// 整形转字符串（无符号，十进制，返回写入的字符数）
static int my_utoa(unsigned int value, char *buf)
{
    int len = 0;
    unsigned int temp = value;
    char rev[10]; // 最大支持10位数
    int i = 0;
    if (value == 0) {
        buf[0] = '0';
        buf[1] = '\0';
        return 1;
    }
    // 逐位取余，逆序存入rev
    while (temp > 0 && i < 10) {
        rev[i++] = (temp % 10) + '0';
        temp /= 10;
    }
    // 再逆序写入buf
    for (int j = i - 1; j >= 0; j--) {
        buf[len++] = rev[j];
    }
    buf[len] = '\0';
    return len;
}

/**
 * @brief 从系统全局变量获取当前状态并发送给上位机
 * @return 发送结果
 */
char MyUart1SendCurrentStatus(void)
{
    if (GetUart1TxStatus() == enumUart1TxBusy) {
        return enumUart1TxFailure;
    }
    
    // 清空发送缓冲区
    memset(g_txBuffer, 0, MY_UART1_TX_BUFFER_SIZE);
    int pos = 0;
    
    // 拼接包头
    strcpy(g_txBuffer, MY_UART1_FRAME_HEADER);
    pos = strlen(g_txBuffer);
    g_txBuffer[pos++] = ',';
    
    // 拼接各字段，从全局变量获取实时数据
    pos += my_utoa(id, &g_txBuffer[pos]);  // 使用全局变量id
    g_txBuffer[pos++] = ',';
    
    pos += my_utoa(temperature[id], &g_txBuffer[pos]);   // 当前温度（使用数组）
    g_txBuffer[pos++] = ',';
    
    pos += my_utoa(tempThresholds[id], &g_txBuffer[pos]);  // 温度阈值
    g_txBuffer[pos++] = ',';
    
    pos += my_utoa(speed[id], &g_txBuffer[pos]);  // 当前速度
    g_txBuffer[pos++] = ',';
    
    pos += my_utoa(rtc_time[id].minute, &g_txBuffer[pos]);  // ETA分钟
    g_txBuffer[pos++] = ',';
    
    pos += my_utoa(rtc_time[id].second, &g_txBuffer[pos]);  // ETA秒数
    g_txBuffer[pos++] = ',';
    
    pos += my_utoa(is_door_open[id], &g_txBuffer[pos]);  // 门状态
    g_txBuffer[pos++] = ',';
    
    pos += my_utoa(is_alarm[id], &g_txBuffer[pos]);  // 报警状态
    
    // 结尾换行
    g_txBuffer[pos++] = '\n';
    
    // 检查数据长度是否超出缓冲区
    if (pos >= MY_UART1_TX_BUFFER_SIZE) {
        return enumUart1TxFailure;
    }
    
    // 固定发送32字节
    return Uart1Print(g_txBuffer, MY_UART1_TX_BUFFER_SIZE);
}

