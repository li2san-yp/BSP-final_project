#include "SerialManager.h"
#include <QSerialPortInfo>
#include <QThread>
#include <QDebug>
#include <QTimer>
#include <QDateTime>

// SerialManager 构造函数：创建并打开多个串口，配置参数并连接 readyRead 信号
SerialManager::SerialManager(QObject *parent) : QObject(parent) {
    // 初始化时间同步定时器
    timeSyncTimer = new QTimer(this);
    timeSyncTimer->setSingleShot(true);  // 只触发一次
    connect(timeSyncTimer, &QTimer::timeout, this, &SerialManager::handleTimeSyncTimer);
    
    // 假设使用 COM2, COM3, COM5
    int comNumber[3]={6,7,8};
    bool anyPortOpened = false;  // 标记是否有任何串口成功打开
    
    for(int i = 0; i < 3; ++i) {
        // QSerialPort：Qt 提供的串口操作类，负责打开、配置、读写
        QSerialPort *serial = new QSerialPort(this);
        // 配置串口参数（波特率、数据位、校验、停止位、流控）
        serial->setBaudRate(QSerialPort::Baud115200);
        serial->setDataBits(QSerialPort::Data8);
        serial->setParity(QSerialPort::NoParity);
        serial->setStopBits(QSerialPort::OneStop);
        serial->setFlowControl(QSerialPort::NoFlowControl);
        serial->setPortName(QString("COM%1").arg(comNumber[i]));
        // 尝试以读写方式打开串口，成功后清空缓冲并注册 readyRead 回调
        if(serial->open(QIODevice::ReadWrite)) {
            // 清空串口缓冲区，防止残留数据造成误报警
            serial->clear(QSerialPort::AllDirections);
            QThread::msleep(100); // 等待清空完成
            
            // 异步读取：收到数据时触发 readyRead() 信号
            connect(serial, &QSerialPort::readyRead, this, &SerialManager::handleReadyRead);
            // qDebug() << "Successfully opened and cleared" << serial->portName();
            anyPortOpened = true;  // 标记有串口成功打开
        } else {
            qWarning() << "无法打开串口" << serial->portName();
        }
        // 将串口对象保存到 ports 列表，后续用于写入或重试
        ports.append(serial);
    }
    
    // 如果有任何串口成功打开，启动1.5秒延迟定时器
    if(anyPortOpened) {
        qDebug() << "串口连接成功，1.5秒后将发送时间同步包";
        timeSyncTimer->start(1500);  // 1.5秒 = 1500毫秒
    }
}

// 析构函数：关闭所有打开的串口，释放资源
SerialManager::~SerialManager() {
    for(QSerialPort *p : ports) {
        if(p->isOpen()) p->close();
    }
}

