#ifndef DIAGNOSISPAGE_H
#define DIAGNOSISPAGE_H

#include <QWidget>
#include <QComboBox>
#include <QSpinBox>

class ChartWidget;

class DiagnosisPage : public QWidget {
    Q_OBJECT
public:
    explicit DiagnosisPage(ChartWidget *chart, QWidget *parent = nullptr);
    void updateTemperatureChart(int carId, const QVector<QPoint> &dataPoints); // 新增方法

signals:
    // 新增：导出温度历史数据的信号
    void exportTemperatureHistory(int carId, int intervalSeconds);

private slots:
    // 新增：处理导出按钮点击
    void onExportTemperatureData();

private:
    ChartWidget *chartWidget;
    QComboBox *carSelectCombo;    // 车厢选择下拉框
    QSpinBox *intervalSpin;       // 抽样间隔选择框
};

#endif // DIAGNOSISPAGE_H
