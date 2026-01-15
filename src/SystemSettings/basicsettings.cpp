#include "basicsettings.h"
#include "qcombobox.h"
#include "qlabel.h"
#include "ui_basicsettings.h"
#include "qss.h"

#include "switchbutton.h"
#include <QSettings>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QLineEdit>
#include <QPushButton>
#include <QMessageBox>
#include <QFile>
#include <QCoreApplication>

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

BasicSettings::BasicSettings(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::BasicSettings)
{
    ui->setupUi(this);
    this->setObjectName("basicsettings");
    Qss::loadStyleSheetFromResource(":/rec/qss/BasicSettings.qss", this);
    initUI();

}

BasicSettings::~BasicSettings()
{
    delete ui;
}


void BasicSettings::initUI()
{
    //本地数据库
    QGroupBox *localBox = new QGroupBox("本地数据库设置");

    localConnType = new QComboBox;
    localConnType->addItem("直连数据库");
    localConnType->setEnabled(false);

    localDbNameEdit = new QLineEdit("iotsystem");

    localTestBtn = new QPushButton("连接测试");
    localTestBtn->setMinimumHeight(36);

    QGridLayout *localGrid = new QGridLayout;
    localGrid->addWidget(new QLabel("连接方式"), 0, 0);
    localGrid->addWidget(localConnType, 0, 1, 1, 3);

    localGrid->addWidget(new QLabel("数据库名"), 1, 0);
    localGrid->addWidget(localDbNameEdit, 1, 1, 1, 3);

    localGrid->addWidget(localTestBtn, 2, 0, 1, 4);

    localBox->setLayout(localGrid);

    //云端数据库
    QGroupBox *remoteBox = new QGroupBox("云端数据库同步");

    // syncSwitch = new QLabel("关闭");
    // syncSwitch->setObjectName("SyncSwitch");
    syncSwitch = new SwitchButton(this);
    syncSwitch->setTextOn("开启");
    syncSwitch->setTextOff("关闭");
    syncSwitch->setBgColorOn(QColor("#498fbc"));
    syncSwitch->setBgColorOff(QColor("#555555"));
    syncSwitch->setFixedSize(80, 24);

    remoteConnType = new QComboBox;
    remoteConnType->addItem("直连数据库");
    remoteConnType->setEnabled(false);

    remoteDbType = new QComboBox;
    remoteDbType->addItem("MySql");
    remoteDbType->setEnabled(false);

    remoteDbNameEdit = new QLineEdit("iotsystem");
    hostEdit = new QLineEdit("127.0.0.1:3306");
    userEdit = new QLineEdit("root");
    passwordEdit = new QLineEdit;
    passwordEdit->setEchoMode(QLineEdit::Password);

    remoteTestBtn = new QPushButton("测试连接");
    remoteTestBtn->setMinimumHeight(36);

    QGridLayout *remoteGrid = new QGridLayout;
    remoteGrid->addWidget(new QLabel("远程同步"), 0, 0);
    remoteGrid->addWidget(syncSwitch, 0, 1);

    remoteGrid->addWidget(new QLabel("连接方式"), 1, 0);
    remoteGrid->addWidget(remoteConnType, 1, 1);
    remoteGrid->addWidget(new QLabel("主机类型"), 1, 2);
    remoteGrid->addWidget(remoteDbType, 1, 3);

    remoteGrid->addWidget(new QLabel("数据库名"), 2, 0);
    remoteGrid->addWidget(remoteDbNameEdit, 2, 1);
    remoteGrid->addWidget(new QLabel("主机信息"), 2, 2);
    remoteGrid->addWidget(hostEdit, 2, 3);

    remoteGrid->addWidget(new QLabel("用户名"), 3, 0);
    remoteGrid->addWidget(userEdit, 3, 1);
    remoteGrid->addWidget(new QLabel("用户密码"), 3, 2);
    remoteGrid->addWidget(passwordEdit, 3, 3);

    remoteGrid->addWidget(remoteTestBtn, 4, 0, 1, 4);

    remoteBox->setLayout(remoteGrid);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(localBox);
    mainLayout->addWidget(remoteBox);

    // 底部组件
    QHBoxLayout *bottomLayout = new QHBoxLayout;
    bottomLayout->addStretch();

    autoLoginBtn = new SwitchButton(this);
    autoLoginBtn->setBgColorOn(Qt::green);
    autoLoginBtn->setBgColorOff(Qt::red);
    autoLoginBtn->setTextOff("取消自动登陆");
    autoLoginBtn->setTextOn("自动登陆");
    autoLoginBtn->setFixedSize(100, 24);

    bottomLayout->addWidget(autoLoginBtn);

    mainLayout->addLayout(bottomLayout);
    mainLayout->addSpacing(8);

    QSettings settings("login.ini", QSettings::IniFormat);
    bool autoLogin = settings.value("autologin").toBool();
    autoLoginBtn->setChecked(autoLogin);

    connect(autoLoginBtn, &SwitchButton::checkedChanged, this, [](bool checked){
        QSettings settings("login.ini", QSettings::IniFormat);
        settings.setValue("autologin", checked);
    });


    connect(localTestBtn, &QPushButton::clicked, this, &BasicSettings::testLocalDatabase);
    connect(remoteTestBtn, &QPushButton::clicked, this, &BasicSettings::testRemoteDatabase);

}

