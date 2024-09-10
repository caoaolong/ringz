#include "rz.h"
#include "ringz.h"
#include <QSqlQuery>
#include <QSqlRecord>
#include <QJsonObject>
#include <QJsonArray>
#include <QRegularExpression>

QStringList Rz::javaTypeMap =
    QStringList(J_BOOL) << J_BYTE << J_SHORT << J_INT << J_LONG << J_STRING << J_FLOAT << J_DOUBLE << J_DATE << J_CHAR;

QStringList Rz::golangTypeMap =
    QStringList(G_BOOL) << G_BYTE << G_SHORT << G_INT << G_LONG << G_STRING << G_FLOAT << G_DOUBLE << G_DATE << G_CHAR;


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

QStringList Rz::languageMapping(QString type)
{
    if (type == LANG_GOLANG) {
        return golangTypeMap;
    } else if (type == LANG_JAVA) {
        return javaTypeMap;
    } else {
        return QStringList();
    }
}

QString Rz::mapType(QString dbType, QString language, QString dataType)
{
    auto mapping = Ringz::getPreference("mapping").toObject();
    if (dbType == "QMYSQL") {
        QJsonObject mv = mapping[DB_MYSQL].toObject();
        auto types = mv[dataType].toString().split(";");
        for (auto type : types) {
            if (type.startsWith(language)) {
                return type.split(":")[1];
            }
        }
    }
    return "";
}

