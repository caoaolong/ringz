#include "table.h"

Table::Table()
{

}

Table::Table(QString name, QList<TableKey*>* primaryKeys, QList<TableKey*>* indexes, QList<TableColumn*>* columns)
{
    this->name = name;
    this->primaryKeys = primaryKeys;
    this->indexes = indexes;
    this->columns = columns;
}

QString Table::getName() const
{
    return name;
}

QList<TableKey *> *Table::getPrimaryKeys() const
{
    return primaryKeys;
}

QList<TableKey *> *Table::getIndexes() const
{
    return indexes;
}

QList<TableColumn *> *Table::getColumns() const
{
    return columns;
}

void Table::setColumns(QList<TableColumn *> *newColumns)
{
    columns = newColumns;
}