// sendCommand：将命令帧写入串口
// 逻辑分两部分：广播（carId<=0）与定向发送（carId>0）
void SerialManager::sendCommand(int carId, const QByteArray &cmd) {
    // 将命令写入指定车厢串口（carId=0 表示广播）
    // qDebug() << "Sending to car" << carId << ":" << cmd; // 已注释：调试信息
    
    if(carId <= 0) {
        // 广播到所有串口
        for(QSerialPort *p : ports) {
            if(!p->isOpen()) {
                // 如果串口意外关闭，尝试重新打开（简单重连策略）
                if(!p->open(QIODevice::ReadWrite)) {
                    qWarning() << "无法重新打开串口" << p->portName() << "跳过发送命令";
                    continue;
                } else {
                    // 重新打开后需要重新设置 readyRead 连接并清空缓冲区
                    p->clear(QSerialPort::AllDirections);
                    connect(p, &QSerialPort::readyRead, this, &SerialManager::handleReadyRead, Qt::UniqueConnection);
                    qDebug() << "成功重新打开串口" << p->portName();
                }
            }
            // 写入命令帧到该串口
            p->write(cmd);
        }
    } else {
        // 使用动态映射表查找对应的串口，避免盲目广播
        if(carIdToPortMap.contains(carId)) {
            QSerialPort *targetPort = carIdToPortMap[carId];
            if(!targetPort->isOpen()) {
                // 如果找到的串口关闭，尝试重新打开
                if(!targetPort->open(QIODevice::ReadWrite)) {
                    qWarning() << "无法重新打开串口" << targetPort->portName() << "命令发送失败";
                    return;
                } else {
                    // 重新打开成功后重新注册 readyRead，并清空缓冲区
                    targetPort->clear(QSerialPort::AllDirections);
                    connect(targetPort, &QSerialPort::readyRead, this, &SerialManager::handleReadyRead, Qt::UniqueConnection);
                    qDebug() << "成功重新打开串口" << targetPort->portName();
                }
            }
            // 将数据写入目标串口
            targetPort->write(cmd);
            qDebug() << "已发送命令到车厢" << carId << "串口" << targetPort->portName();
        } else {
            // 如果尚未学习到该车厢的串口映射，则退化为广播发送，确保命令能被收到
            qWarning() << "未找到车厢" << carId << "对应的串口，可能尚未接收到该车厢的数据来建立映射关系";
            // 可选：尝试发送到所有串口（降级方案）
            qDebug() << "降级方案：广播命令到所有串口";
            for(QSerialPort *p : ports) {
                if(p->isOpen()) {
                    p->write(cmd);
                }
            }
        }
    }
}

// handleReadyRead：串口有数据可读时触发（由 QSerialPort::readyRead 信号连接）
// 功能：将串口读取的数据追加到对应的接收缓冲区，并按换行符分割为完整帧进行解析
void SerialManager::handleReadyRead() {
    // sender() 返回触发信号的对象，这里应是对应的 QSerialPort*
    QSerialPort *port = qobject_cast<QSerialPort*>(sender());
    if(!port) return;
    // 读取串口当前可用的数据，注意可能是部分帧或多个帧拼接
    QByteArray data = port->readAll();
    // append to buffer
    QByteArray &buf = recvBuffers[port];
    buf.append(data);
    // split by newline (支持 \r\n 或 \n)
    int idx = -1;
    while((idx = buf.indexOf('\n')) != -1) {
        QByteArray line = buf.left(idx);
        // remove optional CR
        if(!line.isEmpty() && line.endsWith('\r')) line.chop(1);
        // remove trailing NUL padding (接收端规定发送方在换行后用 NUL(0) 填充到 32 字节)
        while(!line.isEmpty() && line.endsWith('\0')) line.chop(1);
        // remove leading NUL padding (下位机可能在帧前也有 NUL 填充)
        while(!line.isEmpty() && line.startsWith('\0')) line.remove(0, 1);
        // 将字节数组按 UTF-8 解析为 QString
        QString frame = QString::fromUtf8(line);
        // parse this complete frame
        parseFrame(frame, port);
        // remove processed frame + newline
        buf.remove(0, idx + 1);
    }
}

