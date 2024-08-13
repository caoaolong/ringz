#ifndef DATABASECONNECTION_H
#define DATABASECONNECTION_H

#include "datasource.h"
#include <QSqlDatabase>

class DatabaseConnection
{
public:
    DatabaseConnection(DatasourceInfo *info);
    QSqlDatabase get() {return conn;};
    DatasourceInfo *datasource() {return info;};
    QStringList tables();
    void close();

private:
    DatasourceInfo *info;
    QSqlDatabase conn;
};

#endif // DATABASECONNECTION_H
