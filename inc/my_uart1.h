/**********************************自定义串口1通信模块 V1.0 说明************************************************************************
自定义串口1模块提供下位机与上位机的通信功能：
(1) 下位机数据上报：按照指定协议格式发送设备状态数据
(2) 上位机命令接收：接收并解析上位机发送的控制命令

通信协议格式：
发送格式：BSP,carId,temp,tempThresholds,speed,etaMin,etaSec,door,alarm\n
字段说明（所有字段均为整型）：
- BSP: 包头标识（固定为"BSP"）
- carId: 车辆ID（整型）
- temp: 当前温度（整型）
- tempThresholds: 温度阈值（整型）
- speed: 速度（整型）
- etaMin: 预计到达时间分钟（整型）
- etaSec: 预计到达时间秒数（整型）
- door: 门状态（0关闭，1打开）
- alarm: 报警状态（0无报警，1有报警）

串口配置（与上位机对齐）：
- 波特率：115200 bps
- 数据位：8位
- 奇偶校验：无
- 停止位：1位
- 流控制：无

*/

#ifndef _MY_UART1_H_
#define _MY_UART1_H_

#include "core.h"

// 通信协议相关常量定义
#define MY_UART1_BAUD_RATE      115200      // 串口波特率（与上位机对齐）
#define MY_UART1_TX_BUFFER_SIZE 32          // 发送缓冲区大小
#define MY_UART1_RX_BUFFER_SIZE 32          // 接收缓冲区大小
#define MY_UART1_FRAME_END      '\n'        // 帧结束符
#define MY_UART1_FIELD_COUNT    9           // 数据字段数量（包含包头）
#define MY_UART1_FRAME_HEADER   "BSP"       // 数据包头标识
#define MY_UART1_HEADER_SIZE    3            // 包头长度

// 设备状态数据结构（所有字段均为整型）
typedef struct {
    unsigned int carId;           // 车辆ID
    unsigned int temp;            // 当前温度（整型）
    unsigned int tempThresholds;  // 温度阈值（整型）
    unsigned int speed;           // 速度
    unsigned int etaMin;          // 预计到达时间分钟
    unsigned int etaSec;          // 预计到达时间秒数
    unsigned char door;           // 门状态（0关闭，1打开）
    unsigned char alarm;          // 报警状态（0无报警，1有报警）
} DeviceData_t;

// 接收状态枚举
typedef enum {
    MY_UART1_RX_IDLE = 0,        // 空闲状态
    MY_UART1_RX_RECEIVING,       // 接收中
    MY_UART1_RX_COMPLETE,        // 接收完成
    MY_UART1_RX_ERROR            // 接收错误
} MyUart1RxStatus_t;

// 公共函数声明
extern void MyUart1Init(void);                                    // 初始化自定义串口1模块
extern char MyUart1SendDeviceData(DeviceData_t *data);           // 发送设备数据
extern MyUart1RxStatus_t MyUart1GetRxStatus(void);               // 获取接收状态
extern char* MyUart1GetRxData(void);                             // 获取接收到的数据
extern void MyUart1ClearRxBuffer(void);                          // 清空接收缓冲区
extern void MyUart1ProcessRxData(void);                          // 处理接收到的数据
extern int MyUart1ProcessCommand(void);                          // 处理完整的接收命令

// 外部变量声明
extern DeviceData_t g_deviceData;                                // 全局设备数据
extern char g_rxBuffer[MY_UART1_RX_BUFFER_SIZE];                 // 接收缓冲区
extern char g_txBuffer[MY_UART1_TX_BUFFER_SIZE];                 // 发送缓冲区

/*
使用示例：

1. 初始化（在main函数中调用）：
   MyUart1Init();

2. 发送设备数据：
   DeviceData_t data = {1, 23, 45, 2, 30, 1, 0, 0};
   MyUart1SendDeviceData(&data);

3. 在主循环中处理接收数据：
   while(1) {
       MySTC_OS();
       MyUart1ProcessRxData();
       
       // 检查是否收到新数据
       if(MyUart1GetRxStatus() == MY_UART1_RX_COMPLETE) {
           // 数据已自动解析到 g_deviceData 中
           // 在这里处理收到的数据
       }
   }

通信协议示例（所有字段均为整型，带包头）：
发送：BSP,1,23,45,2,30,1,0,0\n
接收：BSP,2,25,40,5,15,30,1,1\n
*/

#endif