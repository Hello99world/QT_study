#include "systemsettings.h"
#include "ui_systemsettings.h"
#include "basicsettings.h"
#include "detectormanagement.h"
#include "portmanagement.h"
#include "typesetting.h"

#include <QHBoxLayout>
#include <QListWidget>
#include <QStackedWidget>

SystemSettings::SystemSettings(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::SystemSettings)
{
    // 系统设置父窗口
    ui->setupUi(this);
    // 水平布局
    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    // 左侧菜单
    QListWidget *menuList = new QListWidget;
    menuList->setObjectName("sysMenu");
    mainLayout->addWidget(menuList, 1);
    QList<QPair<QString, QString>> menuData = {
        {"基本设置", ":/rec/image/dsys0.png"},
        {"端口管理", ":/rec/image/dsys1.png"},
        {"探测器管理", ":/rec/image/dsys2.png"},
        {"类型设置", ":/rec/image/dsys3.png"}
    };
    for (int i = 0; i < menuData.size(); ++i) {
        QString text = menuData[i].first;
        QString iconPath = menuData[i].second;
        QListWidgetItem *item = new QListWidgetItem(QIcon(iconPath), text);
        menuList->addItem(item);
    }
    // 右侧页面
    QStackedWidget *stackedWidget = new QStackedWidget(this);
    stackedWidget->setObjectName("sysContent");
    mainLayout->addWidget(stackedWidget, 4);
    BasicSettings *basicsettings = new BasicSettings;
    DetectorManagement *detectormanagement = new DetectorManagement;
    PortManagement *portmanagement = new PortManagement;
    TypeSetting *typessetting = new TypeSetting;
    stackedWidget->addWidget(basicsettings);
    stackedWidget->addWidget(portmanagement);
    stackedWidget->addWidget(detectormanagement);
    stackedWidget->addWidget(typessetting);
    // 链接
    connect(menuList, &QListWidget::currentRowChanged,
            stackedWidget, &QStackedWidget::setCurrentIndex);
}

SystemSettings::~SystemSettings()
{
    delete ui;
}
