#ifndef DATABASECONNECTION_H
#define DATABASECONNECTION_H

#include "datasource.h"
#include "table.h"
#include <QSqlDatabase>

class DatabaseConnection
{
public:
    DatabaseConnection(DatasourceInfo *info);
    QSqlDatabase get() {return conn;};
    DatasourceInfo *datasource() {return info;};
    QList<Table*>* tables();
    void close();
    void connect();

private:
    DatasourceInfo *info;
    QSqlDatabase conn;
};

#endif // DATABASECONNECTION_H
