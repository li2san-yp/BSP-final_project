/**********************************自定义串口1通信模块实现 V1.0 ************************************************************************
自定义串口1模块的具体实现，提供下位机与上位机的通信功能
*/

#include "my_uart1.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// 全局变量定义
DeviceData_t g_deviceData = {0};
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
 * @brief 发送设备数据到上位机（固定32字节）
 */
char MyUart1SendDeviceData(DeviceData_t *data)
{
    if (data == 0) {
        return enumUart1TxFailure;
    }
    if (GetUart1TxStatus() == enumUart1TxBusy) {
        return enumUart1TxFailure;
    }
    // 清空发送缓冲区，确保后面都是0
    memset(g_txBuffer, 0, MY_UART1_TX_BUFFER_SIZE);
    int pos = 0;
    // 拼接包头
    strcpy(g_txBuffer, MY_UART1_FRAME_HEADER);
    pos = strlen(g_txBuffer);
    g_txBuffer[pos++] = ',';
    // 拼接各字段
    pos += my_utoa(data->carId, &g_txBuffer[pos]);
    g_txBuffer[pos++] = ',';
    pos += my_utoa(data->temp, &g_txBuffer[pos]);
    g_txBuffer[pos++] = ',';
    pos += my_utoa(data->tempThresholds, &g_txBuffer[pos]);
    g_txBuffer[pos++] = ',';
    pos += my_utoa(data->speed, &g_txBuffer[pos]);
    g_txBuffer[pos++] = ',';
    pos += my_utoa(data->etaMin, &g_txBuffer[pos]);
    g_txBuffer[pos++] = ',';
    pos += my_utoa(data->etaSec, &g_txBuffer[pos]);
    g_txBuffer[pos++] = ',';
    pos += my_utoa(data->door, &g_txBuffer[pos]);
    g_txBuffer[pos++] = ',';
    pos += my_utoa(data->alarm, &g_txBuffer[pos]);
    // 结尾换行
    g_txBuffer[pos++] = '\n';
    // 检查数据长度是否超出缓冲区
    if (pos >= MY_UART1_TX_BUFFER_SIZE) {
        return enumUart1TxFailure;
    }
    // 固定发送32字节（包含换行符后的0填充）
    return Uart1Print(g_txBuffer, MY_UART1_TX_BUFFER_SIZE);
}

/**
 * @brief 获取接收状态
 */
MyUart1RxStatus_t MyUart1GetRxStatus(void)
{
    return s_rxStatus;
}

/**
 * @brief 获取接收到的数据
 */
char* MyUart1GetRxData(void)
{
    if (s_rxStatus == MY_UART1_RX_COMPLETE) {
        return g_rxBuffer;
    }
    return 0;
}

/**
 * @brief 清空接收缓冲区
 */
void MyUart1ClearRxBuffer(void)
{
    memset(g_rxBuffer, 0, MY_UART1_RX_BUFFER_SIZE);
    s_rxStatus = MY_UART1_RX_IDLE;
    SetUart1Rxd(g_rxBuffer, MY_UART1_RX_BUFFER_SIZE, s_headerMatch, MY_UART1_HEADER_SIZE);
}

/**
 * @brief 处理接收到的数据
 */
void MyUart1ProcessRxData(void)
{
    if (s_rxStatus == MY_UART1_RX_COMPLETE) {
        MyUart1ProcessCommand();
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
    
    g_deviceData.carId = tempCarId;
    g_deviceData.temp = tempTemp;
    g_deviceData.tempThresholds = tempTempThresholds;
    g_deviceData.speed = tempSpeed;
    g_deviceData.etaMin = tempEtaMin;
    g_deviceData.etaSec = tempEtaSec;
    g_deviceData.door = tempDoor;
    g_deviceData.alarm = tempAlarm;
    
    return 0;
}

/**
 * @brief 处理完整的接收命令
 */
int MyUart1ProcessCommand(void)
{
    if (s_rxStatus != MY_UART1_RX_COMPLETE) {
        return -1;
    }
    
    int result = ParseCommand(g_rxBuffer);
    MyUart1ClearRxBuffer();
    
    return result;
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