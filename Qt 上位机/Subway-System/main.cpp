#include "mainwindow.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>

// 程序入口：初始化 QApplication、设置默认字体并尝试加载系统语言的翻译文件
// 说明：翻译文件资源位于资源路径 ":/i18n/Subway-System_<locale>"，若找到则安装翻译器
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    // 设置默认界面字体（可按需修改）
    QApplication::setFont(QFont("Microsoft YaHei", 14));
    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "Subway-System_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }
    MainWindow w;
    w.show();
    return a.exec();
}
