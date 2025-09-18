#include "StatusPage.h"
#include <QVBoxLayout>
#include <QHeaderView>
#include <QTableWidgetItem>

// 页面构造函数：创建 QTableWidget 并设置列头与基本行为
StatusPage::StatusPage(QWidget *parent) : QWidget(parent) {
    // 布局：垂直布局中只有一个表格
    QVBoxLayout *layout = new QVBoxLayout(this);

    // 创建一个 0 行 8 列的表格，列顺序与含义如下：
    // 0: 车厢号
    // 1: 温度(℃)
    // 2: 报警阈值(℃)
    // 3: 车速(km/h)
    // 4: 到站(分:秒)  (当 mode==0 时显示 ETA)
    // 5: 停站时间       (当 mode==1 时显示停站时间)
    // 6: 车门状态
    // 7: 报警状态
    statusTable = new QTableWidget(0, 8, this);
    QStringList headers = {"车厢号", "温度(℃)", "报警阈值(℃)", "车速(km/h)", "到站(分:秒)", "停站时间", "车门状态", "报警状态"};
    statusTable->setHorizontalHeaderLabels(headers);

    // 表格仅作展示，禁止编辑；隐藏行号，整行选择
    statusTable->verticalHeader()->setVisible(false);
    statusTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    statusTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    statusTable->setSelectionMode(QAbstractItemView::SingleSelection);
        // 允许用户交互式调整列宽，并设置一些更宽的默认列宽，提升可读性
    statusTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    statusTable->horizontalHeader()->setDefaultSectionSize(110); // 默认宽度
    // 对关键列做细粒度宽度设置，便于重要字段显示
    statusTable->setColumnWidth(0, 80);   // 车厢号
    statusTable->setColumnWidth(1, 100);  // 温度
    statusTable->setColumnWidth(2, 120);  // 报警阈值
    statusTable->setColumnWidth(3, 120);  // 车速
    statusTable->setColumnWidth(4, 120);  // 到站
    statusTable->setColumnWidth(5, 120);  // 停站时间
    statusTable->setColumnWidth(6, 100);  // 车门状态
    statusTable->setColumnWidth(7, 100);  // 报警状态
        // 增大表格行高（默认行高），便于视觉识别和点击
        statusTable->verticalHeader()->setDefaultSectionSize(32);
        layout->addWidget(statusTable);
}

// initCars: 根据提供的车厢编号创建表格行并初始化显示占位符
// 说明：表格初始化处将 "到站" 默认显示为 "--:--"，而 "停站时间" 使用 "---" 表示空值
void StatusPage::initCars(const QList<int> &carIds) {
    for(int id : carIds) {
        int row = statusTable->rowCount();
        statusTable->insertRow(row);
            // 初始插入时为每行设置较大的高度
            statusTable->setRowHeight(row, 50);
        statusTable->setItem(row, 0, new QTableWidgetItem(QString::number(id)));
        statusTable->setItem(row, 1, new QTableWidgetItem("--"));
        statusTable->setItem(row, 2, new QTableWidgetItem("--"));
        statusTable->setItem(row, 3, new QTableWidgetItem("--"));
        statusTable->setItem(row, 4, new QTableWidgetItem("--:--"));
        statusTable->setItem(row, 5, new QTableWidgetItem("---"));  // 停站时间初始化为占位符
        statusTable->setItem(row, 6, new QTableWidgetItem(tr("未知")));
        statusTable->setItem(row, 7, new QTableWidgetItem(tr("正常")));
    }
}

// updateStatus: 根据接收的数据更新对应车厢行的显示
// 关键点：
// - `mode` 控制 "到站" 与 "停站时间" 的展示位置：
//     - mode==0 (RUNNING_MODE): 在第4列显示 ETA(分:秒)，第5列显示 "---"
//     - mode==1 (STOP_MODE)   : 第4列显示 "--:--"，第5列显示停站时间(分:秒)
// - `triggered` 为 true 时将整行标为红色背景并在第7列显示 "报警"
// - 函数会遍历表格，匹配第一列的车厢号以定位要更新的行
void StatusPage::updateStatus(int carId, int temp, int tempThreshold, int speed, int etaMin, int etaSec, int mode, bool doorOpen, bool triggered) {
    // 尝试查找已有行
    int foundRow = -1;
    for(int r = 0; r < statusTable->rowCount(); ++r) {
        QTableWidgetItem *idItem = statusTable->item(r, 0);
        if(!idItem) continue;
        if(idItem->text().toInt() == carId) { foundRow = r; break; }
    }

    // 如果没有找到对应车厢，则动态插入一行并初始化单元格（支持动态学习到的新车厢）
    if(foundRow == -1) {
        foundRow = statusTable->rowCount();
        statusTable->insertRow(foundRow);
        statusTable->setRowHeight(foundRow, 50);
        // 创建并初始化各列（与 initCars 保持一致的占位符）
        statusTable->setItem(foundRow, 0, new QTableWidgetItem(QString::number(carId)));
        statusTable->setItem(foundRow, 1, new QTableWidgetItem("--"));
        statusTable->setItem(foundRow, 2, new QTableWidgetItem("--"));
        statusTable->setItem(foundRow, 3, new QTableWidgetItem("--"));
        statusTable->setItem(foundRow, 4, new QTableWidgetItem("--:--"));
        statusTable->setItem(foundRow, 5, new QTableWidgetItem("---"));
        statusTable->setItem(foundRow, 6, new QTableWidgetItem(tr("未知")));
        statusTable->setItem(foundRow, 7, new QTableWidgetItem(tr("正常")));
    }

    int r = foundRow;
    // 更新基本数值列（温度、阈值、速度）
    if(!statusTable->item(r,1)) statusTable->setItem(r,1, new QTableWidgetItem("--"));
    if(!statusTable->item(r,2)) statusTable->setItem(r,2, new QTableWidgetItem("--"));
    if(!statusTable->item(r,3)) statusTable->setItem(r,3, new QTableWidgetItem("--"));
    statusTable->item(r,1)->setText(QString::number(temp));
    statusTable->item(r,2)->setText(QString::number(tempThreshold));
    statusTable->item(r,3)->setText(QString::number(speed));

    // 根据 mode 值选择显示 ETA 或 停站时间
    if(mode == 0) {
        // RUNNING_MODE: 显示 ETA，停站时间为占位符
        statusTable->item(r,4)->setText(QString("%1:%2").arg(etaMin).arg(etaSec, 2, 10, QChar('0')));
        statusTable->item(r,5)->setText("---");
    } else {
        // STOP_MODE: ETA 为 --:--，将传来的分钟/秒作为停站时间显示
        statusTable->item(r,4)->setText("--:--");
        statusTable->item(r,5)->setText(QString("%1:%2").arg(etaMin).arg(etaSec, 2, 10, QChar('0')));
    }

    // 车门状态与报警状态（报警会同时改变单元格背景色以示醒目）
    if(!statusTable->item(r,6)) statusTable->setItem(r,6, new QTableWidgetItem(tr("未知")));
    if(!statusTable->item(r,7)) statusTable->setItem(r,7, new QTableWidgetItem(tr("正常")));
    statusTable->item(r,6)->setText(doorOpen ? tr("打开") : tr("关闭"));
    statusTable->item(r,7)->setText(triggered ? tr("报警") : tr("正常"));
    // 背景色切换：报警 -> 红色背景；正常 -> 白色背景
    for(int c = 0; c < statusTable->columnCount(); ++c) {
        statusTable->item(r,c)->setBackground(triggered ? Qt::red : Qt::white);
    }
}
