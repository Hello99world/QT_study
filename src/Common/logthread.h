#ifndef LOGTHREAD_H
#define LOGTHREAD_H

#include <QObject>
#include <QThread>
#include <QDateTime>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>


// 登录日志数据结构
struct LoginLog
{
    QString userName;    // 用户名
    QString userType;    // 用户类型（如"管理员"/"普通用户"）
    QString logType;     // 日志类型（登录场景填"登录"）
    QString logContent;  // 日志内容（如"登录成功"/"登录失败"）
    QDateTime triggerTime; // 触发时间
};

class LogThread : public QThread
{
    Q_OBJECT
public:
        // 构造函数：接收日志数据
    explicit LogThread(const LoginLog& log, QObject *parent = nullptr);

protected:
    void run() override;

private:
    LoginLog m_log;
};

#endif // LOGTHREAD_H
