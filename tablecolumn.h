#ifndef TABLECOLUMN_H
#define TABLECOLUMN_H

#include <QString>

class TableColumn
{
public:
    TableColumn(QString name);
    QString getName() const;

private:
    QString name;
};

#endif // TABLECOLUMN_H
