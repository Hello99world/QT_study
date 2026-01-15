#include "widget.h"
#include "logthread.h"
#include "qevent.h"
#include "qlcdnumber.h"
#include "qmenu.h"
#include "ui_widget.h"

#include "dataquery.h"
#include "equipmentmonitor.h"
#include "othersettings.h"
#include "systemsettings.h"
#include "framelesswidget.h"
#include "login.h"

#include <qlabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QListWidget>
#include <QPushButton>
#include <QButtonGroup>
#include <QStackedLayout>
#include <QTimer>
#include <QTime>
#include <QProcess>
#include <QSystemTrayIcon>
#include <QMenu>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    // 0.主窗口
    ui->setupUi(this);
    FrameLessWidget *widget = new FrameLessWidget(this);
    this->setObjectName("MainWindow");
    this->setMinimumSize(1200,700);
    setWindowFlags(Qt::FramelessWindowHint | windowFlags());
    // 主窗口垂直布局
    QVBoxLayout * mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    // 1.首部：
    QWidget *topBar = new QWidget(this);
    topBar->setObjectName("topBar");
    topBar->setFixedHeight(70);
    mainLayout->addWidget(topBar,1); // 加入主垂直布局
    // 首部水平布局
    QHBoxLayout *topLayout = new QHBoxLayout(topBar);
    topLayout->setContentsMargins(0, 0, 0, 0);
    topLayout->setSpacing(0);
    // 1.1左侧标题
    QWidget *leftTop = new QWidget(topBar);
    leftTop->setObjectName("topLeft");
    QLabel *logoLabel = new QLabel(leftTop);
    logoLabel->setPixmap(QPixmap(":/rec/image/mainlogo.png"));

    logoLabel->setFixedSize(42, 42);
    logoLabel->setScaledContents(true);

    QLabel *titleLabel = new QLabel("蜗牛物联网监控平台", leftTop);
    QLabel *entitleLabel = new QLabel("IOT integrated management platform", leftTop);

    titleLabel->setObjectName("cnTitle");
    entitleLabel->setObjectName("enTitle");


    QVBoxLayout *textLayout = new QVBoxLayout();
    textLayout->addWidget(titleLabel);
    textLayout->addWidget(entitleLabel);
    textLayout->setContentsMargins(0, 0, 0, 0);
    textLayout->setSpacing(2);
    QHBoxLayout *TitleLayout = new QHBoxLayout(leftTop);
    TitleLayout->addWidget(logoLabel);
    TitleLayout->addLayout(textLayout);
    TitleLayout->setContentsMargins(10, 0, 0, 0);
    TitleLayout->setSpacing(10);
    leftTop->setLayout(TitleLayout);
    topLayout->addWidget(leftTop); // 加入首部水平布局
    // 1.2中部功能
    QWidget *mainTop = new QWidget(topBar);
    mainTop->setObjectName("topCenter");
    mainLayout->addWidget(mainTop, 10);
    // 堆叠布局
    QStackedLayout *stackedLayout = new QStackedLayout(mainTop);
    DataQuery *dataquery = new DataQuery;
    EquipmentMonitor *equipmentmonitor = new EquipmentMonitor;
    OtherSettings *othersettings = new OtherSettings;
    SystemSettings *systemsettings = new SystemSettings;
    stackedLayout->addWidget(equipmentmonitor); // 0
    stackedLayout->addWidget(dataquery);        // 1
    stackedLayout->addWidget(systemsettings);   // 2
    stackedLayout->addWidget(othersettings);    // 3
    // 功能按键
    QButtonGroup *fuctionButGroup = new QButtonGroup(this);
    fuctionButlist = {"设备监控", "数据查询", "系统设置", "其他设置", "系统重启"};
    fuctionImglist = {":/rec/image/jiankong.png",":/rec/image/shuju.png",":/rec/image/tiaoj.png",":/rec/image/shezhi.png",":/rec/image/yanshi.png"};
    for (int i = 0; i < fuctionButlist.size(); i++)
    {
        QPushButton *btn = new QPushButton(fuctionButlist.at(i));
        if (fuctionButlist.at(i) == "系统设置")
            btnSystemSetting = btn;
        if (fuctionButlist.at(i) == "系统重启")
            btnReboot = btn;
        btn->setIcon(QIcon(fuctionImglist.at(i)));
        btn->setObjectName("topFuncBtn");
        topLayout->addWidget(btn);
        //同一组按钮互斥
        btn->setCheckable(true);
        fuctionButGroup->addButton(btn);
        if (i == 0)
        {
            btn->setChecked(true);
        }
        if (i != fuctionButlist.size()-1)
        {
            connect(btn, &QPushButton::clicked, [=](){
            stackedLayout->setCurrentIndex(i);
            });
        }
        else  // 系统重启
        {

            connect(btn, &QPushButton::clicked, [=](){

                // 写重启日志
                LoginLog log;
                log.userName = g_currentUser.userName;
                log.userType = g_currentUser.userType;
                log.logType = "设备上报";
                log.logContent = "系统重启";
                log.triggerTime = QDateTime::currentDateTime();

                LogThread *t = new LogThread(log);
                connect(t, &QThread::finished, t, &QObject::deleteLater);
                t->start();

                //获取当前应用程序的路径
                QString program =  QApplication::applicationFilePath();
                QApplication::quit();
                QProcess::startDetached(program);

            });
        }
    }
    // 1.3右侧功能
    QWidget *rightTop = new QWidget(topBar);
    rightTop->setObjectName("topRight");
    topLayout->addWidget(rightTop);
    // 垂直布局
    QVBoxLayout *rightLayout = new QVBoxLayout(rightTop);
    rightLayout->setContentsMargins(0, 0, 0, 0);
    rightLayout->setSpacing(5);
    rightLayout->setAlignment(Qt::AlignCenter);
    // 三按钮
    QWidget *btnArea = new QWidget(rightTop);
    QHBoxLayout *btnLayout  = new QHBoxLayout(btnArea);
    btnLayout ->setContentsMargins(0, 0, 0, 0);
    btnLayout ->setSpacing(0);
    btnArea->setLayout(btnLayout);
    QPushButton *btnMin = new QPushButton(this);
    QPushButton *btnMax = new QPushButton(this);
    QPushButton *btnClose = new QPushButton(this);
    btnMin->setObjectName("winBtnMin");
    btnMax->setObjectName("winBtnMax");
    btnClose->setObjectName("winBtnClose");
    btnMin->setIcon(QIcon(":/rec/image/zuixiao.png"));
    btnMax->setIcon(QIcon(":/rec/image/zuoda.png"));
    btnClose->setIcon(QIcon(":/rec/image/guanbi.png"));

    // connect(btnMin, &QPushButton::clicked, this, &QWidget::showMinimized);
    connect(btnMin, &QPushButton::clicked, this, [this]() {
        hide();   // 最小化到托盘
    });
    connect(btnMax, &QPushButton::clicked, this, [=]() {
        isMaximized() ? showNormal() : showMaximized();
    });
    connect(btnClose, &QPushButton::clicked, this, &QWidget::close);
    // 创建托盘菜单
    trayIcon = new QSystemTrayIcon(QIcon(":/rec/image/min.JPG"), this);
    //创建菜单
    QMenu *trayMenu = new QMenu(this);
    QAction *restoreAction = new QAction("恢复窗口", this);
    QAction *quitAction = new QAction("退出程序", this);
    trayMenu->addAction(restoreAction);
    trayMenu->addAction(quitAction);
    //将菜单添加到托盘
    trayIcon->setContextMenu(trayMenu);
    // 连接托盘信号
    // 当用户点击/双击/右键等操作托盘图标时，触发onTrayIconActivated函数
    connect(trayIcon, &QSystemTrayIcon::activated, this, &Widget::onTrayIconActivated);
    // 当用户点击托盘右键菜单中的“恢复”选项时，恢复主窗口为正常显示状态
    connect(restoreAction, &QAction::triggered, this, &Widget::showNormal);
    // 当用户点击托盘右键菜单中的“退出”选项时，关闭整个应用程序
    connect(quitAction, &QAction::triggered, qApp, &QApplication::quit);
    // 显示托盘图标
    trayIcon->show();
    this->setWindowIcon(QIcon(":/rec/image/min.JPG"));

    btnLayout ->addWidget(btnMin);
    btnLayout ->addWidget(btnMax);
    btnLayout ->addWidget(btnClose);
    rightLayout->addWidget(btnArea);
    // 实时时间
    QLCDNumber *lcdNumber = new QLCDNumber(rightTop);

    lcdNumber->setObjectName("digitalClock");
    lcdNumber->setDigitCount(8);
    lcdNumber->setSegmentStyle(QLCDNumber::Flat);

    QTime initTime = QTime::currentTime();
    lcdNumber->display(initTime.toString("hh:mm:ss"));
    // 创建定时器
    QTimer *timeTimer = new QTimer(this);
    connect(timeTimer, &QTimer::timeout, this, [lcdNumber](){
        QTime currentTime = QTime::currentTime();
        lcdNumber->display(currentTime.toString("hh:mm:ss"));
    });
    timeTimer->start(1000); // 1秒更新一次

    rightLayout->addWidget(lcdNumber);
    // 禁用系统设置
    if (!g_currentUser.permissionSystemSetting)
    {
        btnSystemSetting->setEnabled(false);
    }

    // 禁用系统重启
    if (!g_currentUser.permissionReboot)
    {
        btnReboot->setEnabled(false);
    }
}

Widget::~Widget()
{
    delete ui;
}

void Widget::onTrayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
    // 判断激活原因：单击（Trigger）或双击（DoubleClick）
    // 注：不同系统对单击/双击的识别可能略有差异，同时处理两种情况提升兼容性
    if (reason == QSystemTrayIcon::Trigger || reason == QSystemTrayIcon::DoubleClick)
    {
        // 恢复窗口为普通显示状态（解除最小化/隐藏状态）
        showNormal();

        // 将窗口提升到所有其他窗口之上（置顶显示）
        raise();

        // 激活窗口，使其获得键盘焦点并成为活动窗口
        activateWindow();
    }
}

void Widget::closeEvent(QCloseEvent *event)
{
    // 只有登录成功的用户才记录退出日志
    if (!g_currentUser.userName.isEmpty())
    {
        LoginLog log;
        log.userName = g_currentUser.userName;
        log.userType = g_currentUser.userType;
        log.logType = "用户操作";
        log.logContent = "用户退出";
        log.triggerTime = QDateTime::currentDateTime();

        LogThread *t = new LogThread(log);
        connect(t, &QThread::finished, t, &QObject::deleteLater);
        t->start();
    }

    event->accept();
}
