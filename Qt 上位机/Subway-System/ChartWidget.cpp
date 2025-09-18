#include "ChartWidget.h"
#include <QVBoxLayout>
#include <QDebug>
#include <QtCharts/QValueAxis>
#include <QMap>
#include <algorithm>
#include <QEvent>
#include <QMouseEvent>
#include <QToolTip>

ChartWidget::ChartWidget(QWidget *parent) : QWidget(parent) {
    chart = new QChart();
    chart->legend()->show(); // 显示图例以区分不同车厢
    chart->setTitle(tr("温度历史曲线"));
    
    chartView = new QChartView(chart, this);
    chartView->setRenderHint(QPainter::Antialiasing); // 抗锯齿
    
    // 设置工具提示格式，确保显示整数
    chartView->setToolTip("");
    chartView->installEventFilter(this); // 安装事件过滤器来自定义工具提示
    
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(chartView);
    
    // qDebug() << "ChartWidget constructor finished - chart:" << (chart != nullptr) 
    //          << "chartView:" << (chartView != nullptr);
}

void ChartWidget::plotTemperatureHistory(int carId, const QVector<QPoint> &dataPoints) {
    // 入口日志（已注释，可在开发时打开以便调试）
    // qDebug() << "ChartWidget::plotTemperatureHistory called for carId" << carId << "with" << dataPoints.size() << "points";

    // 获取或创建该车厢对应的系列（QLineSeries）
    QLineSeries *series = nullptr;
    if (!carSeries.contains(carId)) {
        series = new QLineSeries(this);
        series->setName(tr("车厢 %1").arg(carId));
        // Point label 格式设置（仅供调试），常态下不显示点标签
        series->setPointLabelsFormat("(@xPoint, @yPoint)");
        series->setPointLabelsVisible(false);
        carSeries[carId] = series;
        chart->addSeries(series);
    } else {
        series = carSeries[carId];
    }

    // 清空 series 并填充新数据（将整数 QPoint 转换为浮点加入 QLineSeries）
    series->clear();
    for(const QPoint &p : dataPoints) {
        // 注意：QLineSeries 使用浮点坐标，但我们传入的是整数值；在显示与 tooltip 中会被四舍五入为整数
        series->append(p.x(), p.y());
    }

    // 创建默认坐标轴并设置为整数格式显示（防止出现小数）
    chart->createDefaultAxes();
    auto axes = chart->axes();
    for(auto axis : axes) {
        QValueAxis *valueAxis = qobject_cast<QValueAxis*>(axis);
        if(valueAxis) {
            // 使用 "%.0f" 强制轴刻度为整数显示
            valueAxis->setLabelFormat("%.0f");
        }
    }

    if (!dataPoints.isEmpty()) {
        // 先根据当前 series 的数据计算 min/max
        int minX = dataPoints.first().x();
        int maxX = dataPoints.first().x();
        int minY = dataPoints.first().y();
        int maxY = dataPoints.first().y();
        for(const QPoint &p : dataPoints) {
            minX = qMin(minX, p.x());
            maxX = qMax(maxX, p.x());
            minY = qMin(minY, p.y());
            maxY = qMax(maxY, p.y());
        }

        // 扩展范围以包含其他已存在系列的数据点
        for(auto it = carSeries.begin(); it != carSeries.end(); ++it) {
            QLineSeries *s = it.value();
            auto points = s->points();
            for(const QPointF &p : points) {
                minX = qMin(minX, static_cast<int>(p.x()));
                maxX = qMax(maxX, static_cast<int>(p.x()));
                minY = qMin(minY, static_cast<int>(p.y()));
                maxY = qMax(maxY, static_cast<int>(p.y()));
            }
        }

        // 根据计算出的范围设置坐标轴
        for(auto axis : axes) {
            if(axis->orientation() == Qt::Horizontal) {
                axis->setRange(qMax(0, minX - 1), maxX + 1);
            } else if(axis->orientation() == Qt::Vertical) {
                int yMargin = qMax(1, (maxY - minY) / 10);
                axis->setRange(minY - yMargin, maxY + yMargin);
            }
        }
    }

    // 刷新图表
    chartView->update();
    chart->update();
}

bool ChartWidget::eventFilter(QObject *object, QEvent *event) {
    if (object == chartView && event->type() == QEvent::MouseMove) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        QPointF chartPoint = chart->mapToValue(mouseEvent->pos());
        // 将坐标格式化为整数（四舍五入），用于显示为 tooltip
        QString toolTip = QString("(%1, %2)")
                          .arg(qRound(chartPoint.x()))
                          .arg(qRound(chartPoint.y()));

        // 检查是否有数据点接近鼠标位置：遍历所有系列并寻找与鼠标位置差值小于阈值的点
        // 说明：由于 series 存储为浮点 QPointF，此处使用容差 1.0 个单位来判定 "接近"
        bool nearPoint = false;
        for(auto it = carSeries.begin(); it != carSeries.end(); ++it) {
            QLineSeries *series = it.value();
            auto points = series->points();
            for(const QPointF &p : points) {
                if(qAbs(p.x() - chartPoint.x()) < 1.0 && qAbs(p.y() - chartPoint.y()) < 1.0) {
                    nearPoint = true;
                    // 当靠近某个点时，显示该系列名称与整数坐标
                    toolTip = QString("%1: (%2, %3)")
                              .arg(series->name())
                              .arg(qRound(p.x()))
                              .arg(qRound(p.y()));
                    break;
                }
            }
            if(nearPoint) break;
        }

        if(nearPoint) {
            QToolTip::showText(mouseEvent->globalPos(), toolTip);
        } else {
            QToolTip::hideText();
        }
    }
    return QWidget::eventFilter(object, event);
}
