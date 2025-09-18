// MainWindow.h
// 本文件定义 MainWindow 类，作为应用程序的主窗口，负责：
// - 创建和组织各功能选项卡（实时监控、报警日志、远程控制、维护诊断）
// - 连接串口管理模块（SerialManager）与各页面，分发实时数据
// - 缓存各车厢的实时数据（用于自动填充控制界面、历史绘图和报警判定）
//
// 备注：
// - QMainWindow: Qt 的主窗口类，提供菜单栏、工具栏、中心窗口区等常用主窗口功能
// - QMap/QVector/QDateTime/QTimer: 常用容器与时间/定时器工具

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "SerialManager.h"
#include "AlarmLog.h"
// ChartWidget 已移除，由 DiagnosisPage 管理
#include <QTableWidget>
#include <QMap>
#include <QVector>
#include <QDateTime>


class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    // 构造与析构：负责创建各页面、建立信号槽连接
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    // 串口管理器对象，负责底层串口收发
    SerialManager *serialManager;
    // AlarmLog：封装报警日志的类，负责界面表格与文件持久化
    AlarmLog *alarmLog;
    // chartWidget 已移除，由 DiagnosisPage 自己管理
    // UI 元素
    QTabWidget *tabs; // QTabWidget：用于管理多个选项卡页面
    // per-tab widgets (now encapsulated)
    QWidget *statusPage, *alarmPage, *controlPage, *diagnosisPage;
    class StatusPage *statusWidget;
    class AlarmPage *alarmWidget;
    class ControlPage *controlWidget;
    class DiagnosisPage *diagnosisWidget;
    // 状态页控件
    QTableWidget *statusTable;
    // 远程控制页控件（部分）
    // 诊断页控件（部分）

    // 业务数据：每个车厢的温度阈值
    QMap<int, int> tempThresholds; // carId -> threshold (整数 °C)
    // 温度历史：每个车厢保留时间序列点 (x=time index, y=temp) - 使用整数坐标
    QMap<int, QVector<QPoint>> tempHistory;
    // 每车的最近一次实时值缓存，供 ControlPage 自动填充使用
    QMap<int, int> lastTemp;
    QMap<int, int> lastTempThreshold;
    QMap<int, int> lastSpeed;
    QMap<int, int> lastMode;  // 运行模式缓存: 0=RUNNING_MODE, 1=STOP_MODE
    QMap<int, int> lastDoor;
    QMap<int, int> lastAlarm;
    
    // 每车厢的最后报警时间，用于实现报警冷却（1分钟内不重复报警）
    QMap<int, QDateTime> lastAlarmTime;

    // 启动延迟定时器，防止程序刚启动时的数据误报
    QTimer *startupTimer; // QTimer：Qt 的定时器类，用于延时或周期性任务
    // 控制页面自动填充定时器，控制填充频率为10秒一次
    QTimer *autoFillTimer;

    // 闪烁控制（简单实现：记录需要闪烁的行索引和计时器）
    // ... (实现细节在 cpp 中)
private slots:
    // onDataReceived：当 SerialManager 解析出一条完整数据帧时触发，
    // 负责更新缓存、判断报警、刷新界面和触发日志
    void onDataReceived(int carId, int temp, int tempThreshold, int speed, int etaMinutes, int etaSeconds, int mode, bool doorOk, bool alarmOn, int hour, int minute, int second);
    // onAlarmTriggered：专门处理由 SerialManager 上报的报警事件并记录
    void onAlarmTriggered(int carId, const QString &type, const QString &message);
    // control actions
    void onSetThreshold(int carId, int threshold);
    // 接收完整控制参数的槽，直接由 ControlPage::sendControl 发射
    void onSendControl(int carId, int temp, int tempThreshold, int speed, int etaMin, int etaSec, int mode, int door, int alarm);
    void onExportAlarms();
    // 新增：导出温度历史数据的槽函数
    void onExportTemperatureHistory(int carId, int intervalSeconds);
    // helper to push cached values to control widget
    void pushCachedToControl(int carId);
};
#endif // MAINWINDOW_H
