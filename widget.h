#ifndef WIDGET_H
#define WIDGET_H

#include "qpushbutton.h"
#include "qsystemtrayicon.h"
#include <QWidget>
#include <QStackedWidget>

QT_BEGIN_NAMESPACE
namespace Ui {
class Widget;
}
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();
    void time_change();

private slots:
    void onTrayIconActivated(QSystemTrayIcon::ActivationReason reason);

private:
    Ui::Widget *ui;
    QPoint offset;
    QList<QString> fuctionButlist;
    QList<QString> fuctionImglist;
    QSystemTrayIcon *trayIcon;

    QPushButton *btnSystemSetting;
    QPushButton *btnReboot;

protected:
    void closeEvent(QCloseEvent *event) override;


};
#endif // WIDGET_H
