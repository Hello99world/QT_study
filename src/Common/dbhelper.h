#ifndef DBHELPER_H
#define DBHELPER_H

#include <QSqlDatabase>

class DbHelper
{
public:
    DbHelper();
    static QSqlDatabase getDatabase(QString dbName, QString defaultName = QSqlDatabase::defaultConnection);
};

#endif // DBHELPER_H
