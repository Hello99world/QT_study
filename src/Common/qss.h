#ifndef QSS_H
#define QSS_H

#include <QFile>
#include <QApplication>
#include <QWidget>

class Qss
{
public:
    Qss();
    // 全程序QSS
    static void loadStyleSheetFromResource(QString source, QApplication *target) {
        QFile file(source);
        file.open(QFile::ReadOnly);
        QByteArray qss = file.readAll();
        target->setStyleSheet(qss);
        file.close();
    }
    // 组件QSS
    static void loadStyleSheetFromResource(QString source, QWidget *target) {
        QFile file(source);
        file.open(QFile::ReadOnly);
        QByteArray qss = file.readAll();
        target->setStyleSheet(qss);
        file.close();
    }
};

#endif // QSS_H
