#include "databaseconnection.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>

DatabaseConnection::DatabaseConnection
    (DatasourceInfo *info)
{
    this->info = info;
    this->conn = QSqlDatabase::addDatabase(info->getType());
    this->conn.setHostName(info->getHost());
    this->conn.setUserName(info->getUsername());
    this->conn.setPassword(info->getPassword());
    this->conn.setPort(info->getPort());
    this->conn.setDatabaseName(info->getDatabase());

    if (!this->conn.open())
        QMessageBox::information(nullptr, "错误", this->conn.lastError().text());
}

QStringList DatabaseConnection::tables()
{
    QStringList tables;

    if (!this->conn.isOpen())
        return tables;

    QSqlQuery query;
    query.exec("show tables");
    while(query.next()){
        tables << query.value(0).toString();
    }
    return tables;
}

void DatabaseConnection::close()
{
    this->conn.close();
}
