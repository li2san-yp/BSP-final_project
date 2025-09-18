#include "AlarmLog.h"
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QHeaderView>
#include <QStringConverter>

AlarmLog::AlarmLog(QWidget *parent) : QWidget(parent) {
    // 创建 4 列表格，列顺序: 时间 | 车厢号 | 报警类型 | 内容
    alarmTable = new QTableWidget(0, 4, this);
    QStringList headers = {"时间", "车厢号", "报警类型", "内容"};
    alarmTable->setHorizontalHeaderLabels(headers);
    // 允许交互式调整列宽，设置默认列宽，并让最后一列（内容）伸展以填充剩余空间
    alarmTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    alarmTable->horizontalHeader()->setDefaultSectionSize(140);
    alarmTable->setColumnWidth(0, 160); // 时间
    alarmTable->setColumnWidth(1, 80);  // 车厢号
    alarmTable->setColumnWidth(2, 120); // 报警类型
    alarmTable->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch); // 内容列伸展
}

// addEntry: 将报警事件添加到表格并追加写入本地 CSV 日志文件
// CSV 行格式: ISO时间,车厢号,报警类型,内容
// 说明: 使用 append 模式打开本地文件 `alarms.csv`，每次写一行，使用系统默认编码
void AlarmLog::addEntry(int carId, const QString &type, const QString &content) {
    int row = alarmTable->rowCount();
    alarmTable->insertRow(row);
    alarmTable->setItem(row, 0, new QTableWidgetItem(QDateTime::currentDateTime().toString()));
    alarmTable->setItem(row, 1, new QTableWidgetItem(QString::number(carId)));
    alarmTable->setItem(row, 2, new QTableWidgetItem(type));
    alarmTable->setItem(row, 3, new QTableWidgetItem(content));
    alarmTable->scrollToBottom();

    // 同步写入本地 CSV（追加模式），方便持久化与离线查看
    QFile file("alarms.csv");
    if(file.open(QIODevice::Append | QIODevice::Text)) {
        QTextStream out(&file);
        out.setEncoding(QStringConverter::System); // 使用系统默认编码
        out << QDateTime::currentDateTime().toString(Qt::ISODate) << ",";
        out << carId << ",";
        out << type << ",";
        out << content << "\n";
        file.close();
    }
}

// exportToCsv: 将当前表格内容导出到指定文件名
// 注意: 导出时会对包含逗号或引号的字段做 CSV 转义（使用双引号并对内部引号做重复转义）
void AlarmLog::exportToCsv(const QString &filename) {
    QFile file(filename);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text)) return;
    QTextStream out(&file);
    out.setEncoding(QStringConverter::System);
    
    // 写表头（固定顺序）
    out << "时间,车厢号,报警类型,内容\n";
    // 遍历表格并写入每一行
    for(int r = 0; r < alarmTable->rowCount(); ++r) {
        QStringList rowData;
        for(int c = 0; c < alarmTable->columnCount(); ++c) {
            QString cellText = alarmTable->item(r,c)->text();
            // 如果单元格文本包含逗号或双引号，则用双引号包裹并将内部双引号替换为两个双引号
            if(cellText.contains(",") || cellText.contains("\"")) {
                cellText = "\"" + cellText.replace("\"", "\"\"") + "\"";
            }
            rowData << cellText;
        }
        out << rowData.join(",") << "\n";
    }
    file.close();
}
