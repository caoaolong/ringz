#include "rz.h"
#include "ringz.h"
#include <QSqlQuery>
#include <QSqlRecord>
#include <QJsonObject>
#include <QJsonArray>

Rz::Rz() {}

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

QString Rz::tokenColor(QString name)
{
    auto array = Ringz::getTheme("tokenColors").toArray();
    for (auto item : array) {
        auto o = item.toObject();
        if (o["scope"] == name) {
            return o["settings"].toObject()["foreground"].toString();
        }
    }
    return "";
}
