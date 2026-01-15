#include "logthread.h"

LogThread::LogThread(const LoginLog& log, QObject *parent)  : QThread(parent), m_log(log) {}

void LogThread::run()
{
    // 1. 线程内创建独立数据库连接（避免跨线程冲突）
    //QSqlDatabase不支持跨线程共用，共用会导致崩溃,连接在哪个线程创建，就只能在哪个线程使用，销毁也必须在这个线程内；
    //核心目的是保证线程安全，避免多个线程同时操作同一个连接资源引发的冲突。
    QString connName = QString("LogThread_%1").arg((quintptr)QThread::currentThreadId());
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", connName);
    db.setDatabaseName("iot.db");
    if (!db.open())
    {
        qDebug() << "数据库连接失败：" << db.lastError().text();
        return;
    }

    // 2. 插入日志（适配新表字段）
    QSqlQuery query(db);
    QString sql = "INSERT INTO userlog ("
                  "UserName, UserType, LogType, LogContent, TriggerTime"
                  ") VALUES (:userName, :userType, :logType, :logContent, :triggerTime)";
    query.prepare(sql);
    query.bindValue(":userName", m_log.userName);
    query.bindValue(":userType", m_log.userType);
    query.bindValue(":logType", m_log.logType);
    query.bindValue(":logContent", m_log.logContent);
    query.bindValue(":triggerTime", m_log.triggerTime.toString("yyyy-MM-dd HH:mm:ss"));

    if (!query.exec())
    {
        qDebug() << "日志写入失败：" << query.lastError().text();
    }

    db.close();
}
