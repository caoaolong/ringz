#ifndef RZ_H
#define RZ_H

#include "tablecolumn.h"
#include <QColor>
#include <QFont>
#include <QSqlDatabase>
#include <QList>

class Rz
{
public:
    Rz();
    static bool parseBool(QString value);
    static QFont parseFont(QString fontValue);
    static QList<TableColumn> tableDesc(QSqlDatabase db, QString table);
    static QString tokenColor(QString name);
};

#endif // RZ_H
