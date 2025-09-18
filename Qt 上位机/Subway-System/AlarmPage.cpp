#include "AlarmPage.h"
#include "AlarmLog.h"
#include <QVBoxLayout>
#include <QPushButton>
#include <QtWidgets/QFileDialog>

// AlarmPage: 包装 AlarmLog 的展示视图，并提供导出按钮
// 说明：AlarmPage 不直接管理日志数据的写入，只负责界面展示与导出触发
AlarmPage::AlarmPage(AlarmLog *log, QWidget *parent) : QWidget(parent), alarmLog(log) {
    QVBoxLayout *layout = new QVBoxLayout(this);
    // 将 AlarmLog 的表格嵌入到页面中
    if(alarmLog) layout->addWidget(alarmLog->table());

    // 导出按钮：调用 AlarmLog::exportToCsv
    QPushButton *exportBtn = new QPushButton(tr("导出为 CSV"), this);
    layout->addWidget(exportBtn);
    connect(exportBtn, &QPushButton::clicked, this, [this]() {
        QString fileName = QFileDialog::getSaveFileName(this, tr("导出报警 CSV"), "alarms_export.csv", tr("CSV Files (*.csv);;All Files (*)"));
        if(fileName.isEmpty()) return;
        alarmLog->exportToCsv(fileName);
    });
}
