#include "databaseconnection.h"
#include "table.h"
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

    if (info->getConnect()) {
        if (!this->conn.open())
            QMessageBox::information(nullptr, "错误", this->conn.lastError().text());
    }
}

QList<Table*>* DatabaseConnection::tables()
{
    QList<Table*>* tables = new QList<Table*>();

    if (!this->conn.isOpen())
        return tables;

    QMap<QString, QStringList>* indexColumns = new QMap<QString, QStringList>();

    QSqlQuery query;
    query.exec("show tables");
    while(query.next()){
        QSqlQuery keyQuery;
        auto name = query.value(0).toString();
        keyQuery.exec(QString("show index from %1").arg(name));
        QList<TableKey*>* indexes = new QList<TableKey*>();
        QList<TableKey*>* primaryKeys = new QList<TableKey*>();

        while (keyQuery.next()) {
            auto keyName = keyQuery.value(2).toString();
            if (indexColumns->contains(keyName)) {
                indexColumns->find(keyName)->append(keyQuery.value(4).toString());
            } else {
                indexColumns->insert(keyName, QStringList());
            }
        }
        for (QMap<QString, QStringList>::iterator it = indexColumns->begin(); it != indexColumns->end(); ++it) {
            if (it.key() == "PRIMARY") {
                primaryKeys->append(new TableKey(it.key(), it.value()));
            }
            indexes->append(new TableKey(it.key(), it.value()));
        }

        QList<TableColumn*>* columns = new QList<TableColumn*>();
        keyQuery.exec(QString("desc %1").arg(name));
        while(keyQuery.next()) {
            columns->append(new TableColumn(keyQuery.value(0).toString()));
        }

        tables->append(new Table(name, primaryKeys, indexes, columns));
        indexColumns->clear();
    }
    delete indexColumns;
    return tables;
}

void DatabaseConnection::close()
{
    if(this->conn.isOpen()) {
        this->conn.close();
    }
}

void DatabaseConnection::connect()
{
    this->conn.open();
}
