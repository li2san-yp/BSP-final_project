#include "MainWindow.h"
#include <QTabWidget>
#include <QVBoxLayout>
#include <QTableWidget>
#include <QPushButton>
#include <QCheckBox>
#include <QComboBox>
#include <QSpinBox>
#include <QLabel>
#include <QHeaderView>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QVector>
#include <QTimer>
#include <QApplication>
#include <QSpinBox>
#include <QGroupBox>
#include <QFormLayout>
#include <QtWidgets/QFileDialog>
#include <QMessageBox>
#include <QStringConverter>
#include <QDateTime>
#include "StatusPage.h"
#include "AlarmPage.h"
#include "ControlPage.h"
#include "DiagnosisPage.h"
#include <QDebug>

// MainWindow.cpp 主要职责：
// - 初始化各个页面（Status/Alarm/Control/Diagnosis）并将它们添加到QTabWidget中
// - 与 SerialManager 建立连接，接收解析后的数据并进行分发与展示
// - 缓存实时数据以支持 ControlPage 的自动填充和 Diagnosis 页的历史曲线绘制


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    this->resize(1000, 600);

    // 创建串口管理器（负责底层数据收发）和报警日志组件
    serialManager = new SerialManager(this);
    alarmLog = new AlarmLog(this);
    // 不再在 MainWindow 中持有 chartWidget，让 DiagnosisPage 自己创建

    // 创建选项卡并使用独立的 page 类封装每页逻辑
    tabs = new QTabWidget(this);
        // 直接创建页面组件，不使用额外的中间 QWidget 作为 parent，避免多余边距
        // statusPage/alarmPage/controlPage/diagnosisPage 不再作为 container 使用
        statusPage = nullptr;
        alarmPage = nullptr;
        controlPage = nullptr;
        diagnosisPage = nullptr;

    // 创建页面组件（将 alarmLog 传入相应页面；DiagnosisPage 自己创建 ChartWidget）
        statusWidget = new StatusPage(nullptr);
        alarmWidget = new AlarmPage(alarmLog, nullptr);
        controlWidget = new ControlPage(nullptr);
        diagnosisWidget = new DiagnosisPage(nullptr, nullptr); // 传 nullptr，让它自己创建 ChartWidget

    // 将各页面添加到选项卡
    tabs->addTab(statusWidget, tr("实时监控"));
    tabs->addTab(alarmWidget, tr("报警日志"));
    tabs->addTab(controlWidget, tr("远程控制"));
    tabs->addTab(diagnosisWidget, tr("维护诊断"));
    setCentralWidget(tabs);

    // 连接：SerialManager 的 dataReceived 信号到 MainWindow::onDataReceived 槽
    // 通过信号槽机制实现数据驱动的界面更新
    connect(serialManager, &SerialManager::dataReceived, this, &MainWindow::onDataReceived);
    connect(serialManager, &SerialManager::alarmTriggered, this, &MainWindow::onAlarmTriggered);

    // 连接 ControlPage 的 sendControl 信号到 MainWindow 的 onSendControl，以便统一构建帧并发送
    connect(controlWidget, &ControlPage::sendControl, this, &MainWindow::onSendControl);
    // 当 ControlPage 选择变化时，把缓存的最新值推送过去以自动填充
    connect(controlWidget, &ControlPage::carSelected, this, [this](int carId){
        if(carId <= 0) return; // 广播不填充
        pushCachedToControl(carId);
    });
    // 连接 DiagnosisPage 的导出信号
    connect(diagnosisWidget, &DiagnosisPage::exportTemperatureHistory, 
            this, &MainWindow::onExportTemperatureHistory);

    // 初始化：默认车厢阈值与历史缓存
    QList<int> carIds;
    for(int id = 1; id <= 3; ++id) {
        tempThresholds[id] = 40; // 默认阈值（修正为整数）
        tempHistory[id] = QVector<QPoint>();
        carIds.append(id);
    }
    statusWidget->initCars(carIds);
    
    // 添加启动延迟，避免程序刚启动时的数据误报
    startupTimer = new QTimer(this);
    startupTimer->setSingleShot(true);
    startupTimer->start(1000); // 2秒延迟
    // qDebug() << "Startup delay timer started - ignoring alarms for 2 seconds";

    // 控制页面自动填充定时器，每10秒更新一次
    autoFillTimer = new QTimer(this);
    autoFillTimer->setInterval(10000); // 10秒间隔
    connect(autoFillTimer, &QTimer::timeout, this, [this]() {
        int currentCarId = controlWidget->currentCarId();
        if(currentCarId > 0) {
            pushCachedToControl(currentCarId);
        }
    });
    autoFillTimer->start();

    // 小型闪烁定时器，用于触发表格闪烁效果（占位）
    QTimer *blinkTimer = new QTimer(this);
    blinkTimer->setInterval(500);
    connect(blinkTimer, &QTimer::timeout, this, [this]() {
        // 简单实现：未实现复杂闪烁队列，仅占位
    });
    blinkTimer->start();
}

