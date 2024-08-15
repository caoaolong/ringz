#ifndef TABLEKEY_H
#define TABLEKEY_H

#include <QString>
#include <QStringList>

class TableKey
{
public:
    TableKey(QString name, QStringList columns);
    QString getName() const;
    void setName(const QString &newName);
    QStringList getColumns() const;
    void setColumns(const QStringList &newColumns);

private:
    QString name;
    QStringList columns;
};

#endif // TABLEKEY_H
