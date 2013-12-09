#ifndef SQLITE_H
#define SQLITE_H

#include "common.h"
#include <QtSql/QtSql>
#include "devicesinfo.h"
#include "log.h"

class  Sqlite{
public:
    Sqlite();
    bool updatedevicesinfomap(QMap<QString, QMap<QString, QString> *> *);
    bool updatedevicesinfotable(DevicesInfo &);

private:
    static QString m_databasename;
    static QString m_driver;
    QSqlDatabase m_db;

    static QMutex m_sql_mutex;

    bool init();
};

#endif // SQLITE_H