// onDataReceived：处理从 SerialManager 发来的结构化数据
// 主要任务：更新历史数据缓存、缓存最新值、更新图表/状态页、判断报警并记录
void MainWindow::onDataReceived(int carId, int temp, int tempThreshold, int speed, int etaMinutes, int etaSeconds, int mode, bool doorOpen, bool alarmOn, int hour, int minute, int second) {
    // qDebug() << "=== MainWindow received data ===";
    // qDebug() << "carId:" << carId << "temp:" << temp << "tempThreshold:" << tempThreshold<< "speed:" << speed << "etaMin:" << etaMinutes << "etaSec:" << etaSeconds << "mode:" << mode << "doorOpen:" << doorOpen << "alarmOn:" << alarmOn;
    // 时间参数 hour, minute, second 仅用于上位机发送给下位机，接收时不做特殊处理
    Q_UNUSED(hour); Q_UNUSED(minute); Q_UNUSED(second);
    
    // 将接收到的温度追加到对应车厢的历史向量中，最多保留1000个点
    if(!tempHistory.contains(carId)) tempHistory[carId] = QVector<QPoint>();
    QVector<QPoint> &hist = tempHistory[carId];
    int idx = hist.size() > 0 ? hist.last().x() + 1 : 0;
    hist.append(QPoint(idx, temp));  // 直接使用整数温度值
    while(hist.size() > 1000) hist.remove(0);
    // 更新本地存储的阈值（上位机从下位机同步阈值，整数 °C）
    tempThresholds[carId] = tempThreshold;

    // 更新最近值缓存
    lastTemp[carId] = temp;
    lastTempThreshold[carId] = tempThreshold;
    lastSpeed[carId] = speed;
    lastMode[carId] = mode;  // 缓存运行模式
    lastDoor[carId] = doorOpen ? 1 : 0;
    lastAlarm[carId] = alarmOn ? 1 : 0;

    // 注释掉频繁的自动填充，改为由定时器控制（10秒一次）
    // 如果 control 页正在显示或选择了该车，则推送最新值以自动填充
    // int ctrlCar = controlWidget->currentCarId();
    // if(ctrlCar == carId) {
    //     pushCachedToControl(carId);
    // }
    
    // 更新温度历史曲线显示
    // qDebug() << "About to update chart - diagnosisWidget:" << (diagnosisWidget != nullptr)  << "tempHistory.contains(" << carId << "):" << tempHistory.contains(carId)<< "history size:" << (tempHistory.contains(carId) ? tempHistory[carId].size() : -1);
    if(diagnosisWidget && tempHistory.contains(carId)) {
        diagnosisWidget->updateTemperatureChart(carId, tempHistory[carId]);
        // qDebug() << "Called updateTemperatureChart for carId" << carId << "with" << tempHistory[carId].size() << "points";
    }
    
    // 报警判断：检查安防报警或温度超阈值
    // 获取该车厢的阈值，如果不存在则使用默认值40度
    int currentThreshold = tempThresholds.value(carId, 40);
    bool tempExceeded = (temp > currentThreshold);
    
    // 在启动期间（前2秒）忽略所有报警，防止程序启动时的误报
    bool startupPeriod = startupTimer->isActive();
    bool triggered = !startupPeriod && (alarmOn || tempExceeded);
    
    // qDebug() << "Alarm check for carId" << carId << "- temp:" << temp << "threshold:" << currentThreshold << "alarmOn:" << alarmOn << "tempExceeded:" << tempExceeded << "startupPeriod:" << startupPeriod<< "triggered:" << triggered;
    
    // 更新状态页显示（含 mode 字段，用于 ETA/停站时间显示控制）
    statusWidget->updateStatus(carId, temp, currentThreshold, speed, etaMinutes, etaSeconds, mode, doorOpen, triggered);
    if(triggered) {
        // 检查报警冷却时间：同一车厢1分钟内只报警一次
        QDateTime currentTime = QDateTime::currentDateTime();
        bool shouldLog = true;
        
        if(lastAlarmTime.contains(carId)) {
            QDateTime lastTime = lastAlarmTime[carId];
            qint64 secondsSinceLastAlarm = lastTime.secsTo(currentTime);
            shouldLog = (secondsSinceLastAlarm >= 60); // 60秒 = 1分钟
        }
        
        if(shouldLog) {
            QApplication::beep();
            // QString type = alarmOn ? tr("安防报警") : tr("温度/门异常");
            QString type = tr("温度报警");
            QString msg = tr("车厢%1 异常: 温度 %2℃, 车速 %3, 到站 %4分%5秒, 门 %6")
                          .arg(carId).arg(temp).arg(speed).arg(etaMinutes).arg(etaSeconds).arg(doorOpen ? tr("打开") : tr("关闭"));
            alarmLog->addEntry(carId, type, msg);
            // 更新最后报警时间
            lastAlarmTime[carId] = currentTime;
        }
    }
}

