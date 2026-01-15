#include "qthread.h"
#include "ui_login.h"
#include "login.h"
#include "logthread.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QCheckBox>
#include <QPushButton>
#include <QMouseEvent>
#include <QMessageBox>
#include <QSettings>
#include <QPixmap>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

#include "qss.h"
#include "dbhelper.h"

// 全局变量定义
CurrentUser g_currentUser;

login::login(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::login)
{
    ui->setupUi(this);
    this->setObjectName("LoginWindow");
    Qss::loadStyleSheetFromResource(":/rec/qss/rec.qss", this);
    initUI();
    readFile();
    // tryLogin();
}


login::~login()
{
    delete ui;
}

// UI 构建
void login::initUI()
{
    // 主界面
    setWindowFlag(Qt::FramelessWindowHint);
    resize(400, 260);
    // 垂直布局
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    // 顶部窗口
    QWidget *top = new QWidget;
    top->setObjectName("top");
    // 水平布局
    QHBoxLayout *topLayout = new QHBoxLayout(top);
    topLayout->setContentsMargins(10, 0, 10, 0);
    // 图片标签
    QLabel *logo = new QLabel;
    logo->setFixedSize(20, 20);
    QPixmap pix(":/rec/image/login.png");
    logo->setPixmap(pix.scaled(logo->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    logo->setObjectName("logoLabel");
    QLabel *title = new QLabel("用户登录");

    closeLabel = new QLabel();
    closeLabel->setObjectName("closeLabel");
    closeLabel->setFixedSize(20, 20);
    // 事件过滤器
    closeLabel->installEventFilter(this);
    // 加入水平
    topLayout->addWidget(logo);
    topLayout->addStretch();
    topLayout->addWidget(title);
    topLayout->addStretch();
    topLayout->addWidget(closeLabel);

    // 中部
    QWidget *center = new QWidget;
    center->setObjectName("center");
    QLabel *sysTitle = new QLabel("蜗牛物联网监控平台");
    sysTitle->setAlignment(Qt::AlignCenter);
    // 水平布局
    QVBoxLayout *centerLayout = new QVBoxLayout(center);
    centerLayout->addWidget(sysTitle);

    // 底部
    QWidget *bottom = new QWidget;
    bottom->setObjectName("bottom");
    QGridLayout *grid = new QGridLayout(bottom);
    grid->setContentsMargins(20, 10, 20, 20);
    grid->setHorizontalSpacing(10);
    grid->setVerticalSpacing(10);

    QLabel *userLabel = new QLabel("用户名：");
    QLabel *pwdLabel  = new QLabel("密码：");
    usernameEdit = new QLineEdit;
    passwordEdit = new QLineEdit;
    passwordEdit->setEchoMode(QLineEdit::Password);

    QPushButton *loginBtn = new QPushButton("登录");
    QPushButton *closeBtn = new QPushButton("关闭");
    rememberCheck  = new QCheckBox("记住密码");
    autoLoginCheck = new QCheckBox("自动登录");

    grid->addWidget(userLabel,     0, 0);
    grid->addWidget(usernameEdit,  0, 1, 1, 2);
    grid->addWidget(loginBtn,      0, 3);
    grid->addWidget(pwdLabel,      1, 0);
    grid->addWidget(passwordEdit,  1, 1, 1, 2);
    grid->addWidget(closeBtn,      1, 3);
    grid->addWidget(rememberCheck, 2, 1);
    grid->addWidget(autoLoginCheck,2, 2);

    mainLayout->addWidget(top, 1);
    mainLayout->addWidget(center, 2);
    mainLayout->addWidget(bottom, 4);

    connect(closeBtn, &QPushButton::clicked, this, &QWidget::close);

    connect(loginBtn, &QPushButton::clicked, this, [=]() {
        QString user = usernameEdit->text().trimmed();
        QString pwd  = passwordEdit->text();

        if (user.isEmpty() || pwd.isEmpty()) {
            QMessageBox::warning(this, "", "用户名或密码不能为空");
            return;
        }

        if (checkUser(user, pwd)) {
            writeFile();
            startLoginLogThread(user);
            emit accept();
        } else {
            QMessageBox::warning(this, "", "用户名或密码错误");
        }
    });
}


// 登录逻辑
bool login::checkUser(const QString &username, const QString &pwd)
{
    QSqlDatabase db = DbHelper::getDatabase("iot.db");
    if (db.open())
    {
        QString sql = "select UserName, UserType ,Permission1, Permission2 from userinfo where username =:username";
        // QSqlQuery query;
        QSqlQuery query(db);
        query.prepare(sql);
        query.bindValue(":username", username);
        if (query.exec())
        {
            if (query.next())
            {
                qDebug() << query.value(0);
                QString dbPwd = query.value(0).toString();
                if (dbPwd == pwd)
                {
                    // 保存数据
                    g_currentUser.userName = username;
                    g_currentUser.userType = query.value(1).toString();
                    g_currentUser.permissionSystemSetting = (query.value(2).toString() == "启用");
                    g_currentUser.permissionReboot        = (query.value(3).toString() == "启用");
                    return true;
                }
                else
                {
                    //QMessageBox::information(this, "", "用户名或密码不正确");
                    return false;
                }
            }
        }
        else
        {
            qDebug() << query.lastError().text();
        }
    }
    return false;
}

void login::writeFile()
{
    QSettings s("login.ini", QSettings::IniFormat);
    s.setValue("username", usernameEdit->text());
    s.setValue("password", passwordEdit->text());
    s.setValue("remember", rememberCheck->isChecked());
    s.setValue("autologin", autoLoginCheck->isChecked());
}

void login::readFile()
{
    QSettings s("login.ini", QSettings::IniFormat);
    if (s.value("remember").toBool()) {
        usernameEdit->setText(s.value("username").toString());
        passwordEdit->setText(s.value("password").toString());
        rememberCheck->setChecked(true);
        autoLoginCheck->setChecked(s.value("autologin").toBool());
    }
}

bool login::tryLogin()
{
    QSettings s("login.ini", QSettings::IniFormat);
    if (!s.value("autologin").toBool())
        return false;

    QString username = s.value("username").toString();
    if (checkUser(
            username,
            s.value("password").toString()))
    {
        emit accept();
        startLoginLogThread(username);
        return true;
    }

    return false;
}

void login::startLoginLogThread(const QString &username)
{
    // 1. 获取用户类型
    QSqlDatabase db = DbHelper::getDatabase("iot.db");
    if (db.open())
    {
        QString sql = "select usertype from userinfo where username =:username";
        QSqlQuery query;
        query.prepare(sql);
        query.bindValue(":username", username);
        if (query.exec() && query.next()){
            QString userType = query.value(0).toString();
            // 2. 构造日志内容
            LoginLog log;
            log.userName = username;
            log.userType = userType;
            log.logType = "用户操作";
            log.logContent = "用户登录";
            log.triggerTime = QDateTime::currentDateTime(); // 当前时间

            // 3. 启动日志线程
            LogThread* logThread = new LogThread(log);
            connect(logThread, &LogThread::finished, logThread, &LogThread::deleteLater);
            logThread->start();
        }
    }
}


// 无边框拖动
bool login::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == closeLabel && event->type() == QEvent::MouseButtonPress) {
        close();
        return true;
    }
    return QWidget::eventFilter(watched, event);
}

void login::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
        dragPos = event->globalPos() - frameGeometry().topLeft();
}

void login::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton)
        move(event->globalPos() - dragPos);
}
