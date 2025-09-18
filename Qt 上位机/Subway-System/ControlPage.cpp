#include "ControlPage.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QGroupBox>
#include <QFormLayout>
#include <QSpinBox>
#include <QCheckBox>

// 构造函数：创建控制页面的 UI 元素并连接信号
ControlPage::ControlPage(QWidget *parent) : QWidget(parent) {
    QVBoxLayout *controlLayout = new QVBoxLayout(this);

    // 车厢选择下拉：0 表示广播
    QLabel *carLabel = new QLabel(tr("选择车厢:"));
    carSelect = new QComboBox();
    carSelect->addItem(tr("广播 (所有车厢)"), 0);
    carSelect->addItem(tr("车厢 1"), 1);
    carSelect->addItem(tr("车厢 2"), 2);
    carSelect->addItem(tr("车厢 3"), 3);
    controlLayout->setContentsMargins(0, 0, 0, 0); // 减少上方空白
    controlLayout->addWidget(carLabel,0);
    controlLayout->addWidget(carSelect,0);

    // 控制项分组：速度、阈值、门、报警、发送按钮
    QGroupBox *controlBox = new QGroupBox(tr("下发控制指令"));
    QFormLayout *controlForm = new QFormLayout(controlBox);
    // 将控件保存为成员变量，便于外部通过 setValues 更新
    setSpeedSpin = new QSpinBox(controlBox);
    setSpeedSpin->setRange(0, 300);
    // 温度输入已移除（设备上报），但保留阈值输入
    setTempThresholdSpin = new QSpinBox(controlBox);
    setTempThresholdSpin->setRange(-50, 200);
    setDoorCheck = new QCheckBox(tr("门打开"), controlBox);
    setAlarmCheck = new QCheckBox(tr("触发报警"), controlBox);
    QPushButton *sendControlBtn = new QPushButton(tr("发送控制指令"), controlBox);

    controlForm->addRow(tr("设置速度 (km/h):"), setSpeedSpin);
    controlForm->addRow(tr("温度报警阈值 (℃):"), setTempThresholdSpin);
    // 将 check 放到表单中以便于布局管理，但值通过 setValues 可被程序设置
    controlForm->addRow(setDoorCheck);
    controlForm->addRow(setAlarmCheck);
    controlForm->addRow(sendControlBtn);
    controlLayout->addWidget(controlBox,0);
    controlLayout->setSpacing(0);

    // 当用户切换车厢时发出 carSelected 信号，供 MainWindow 触发自动填充
    connect(carSelect, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int idx){
        int carId = carSelect->currentData().toInt();
        emit carSelected(carId);
    });

    // 点击发送按钮时构建 sendControl 信号参数并发出
    connect(sendControlBtn, &QPushButton::clicked, this, [this]() {
        int carId = carSelect->currentData().toInt();
        int speedVal = setSpeedSpin->value();
        int tempVal = 0; // UI 中不再提供温度，使用占位值（协议保留字段）
        int tempThresholdVal = setTempThresholdSpin->value();
        int doorVal = setDoorCheck->isChecked() ? 1 : 0;
        int alarmVal = setAlarmCheck->isChecked() ? 1 : 0;
        int etaMin = -1; int etaSec = -1; // 未设置 ETA
        int mode = 0; // 默认运行模式
        emit sendControl(carId, tempVal, tempThresholdVal, speedVal, etaMin, etaSec, mode, doorVal, alarmVal);
    });
}

// 返回当前选择的车厢 ID（0 表示广播）
int ControlPage::currentCarId() const {
    return carSelect->currentData().toInt();
}

// setValues: 将 MainWindow 缓存的最近值回写到 UI，以便用户在下发控制前能看到当前系统数据
// 注意：temp 字段在 UI 中不显示，但保留为参数以兼容协议/历史调用
void ControlPage::setValues(int temp, int tempThreshold, int speed, int mode, int door, int alarm) {
    // setTempSpin 已移除，因此不再设置温度
    setTempThresholdSpin->setValue(tempThreshold);
    setSpeedSpin->setValue(speed);
    // mode 字段接收但不显示到 UI（若需要可扩展为下拉框）
    Q_UNUSED(mode);
    setDoorCheck->setChecked(door == 1);
    setAlarmCheck->setChecked(alarm == 1);
}