// pushCachedToControl：将缓存的最近值推送到 ControlPage 以供自动填充
void MainWindow::pushCachedToControl(int carId) {
    if(!controlWidget) return;
    if(carId <= 0) return;
    int temp = lastTemp.value(carId, 0);
    int tempThreshold = lastTempThreshold.value(carId, tempThresholds.value(carId, 40));
    int speed = lastSpeed.value(carId, 0);
    int mode = lastMode.value(carId, 0);  // 默认运行模式
    int door = lastDoor.value(carId, 0);
    int alarm = lastAlarm.value(carId, 0);
    controlWidget->setValues(temp, tempThreshold, speed, mode, door, alarm);
}

// onAlarmTriggered：处理上层/下位机触发的报警事件（不走 dataReceived 路径时）
void MainWindow::onAlarmTriggered(int carId, const QString &type, const QString &message) {
    // 检查报警冷却时间：同一车厢1分钟内只报警一次
    QDateTime currentTime = QDateTime::currentDateTime();
    bool shouldLog = true;
    
    if(lastAlarmTime.contains(carId)) {
        QDateTime lastTime = lastAlarmTime[carId];
        qint64 secondsSinceLastAlarm = lastTime.secsTo(currentTime);
        shouldLog = (secondsSinceLastAlarm >= 60); // 60秒 = 1分钟
    }
    
    if(shouldLog) {
        // 将报警信息添加到报警日志表格并调用日志保存函数
        alarmLog->addEntry(carId, type, message);
        // 更新最后报警时间
        lastAlarmTime[carId] = currentTime;
    }
}

// onSetThreshold：设置指定车厢或广播设置温度阈值
void MainWindow::onSetThreshold(int carId, int threshold) {
    if(carId <= 0) {
        // 广播设置：应用到所有车厢
        for(auto key : tempThresholds.keys()) tempThresholds[key] = threshold;
    } else {
        tempThresholds[carId] = threshold;
    }
}

