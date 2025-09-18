// SerialManager.h
// 本文件定义了 SerialManager 类，用于管理多个串口(QSerialPort)的打开/读取/写入，
// 并将从下位机接收的帧解析为结构化的数据，通过信号向上层（MainWindow）分发。
//
/// 备注：
/// - QSerialPort: Qt 提供的串口通信类，用于打开、配置和读写串口。
/// - QObject: Qt 的基类，提供对象树、信号槽机制等核心功能。

#ifndef SERIALMANAGER_H
#define SERIALMANAGER_H

#include <QObject>
#include <QSerialPort>
#include <QMap>
#include <QByteArray>
#include <QTimer>
#include <QDateTime>

class SerialManager : public QObject {
    Q_OBJECT
public:
    // 构造函数：可接受父对象指针，遵循 Qt 对象树的生命周期管理
    SerialManager(QObject *parent = nullptr);
    ~SerialManager();

    // 将构造好的命令帧写入对应车厢的串口（carId=0 表示广播到所有串口）
    void sendCommand(int carId, const QByteArray &cmd);  // 向指定车厢下发命令
    
    // 获取当前映射状态（车厢ID <-> 串口）
    bool isCarMapped(int carId) const { return carIdToPortMap.contains(carId); }
    QList<int> getMappedCarIds() const { return carIdToPortMap.keys(); }
    QString getPortNameForCar(int carId) const {
        return carIdToPortMap.contains(carId) ? carIdToPortMap[carId]->portName() : QString();
    }

signals:
    // dataReceived 信号：当接收到并解析出一条完整的数据帧时发送。
    // 参数说明：carId(车厢号)、temperature(温度)、tempThreshold(阈值)、speed(速度)、
    // etaMinutes/etaSeconds(到站时间)、mode(运行模式)、doorOk(车门状态)、alarmOn(报警)
    // hour/minute/second(实时时间，由上位机发送给下位机)
    // mode: 0=RUNNING_MODE, 1=STOP_MODE
    void dataReceived(int carId, int temperature, int tempThreshold, int speed, int etaMinutes, int etaSeconds, int mode, bool doorOk, bool alarmOn, int hour, int minute, int second);
    // alarmTriggered 信号：当检测到安防报警时发出（供上层记录/弹窗）
    void alarmTriggered(int carId, const QString &type, const QString &message);

private slots:
    // handleReadyRead 槽：串口有可读数据时 Qt 会触发 readyRead 信号，
    // 我们在这个槽中读取数据并累计到 per-port 缓冲区中，以便按行解析。
    void handleReadyRead();
    // handleBytesWritten 槽：可选，用于监测写入完成回调
    void handleBytesWritten(qint64 bytes);
    // handleTimeSyncTimer 槽：连接后1.5秒延迟触发，发送时间同步包
    void handleTimeSyncTimer();

private:
    // ports: 存储已创建的 QSerialPort 对象集合
    QVector<QSerialPort*> ports;
    // recvBuffers: 每个串口对应一个缓冲区，用于处理粘包和分帧
    QMap<QSerialPort*, QByteArray> recvBuffers;
    // portToCarIdMap: 动态学习到的 串口 -> 车厢ID 映射（当从某串口收到车厢ID时建立）
    QMap<QSerialPort*, int> portToCarIdMap;
    // carIdToPortMap: 车厢ID -> 串口 的反向映射，方便发送时定位串口
    QMap<int, QSerialPort*> carIdToPortMap;
    
    // 时间同步相关成员变量
    QTimer *timeSyncTimer;  // 连接后1.5秒延迟定时器
    // 存储每个车厢最后接收到的数据（用于构建时间同步包）
    struct LastReceivedData {
        int carId = 0;
        int temp = 0;
        int tempThreshold = 40;
        int speed = 0;
        int etaMin = 0;
        int etaSec = 0;
        int mode = 0;
        int door = 0;
        int alarm = 0;
        bool hasData = false;
    };
    QMap<int, LastReceivedData> lastDataPerCar;  // 按车厢ID缓存最后接收的数据

    // parseFrame: 解析一条完整的文本帧（不包含换行符），并触发对应的信号或映射学习
    void parseFrame(const QString &frame, QSerialPort *port);
    // sendTimeSyncPacket: 发送时间同步包，使用最后接收的数据但更新时间字段
    void sendTimeSyncPacket();
};
#endif // SERIALMANAGER_H
