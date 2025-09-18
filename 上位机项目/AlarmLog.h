#ifndef ALARMLOG_H
#define ALARMLOG_H

#include <QWidget>
#include <QTableWidget>

// AlarmLog: 管理报警记录的 UI 与持久化
// - 在界面上以 QTableWidget 展示报警条目
// - 每次 addEntry 会追加到内置的 CSV 文件 `alarms.csv`（Append 模式），同时添加到表格并滚动到底部
// - exportToCsv 将当前表格中的内容导出到用户指定文件，并在导出时对包含逗号或引号的字段做 CSV 转义
class AlarmLog : public QWidget {
    Q_OBJECT
public:
    AlarmLog(QWidget *parent = nullptr);

    // addEntry: 添加一条报警记录，并追加写入本地 alarms.csv 文件
    // CSV 字段顺序为: 时间,车厢号,报警类型,内容 （时间使用 ISODate 格式）
    void addEntry(int carId, const QString &type, const QString &content);

    // exportToCsv: 将当前表格的所有行写入到指定文件名
    // 注意: 导出时使用系统默认编码（QStringConverter::System）以兼容本地环境
    void exportToCsv(const QString &filename);

    // 暴露内部表格以便外部嵌入或查询（只读指针）
    QTableWidget* table() { return alarmTable; }

private:
    QTableWidget *alarmTable;
};
#endif // ALARMLOG_H
