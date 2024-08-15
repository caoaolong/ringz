#include "tablecolumn.h"

TableColumn::TableColumn(QString name)
{
    this->name = name;
}

QString TableColumn::getName() const
{
    return name;
}
