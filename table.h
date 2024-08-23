#ifndef TABLE_H
#define TABLE_H

#include "tablekey.h"
#include "tablecolumn.h"

class Table
{
public:
    Table();
    Table(QString name, QList<TableKey*>* primaryKeys, QList<TableKey*>* indexes, QList<TableColumn*>* columns);
    QString getName() const;
    QList<TableKey *> *getPrimaryKeys() const;
    QList<TableKey *> *getIndexes() const;

    QList<TableColumn *> *getColumns() const;
    void setColumns(QList<TableColumn *> *newColumns);

private:
    QString name;
    QList<TableKey*>* primaryKeys;
    QList<TableKey*>* indexes;
    QList<TableColumn*>* columns;
};

#endif // TABLE_H
