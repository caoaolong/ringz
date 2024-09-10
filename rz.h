#ifndef RZ_H
#define RZ_H

#include "tablecolumn.h"
#include <QColor>
#include <QFont>
#include <QSqlDatabase>
#include <QList>

#define J_BOOL      "boolean"
#define J_BYTE      "byte"
#define J_SHORT     "short"
#define J_INT       "int"
#define J_LONG      "long"
#define J_STRING    "String"
#define J_FLOAT     "float"
#define J_DOUBLE    "double"
#define J_DATE      "Date"
#define J_CHAR      "char"

#define G_BOOL      "boolean"
#define G_BYTE      "int8"
#define G_SHORT     "int16"
#define G_INT       "int32"
#define G_LONG      "int64"
#define G_STRING    "string"
#define G_FLOAT     "float32"
#define G_DOUBLE    "float64"
#define G_DATE      "time.Time"
#define G_CHAR      "rune"

#define LANG_JAVA       "Java"
#define LANG_GOLANG     "Golang"

#define DB_MYSQL        "MySQL"

class Rz
{
public:
    Rz();
    static bool parseBool(QString value);
    static QFont parseFont(QString fontValue);
    static QList<TableColumn> tableDesc(QSqlDatabase db, QString table);
    static QString tokenColor(QString name);
    static QStringList javaTypeMap, golangTypeMap;
    static QStringList languageMapping(QString type);
    static QString mapType(QString dbType, QString language, QString dataType);
};

#endif // RZ_H
