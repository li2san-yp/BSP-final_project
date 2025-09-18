#ifndef STATUSPAGE_H
#define STATUSPAGE_H

#include <QWidget>
#include <QTableWidget>
#include <QMap>

// StatusPage: 实时监控页，用一个 QTableWidget 展示每个车厢的运行状态
// - 首次出现的 Qt 类型说明：
//   - `QWidget`：Qt 的基础窗口部件类，所有可视控件的基类；StatusPage 继承自 QWidget 以便被放入 QTabWidget。
//   - `QTableWidget`：一个基于 item 的表格控件，用于方便地显示和编辑表格数据。此处用于只读显示。
// 主要职责：
// - 初始化表格列（车厢号、温度、阈值、车速、到站、停站时间、车门、报警状态）
// - 由上层（MainWindow）调用 `updateStatus` 更新某一车厢行的显示
// - 根据 `mode` 的值决定显示 "到站(分:秒)" 还是 "停站时间"

class StatusPage : public QWidget {
    Q_OBJECT
public:
    explicit StatusPage(QWidget *parent = nullptr);

    // 初始化显示指定车厢（会在表格中按顺序创建行）
    // 参数：
    // - carIds: 要初始化的车厢编号列表（例如 {1,2,3}）
    void initCars(const QList<int> &carIds);

    // 更新单行显示；triggered 表示该条目是否处于报警状态（由 MainWindow 判定）
    // 参数说明：
    // - carId: 车厢编号
    // - temp: 当前温度（整数，单位：℃）
    // - tempThreshold: 当前温度阈值（整数，单位：℃）
    // - speed: 当前车速（整数，单位：km/h）
    // - etaMin / etaSec: 当 mode==0 时表示到站剩余分钟和秒；当 mode==1 时表示停站时间的分和秒
    // - mode: 运行模式（0=RUNNING_MODE，1=STOP_MODE），决定 ETA 与停站时间的显示位置
    // - doorOpen: 车门是否打开
    // - triggered: 是否触发报警（显示为“报警”并用红色背景）
    void updateStatus(int carId, int temp, int tempThreshold, int speed, int etaMin, int etaSec, int mode, bool doorOpen, bool triggered);

private:
    // statusTable: 用于显示所有车厢状态的 QTableWidget 实例（只读）
    QTableWidget *statusTable;
};

#endif // STATUSPAGE_H
