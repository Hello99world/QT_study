#include "widget.h"
#include "qss.h"
#include "login.h"

#include <QApplication>

// 字体/图标 #498fbc  rgb(73,143,188)
// 首部背景 #133050
// 左侧功能栏/首部按钮悬空 #033967
// 背景 #0E1A32


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Qss::loadStyleSheetFromResource(":/rec/qss/style.qss", &a);
    //Widget w;
    //w.show();

    Widget *w;//延迟创建
    login login;

    QObject::connect(&login, &login::accept, [&](){
        qDebug() << "accpet";
        login.close();
        w = new Widget;
        w->setAttribute(Qt::WA_DeleteOnClose);//指针延迟删除，关闭窗口后自动删除
        w->show();

    });
    //login.show();
    if (!login.tryLogin())
    {
        login.show();
    }

    return a.exec();
}
