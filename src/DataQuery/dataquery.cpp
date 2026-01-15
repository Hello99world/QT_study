#include "dataquery.h"
#include "ui_dataquery.h"
#include "policerecord.h"
#include "operationlog.h"
#include "managelog.h"


#include <QHBoxLayout>
#include <QListWidget>
#include <QStackedWidget>

DataQuery::DataQuery(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::DataQuery)
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
        {"报警记录", ":/rec/image/databaoj.png"},
        {"运行记录", ":/rec/image/datayunxing.png"},
        {"操作记录", ":/rec/image/yonghu.png"},
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
    PoliceRecord *policerecord = new PoliceRecord;
    OperationLog *operationlog = new OperationLog;
    ManageLog *managelog = new ManageLog;

    stackedWidget->addWidget(policerecord);
    stackedWidget->addWidget(operationlog);
    stackedWidget->addWidget(managelog);

    // 链接
    connect(menuList, &QListWidget::currentRowChanged,
            stackedWidget, &QStackedWidget::setCurrentIndex);

}

DataQuery::~DataQuery()
{
    delete ui;
}
