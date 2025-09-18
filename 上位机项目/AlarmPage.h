#ifndef ALARMPAGE_H
#define ALARMPAGE_H

#include <QWidget>

class AlarmLog;

class AlarmPage : public QWidget {
    Q_OBJECT
public:
    explicit AlarmPage(AlarmLog *log, QWidget *parent = nullptr);
private:
    AlarmLog *alarmLog;
};

#endif // ALARMPAGE_H
