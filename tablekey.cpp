#include "tablekey.h"

TableKey::TableKey(QString name, QStringList columns)
{
    this->name = name;
    this->columns = columns;
}

QString TableKey::getName() const
{
    return name;
}

void TableKey::setName(const QString &newName)
{
    name = newName;
}

QStringList TableKey::getColumns() const
{
    return columns;
}

void TableKey::setColumns(const QStringList &newColumns)
{
    columns = newColumns;
}
