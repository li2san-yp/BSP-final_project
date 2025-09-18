#ifndef CONTROLPAGE_H
#define CONTROLPAGE_H

#include <QWidget>
#include <QComboBox>
class QSpinBox;
class QCheckBox;

// ControlPage: 远程控制页，允许操作人员向下位机下发控制帧
// - 首次出现的 Qt 类型说明：
//   - `QComboBox`：下拉选择控件，此处用于选择目标车厢（0 表示广播）
//   - `QSpinBox`：整数微调框，适合设置速度或阈值等整型数值
//   - `QCheckBox`：复选框，用于设置门/报警开关位
// 设计要点：UI 主要用于设置阈值和速度；温度输入已移除（由设备上报温度），但保持 sendControl 协议字段占位。

class ControlPage : public QWidget {
    Q_OBJECT
public:
    explicit ControlPage(QWidget *parent = nullptr);
    // 返回当前选择的车厢 ID（0 表示广播）
    int currentCarId() const;
public slots:
    // setValues 用于将外部（MainWindow）缓存的最近值写回到 UI 控件中以便自动填充
    // 参数：temp/tempThreshold/speed 为整数，mode 为协议中的模式字段（0/1），door/alarm 为 0/1 标志
    void setValues(int temp, int tempThreshold, int speed, int mode, int door, int alarm);
signals:
    // 当选择车厢变化时发出（carId）用于触发自动填充
    void carSelected(int carId);

    // sendControl: 当用户点击发送按钮时发出，将要下发的协议参数全部传给上层 MainWindow
    // 参数含义：
    // - carId: 目标车厢（0 表示广播）
    // - temp: 温度占位（UI 中移除，但协议保留字段，通常为 0）
    // - tempThreshold: 温度报警阈值（℃，整数）
    // - speed: 目标速度（km/h，整数）
    // - etaMin/etaSec: ETA 或停站时间的分钟与秒（可为 -1 表示不设置）
    // - mode: 运行模式（0=RUNNING_MODE, 1=STOP_MODE）
    // - door: 车门位（0/1）
    // - alarm: 报警位（0/1）
    void sendControl(int carId, int temp, int tempThreshold, int speed, int etaMin, int etaSec, int mode, int door, int alarm);
private:
    QComboBox *carSelect;
    // 控件成员以便外部通过 setValues 修改
    QSpinBox *setSpeedSpin;
    // QSpinBox *setTempSpin; // 温度输入已移除
    QSpinBox *setTempThresholdSpin;
    QCheckBox *setDoorCheck;
    QCheckBox *setAlarmCheck;
};

#endif // CONTROLPAGE_H