// onSendControl：构建 BSP 协议帧并通过 SerialManager 下发（支持 mode 字段）
void MainWindow::onSendControl(int carId, int temp, int tempThreshold, int speed, int etaMin, int etaSec, int mode, int door, int alarm) {
    // 获取当前时间
    QDateTime currentTime = QDateTime::currentDateTime();
    int hour = currentTime.time().hour();
    int minute = currentTime.time().minute();
    int second = currentTime.time().second();
    
    // 使用 ControlPage 提供的真实参数构建并下发帧（整数协议）
    // 新协议：在每帧前加包头 "BSP," 并添加当前时间（hour, minute, second）
    QByteArray frame = "BSP," + QByteArray::number(carId) + "," + QByteArray::number(temp) + "," + QByteArray::number(tempThreshold) + ","
        + QByteArray::number(speed) + "," + QByteArray::number(etaMin) + "," + QByteArray::number(etaSec) + "," + QByteArray::number(mode) + "," 
        + QByteArray::number(door) + "," + QByteArray::number(alarm) + "," + QByteArray::number(hour) + "," + QByteArray::number(minute) + "," + QByteArray::number(second) + "\n";
    qDebug()<<"Current time for control frame - hour:"<<hour<<"minute:"<<minute<<"second:"<<second;
    // qDebug() << "MainWindow sending frame:" << frame;
    // 接收端要求每帧固定 50 字节；如果不足，在换行符后用 NUL(0) 填充
    while(frame.size() < 50) frame.append('\0');
    serialManager->sendCommand(carId, frame);
    // 可选：更新本地缓存的阈值，确保界面一致
    tempThresholds[carId] = tempThreshold;
}

MainWindow::~MainWindow() { }

// onExportAlarms：导出报警数据为 CSV
void MainWindow::onExportAlarms() {
    QString fileName = QFileDialog::getSaveFileName(this, tr("导出报警 CSV"), "alarms_export.csv", tr("CSV Files (*.csv);;All Files (*)"));
    if(fileName.isEmpty()) return;
    alarmLog->exportToCsv(fileName);
}

// onExportTemperatureHistory：导出指定车厢温度历史（按抽样间隔）
void MainWindow::onExportTemperatureHistory(int carId, int intervalSeconds) {
    // qDebug() << "MainWindow::onExportTemperatureHistory called - carId:" << carId << "interval:" << intervalSeconds;
    
    // 检查是否有该车厢的历史数据
    if (!tempHistory.contains(carId) || tempHistory[carId].isEmpty()) {
        QMessageBox::warning(this, tr("警告"), 
                           tr("车厢 %1 没有历史温度数据").arg(carId));
        return;
    }
    
    // 选择保存文件路径
    QString fileName = QFileDialog::getSaveFileName(
        this, 
        tr("导出车厢 %1 温度历史数据").arg(carId),
        QString("car_%1_temperature_history.csv").arg(carId),
        tr("CSV文件 (*.csv)")
    );
    
    if (fileName.isEmpty()) return;
    
    // 创建文件并写入数据
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, tr("错误"), tr("无法创建文件"));
        return;
    }
    
    // 使用系统编码写入
    QTextStream stream(&file);
    stream.setEncoding(QStringConverter::System);
    
    // 写入CSV头部
    stream << tr("时间索引,温度(℃),时间戳") << "\n";
    
    // 获取原始数据
    const QVector<QPoint> &originalData = tempHistory[carId];
    
    // 进行数据抽样：每intervalSeconds秒取一个点
    // 假设每秒收到一次数据，所以每intervalSeconds个点取一个
    QDateTime startTime = QDateTime::currentDateTime().addSecs(-originalData.size());
    
    int exportedPoints = 0;
    for (int i = 0; i < originalData.size(); i += intervalSeconds) {
        const QPoint &point = originalData[i];
        
        // 计算对应的时间戳
        QDateTime pointTime = startTime.addSecs(point.x());
        QString timeString = pointTime.toString("yyyy-MM-dd hh:mm:ss");
        
        // 写入数据行：索引,温度,时间戳
        stream << QString::number(point.x()) << ","
               << QString::number(point.y()) << ","
               << timeString << "\n";
        exportedPoints++;
    }
    
    file.close();
    
    // 显示成功消息
    QMessageBox::information(this, tr("导出成功"), 
                           tr("已导出车厢 %1 的温度历史数据\n"
                              "原始数据点: %2\n"
                              "导出数据点: %3\n"
                              "抽样间隔: %4 秒\n"
                              "文件路径: %5")
                              .arg(carId)
                              .arg(originalData.size())
                              .arg(exportedPoints)
                              .arg(intervalSeconds)
                              .arg(fileName));
}
