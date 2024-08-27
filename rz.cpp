#include "rz.h"
#include <QSqlQuery>
#include <QSqlRecord>

Rz::Rz() {}

QColor Rz::parseColor(QString color)
{
    int r = color.mid(1, 2).toInt(nullptr, 16);
    int g = color.mid(3, 2).toInt(nullptr, 16);
    int b = color.mid(5, 2).toInt(nullptr, 16);
    return QColor(r, g, b);
}

bool Rz::parseBool(QString value)
{
    return value.toLower() == "true";
}

QFont Rz::parseFont(QString fontValue)
{
    auto props = fontValue.split(",");
    QFont font = QFont(props[0], props[1].toInt());
    font.setBold(parseBool(props[2]));
    font.setItalic(parseBool(props[3]));
    return font;
}

QList<TableColumn> Rz::tableDesc(QSqlDatabase db, QString table)
{
    QSqlQuery query(db);
    query.exec(QString("show full columns from %1").arg(table));
    QList<TableColumn> r;
    while (query.next()) {
        r.append(TableColumn(
            query.value("Field").toString(),
            query.value("Type").toString(),
            query.value("Comment").toString()));
    }
    return r;
}
