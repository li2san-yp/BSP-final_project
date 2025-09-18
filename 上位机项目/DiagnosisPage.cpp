#include "DiagnosisPage.h"
#include "ChartWidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QComboBox>
#include <QSpinBox>
#include <QLabel>
#include <QDebug>

// DiagnosisPage: 维护诊断页面，负责展示 ChartWidget（温度历史曲线）并触发导出操作
// 设计要点：
// - ChartWidget 由外部传入或由本页自行创建；如果由本页创建，则以本页为其父对象，负责其生命周期管理
// - 温度历史点类型：`QPoint`，约定 x 表示时间索引（整数秒或样本序号），y 表示温度（整数，单位：℃）
// - 导出请求通过信号发出，由 MainWindow 负责实际的数据写入（MainWindow 持有完整缓存）
DiagnosisPage::DiagnosisPage(ChartWidget *chart, QWidget *parent) : QWidget(parent), chartWidget(chart) {
    // 如果没有传入 ChartWidget，则自行创建（并以本对象为 parent）
    if (!chartWidget) {
        chartWidget = new ChartWidget(this);
    }

    QVBoxLayout *layout = new QVBoxLayout(this);
    if(chartWidget) {
        layout->addWidget(chartWidget);
    }

    // 导出/刷新 控制区域：选择车厢、抽样间隔、刷新与导出按钮
    QHBoxLayout *exportLayout = new QHBoxLayout();
    QLabel *carLabel = new QLabel(tr("选择车厢:"), this);
    carSelectCombo = new QComboBox(this);
    carSelectCombo->addItem(tr("车厢 1"), 1);
    carSelectCombo->addItem(tr("车厢 2"), 2);
    carSelectCombo->addItem(tr("车厢 3"), 3);

    // 抽样间隔（以秒为单位）— 假定历史缓存按秒采样
    QLabel *intervalLabel = new QLabel(tr("数据间隔:"), this);
    intervalSpin = new QSpinBox(this);
    intervalSpin->setRange(1, 60);  // 允许 1-60 秒
    intervalSpin->setValue(10);     // 默认 10 秒
    intervalSpin->setSuffix(tr(" 秒"));

    QPushButton *refreshChartBtn = new QPushButton(tr("刷新曲线"), this);
    QPushButton *exportTempBtn = new QPushButton(tr("导出温度历史数据"), this);

    exportLayout->addWidget(carLabel);
    exportLayout->addWidget(carSelectCombo);
    exportLayout->addWidget(intervalLabel);
    exportLayout->addWidget(intervalSpin);
    exportLayout->addWidget(refreshChartBtn);
    exportLayout->addWidget(exportTempBtn);
    exportLayout->addStretch();
    layout->addLayout(exportLayout);

    // 点击刷新时用测试数据刷新（用于开发/调试）；正式运行时由 MainWindow 调用 updateTemperatureChart
    connect(refreshChartBtn, &QPushButton::clicked, this, [this]() {
        QVector<QPoint> points;
        for(int i = 0; i < 100; ++i) points.append(QPoint(i, 20 + (i % 10)));
        if(chartWidget) chartWidget->plotTemperatureHistory(1, points);
    });

    // 导出按钮：发射信号请 MainWindow 导出对应车厢的数据（MainWindow 持有缓存并负责写入）
    connect(exportTempBtn, &QPushButton::clicked, this, &DiagnosisPage::onExportTemperatureData);
}

// updateTemperatureChart: 将外部传入的温度历史点绘制到 ChartWidget
// - 参数数据点含义：QPoint.x = 时间索引（整数），QPoint.y = 温度（整数，℃）
// - 如果 chartWidget 为空，则仅记录警告（不崩溃）
void DiagnosisPage::updateTemperatureChart(int carId, const QVector<QPoint> &dataPoints) {
    if(chartWidget) {
        chartWidget->plotTemperatureHistory(carId, dataPoints);
    }
}

// onExportTemperatureData: 将导出请求发送给 MainWindow，MainWindow 会根据缓存和抽样间隔写入 CSV
void DiagnosisPage::onExportTemperatureData() {
    // 选中的车厢 ID
    int selectedCarId = carSelectCombo->currentData().toInt();
    // 抽样间隔（秒）
    int intervalSeconds = intervalSpin->value();
    // 发射导出信号，由 MainWindow 处理实际的文件写入
    emit exportTemperatureHistory(selectedCarId, intervalSeconds);
}
