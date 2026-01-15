#include "dbhelper.h"

#include <QDir>
#include <QCoreApplication>


DbHelper::DbHelper() {}

QSqlDatabase DbHelper::getDatabase(QString dbName, QString defaultName)
{
    QSqlDatabase db;
    if (QSqlDatabase::contains(defaultName))
    {
        db = QSqlDatabase::database(defaultName);
    }
    else
    {
        db = QSqlDatabase::addDatabase("QSQLITE");
    }
    db.setDatabaseName(dbName);
    return db;
}