// parseFrame：将一条完整的文本帧按逗号分隔并解析各个字段
void SerialManager::parseFrame(const QString &frame, QSerialPort *port) {
    // 新协议：帧以包头 BSP 开始，形如: BSP,carId,temp,tempThreshold,speed,etaMin,etaSec,mode,door,alarm,hour,minute,second
    // qDebug() << "Parsing frame:" << frame; // 添加调试输出
    QStringList parts = frame.split(',', Qt::KeepEmptyParts);
    // qDebug() << "Frame parts count:" << parts.size() << "Parts:" << parts; // 调试字段数量和内容
    
    // 校验字段数量，防止解析越界
    if(parts.size() != 13) {
        // qDebug() << "Frame rejected: expected 13 parts, got" << parts.size();
        return; // header + 12 numeric fields (增加了hour,minute,second)
    }
    // 验证包头是否为 BSP
    if(parts[0] != QLatin1String("BSP")) {
        // qDebug() << "Frame rejected: header not BSP, got" << parts[0];
        return; // 必须以 BSP 开头
    }
    
    bool ok = true;
    // 解析车厢 ID（整型）
    int carId = parts[1].toInt(&ok); 
    if(!ok) { // qDebug() << "Failed to parse carId:" << parts[1]; 
        return; }
    
    // 解析温度（整型）
    int temp = parts[2].toInt(&ok); 
    if(!ok) { // qDebug() << "Failed to parse temp:" << parts[2]; 
        return; }
    
    // 解析阈值（整型）
    int tempThreshold = parts[3].toInt(&ok); 
    if(!ok) { // qDebug() << "Failed to parse tempThreshold:" << parts[3]; 
        return; }
    
    // 解析速度（整型）
    int speed = parts[4].toInt(&ok); 
    if(!ok) { // qDebug() << "Failed to parse speed:" << parts[4]; 
        return; }
    
    // 解析到站时间（分钟）
    int etaMin = parts[5].toInt(&ok); 
    if(!ok) { // qDebug() << "Failed to parse etaMin:" << parts[5]; 
        return; }
    
    // 解析到站时间（秒）
    int etaSec = parts[6].toInt(&ok); 
    if(!ok) { // qDebug() << "Failed to parse etaSec:" << parts[6]; 
        return; }
    
    // 解析运行模式字段（新增）
    int mode = parts[7].toInt(&ok); 
    if(!ok) { // qDebug() << "Failed to parse mode:" << parts[7]; 
        return; }
    
    // 解析车门状态字段
    int doorVal = parts[8].toInt(&ok); 
    if(!ok) { // qDebug() << "Failed to parse doorVal:" << parts[8]; 
        return; }
    
    // 解析报警字段
    int alarmVal = parts[9].toInt(&ok); 
    if(!ok) { // qDebug() << "Failed to parse alarmVal:" << parts[9]; 
        return; }
    
    // 解析时间字段（hour, minute, second）- 由上位机发送给下位机，接收时不做特殊处理
    int hour = parts[10].toInt(&ok);
    if(!ok) { // qDebug() << "Failed to parse hour:" << parts[10]; 
        return; }
    // qDebug()<<"Time received - hour:"<<hour;
    
    int minute = parts[11].toInt(&ok);
    if(!ok) { // qDebug() << "Failed to parse minute:" << parts[11]; 
        return; }
    // qDebug()<<"Time received - minute:"<<minute;
    
    int second = parts[12].toInt(&ok);
    if(!ok) { // qDebug() << "Failed to parse second:" << parts[12]; 
        return; }
    // qDebug()<<"Time received - second:"<<second;

    // 转换门和报警状态 - 注意：即使值不是0/1也应该能处理
    bool doorOpen = (doorVal != 0); // 改为非0即为开启
    bool alarmOn = (alarmVal != 0);  // 改为非0即为报警
    
    // 建立串口到车厢ID的动态映射关系
    // 避免覆盖：如果该车厢ID已经映射到其他串口，保持原有映射不变
    if(!portToCarIdMap.contains(port) || portToCarIdMap[port] != carId) {
        if(!carIdToPortMap.contains(carId)) {
            // 只有当该车厢ID尚未映射时才建立新映射
            portToCarIdMap[port] = carId;
            carIdToPortMap[carId] = port;
            qDebug() << "学习到映射关系: 串口" << port->portName() << "-> 车厢" << carId;
        } else if(carIdToPortMap[carId] != port) {
            // 如果车厢ID已映射到其他串口，发出警告但不覆盖
            qWarning() << "警告：车厢" << carId << "已映射到串口" << carIdToPortMap[carId]->portName() 
                      << "，忽略来自串口" << port->portName() << "的重复映射请求";
        }
    }
    
    // qDebug() << "Successfully parsed frame - carId:" << carId << "temp:" << temp 
             // << "speed:" << speed << "etaSec:" << etaSec << "mode:" << mode << "door:" << doorOpen << "alarm:" << alarmOn;

    // 缓存最后接收到的数据（用于时间同步包构建）
    LastReceivedData &data = lastDataPerCar[carId];
    data.carId = carId;
    data.temp = temp;
    data.tempThreshold = tempThreshold;
    data.speed = speed;
    data.etaMin = etaMin;
    data.etaSec = etaSec;
    data.mode = mode;
    data.door = doorVal;
    data.alarm = alarmVal;
    data.hasData = true;

    // 发射 dataReceived 信号，将解析后的数据上报给主窗口
    emit dataReceived(carId, temp, tempThreshold, speed, etaMin, etaSec, mode, doorOpen, alarmOn, hour, minute, second);
    if(alarmOn) {
        QString msg = tr("车厢%1异常: 温度 %2℃, 车速 %3, 到站 %4分%5秒, 门 %6")
                          .arg(carId).arg(temp).arg(speed).arg(etaMin).arg(etaSec).arg(doorOpen ? tr("打开") : tr("关闭"));
        emit alarmTriggered(carId, tr("温度报警"), msg);
    }
}