// 本地数据库测试

void BasicSettings::testLocalDatabase()
{
    QString dbName = localDbNameEdit->text().trimmed();
    if (dbName.isEmpty()) {
        QMessageBox::warning(this, "提示", "请输入数据库名");
        return;
    }

    QString dbPath =
        QCoreApplication::applicationDirPath() + "/" + dbName + ".db";

    if (!QFile::exists(dbPath)) {
        QMessageBox::critical(this, "失败", "数据库文件不存在：\n" + dbPath);
        return;
    }

    const QString connName = "local_test_conn";
    QSqlDatabase db;

    if (QSqlDatabase::contains(connName))
        db = QSqlDatabase::database(connName);
    else
        db = QSqlDatabase::addDatabase("QSQLITE", connName);

    db.setDatabaseName(dbPath);

    if (!db.open()) {
        QMessageBox::critical(this, "失败", db.lastError().text());
        return;
    }

    QSqlQuery q(db);
    if (!q.exec("SELECT name FROM sqlite_master LIMIT 1")) {
        QMessageBox::critical(this, "失败", "不是有效的 SQLite 数据库");
        return;
    }

    QMessageBox::information(this, "成功", "本地数据库连接成功");
}

// 云端数据库测试

void BasicSettings::testRemoteDatabase()
{
    QString dbName = remoteDbNameEdit->text().trimmed();
    QString host   = hostEdit->text().trimmed();
    QString user   = userEdit->text().trimmed();
    QString pwd    = passwordEdit->text();

    if (dbName.isEmpty() || host.isEmpty() || user.isEmpty()) {
        QMessageBox::warning(this, "提示", "参数未填写完整");
        return;
    }

    QString ip = host.section(':', 0, 0);
    int port   = host.contains(':') ? host.section(':', 1, 1).toInt() : 3306;

    const QString connName = "mysql_test_conn";
    QSqlDatabase db;

    if (QSqlDatabase::contains(connName))
        db = QSqlDatabase::database(connName);
    else
        db = QSqlDatabase::addDatabase("QMYSQL", connName);

    db.setHostName(ip);
    db.setPort(port);
    db.setUserName(user);
    db.setPassword(pwd);
    db.setDatabaseName(dbName);

    if (!db.open()) {
        QMessageBox::critical(this, "失败", db.lastError().text());
        return;
    }

    QMessageBox::information(this, "成功", "MySQL 连接成功");
}
