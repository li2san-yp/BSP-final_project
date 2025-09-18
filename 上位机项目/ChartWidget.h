#ifndef CHARTWIDGET_H
#define CHARTWIDGET_H

#include <QWidget>
#include <QtCharts/QChartView>
#include <QtCharts/QChart>
#include <QtCharts/QLineSeries>
#include <QMap>

// ChartWidget: 使用 Qt Charts 显示多车厢的温度历史曲线
// 设计说明：
// - 使用 `QLineSeries` 为每个车厢绘制一条线，保存在 `carSeries` 映射中
// - 使用 `QVector<QPoint>` 作为输入数据：约定 `QPoint.x()` 表示时间索引（或样本序号），`QPoint.y()` 表示温度（整数，单位 ℃）
// - 轴标签格式强制为整数（使用 QValueAxis::setLabelFormat("%.0f")）以避免显示小数点
// - 通过安装事件过滤器在鼠标移动时显示整数坐标或某个系列附近的整数点工具提示

class ChartWidget : public QWidget {
    Q_OBJECT
public:
    explicit ChartWidget(QWidget *parent = nullptr);

    // plotTemperatureHistory: 绘制指定车厢的温度历史
    // - carId: 车厢编号，用于在图例中区分
    // - dataPoints: 每个点为 QPoint，x=时间索引(整数)，y=温度(整数℃)
    void plotTemperatureHistory(int carId, const QVector<QPoint> &dataPoints);

protected:
    // 重载 eventFilter，用于在图表视图上捕获鼠标移动事件并显示自定义 tooltip
    bool eventFilter(QObject *object, QEvent *event) override;

private:
    QChartView *chartView;
    QChart *chart;
    // carSeries: map from carId -> QLineSeries*，每个 series 的点使用 QPoint 转换为 QPointF 添加
    QMap<int, QLineSeries*> carSeries; // 每个车厢一个系列
};

#endif // CHARTWIDGET_H