// （可选）监测写入完成信号
void SerialManager::handleBytesWritten(qint64 bytes) {
    Q_UNUSED(bytes);
    // 可用于确认数据发送情况
}

// handleTimeSyncTimer：定时器触发时发送时间同步包
void SerialManager::handleTimeSyncTimer() {
    qDebug() << "时间同步定时器触发，开始发送时间同步包";
    sendTimeSyncPacket();
}

// sendTimeSyncPacket：发送时间同步包给所有已知车厢
void SerialManager::sendTimeSyncPacket() {
    // 获取当前实时时间
    QDateTime currentTime = QDateTime::currentDateTime();
    int hour = currentTime.time().hour();
    int minute = currentTime.time().minute();
    int second = currentTime.time().second();
    
    qDebug() << "准备发送时间同步包，当前时间:" << hour << ":" << minute << ":" << second;
    
    // 遍历所有已缓存的车厢数据
    for(auto it = lastDataPerCar.begin(); it != lastDataPerCar.end(); ++it) {
        const LastReceivedData &data = it.value();
        if(!data.hasData) {
            qDebug() << "车厢" << data.carId << "无有效数据，跳过时间同步";
            continue;
        }
        
        // 构建时间同步包：使用原始数据但更新时间字段
        QByteArray frame = "BSP," + QByteArray::number(data.carId) + "," 
                         + QByteArray::number(data.temp) + "," 
                         + QByteArray::number(data.tempThreshold) + ","
                         + QByteArray::number(data.speed) + "," 
                         + QByteArray::number(data.etaMin) + "," 
                         + QByteArray::number(data.etaSec) + "," 
                         + QByteArray::number(data.mode) + "," 
                         + QByteArray::number(data.door) + "," 
                         + QByteArray::number(data.alarm) + "," 
                         + QByteArray::number(hour) + "," 
                         + QByteArray::number(minute) + "," 
                         + QByteArray::number(second) + "\n";
        
        // 填充到50字节
        while(frame.size() < 50) frame.append('\0');
        
        // 发送到对应车厢的串口
        if(carIdToPortMap.contains(data.carId)) {
            QSerialPort *targetPort = carIdToPortMap[data.carId];
            if(targetPort && targetPort->isOpen()) {
                targetPort->write(frame);
                qDebug() << frame;
            } else {
                qWarning() << "车厢" << data.carId << "对应的串口未打开，无法发送时间同步包";
            }
        } else {
            qWarning() << "车厢" << data.carId << "未建立串口映射，无法发送时间同步包";
        }
    }
    
    // 如果没有任何车厢数据，记录警告
    if(lastDataPerCar.isEmpty()) {
        qWarning() << "没有接收到任何车厢数据，无法发送时间同步包";
    }
}
