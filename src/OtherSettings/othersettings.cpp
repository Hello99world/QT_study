#include "othersettings.h"
#include "ui_othersettings.h"
#include "usermanage.h"

#include <QHBoxLayout>
#include <QListWidget>
#include <QStackedWidget>

OtherSettings::OtherSettings(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::OtherSettings)
{
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
        {"用户管理", ":/rec/image/yonghu.png"},
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
    UserManage *usermanage = new UserManage;

    stackedWidget->addWidget(usermanage);

    // 链接
    connect(menuList, &QListWidget::currentRowChanged,
            stackedWidget, &QStackedWidget::setCurrentIndex);
}

OtherSettings::~OtherSettings()
{
    delete ui;
}
