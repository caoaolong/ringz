#include "tablecolumn.h"

TableColumn::TableColumn(QString name)
{
    this->name = name;
}

TableColumn::TableColumn(QString name, QString type)
    :TableColumn(name)
{
    this->type = type;
}

TableColumn::TableColumn(QString name, QString type, QString comment)
    :TableColumn(name, type)
{
    this->comment = comment;
}

QString TableColumn::getName() const
{
    return name;
}

QString TableColumn::getType() const
{
    return type;
}

QString TableColumn::getCollation() const
{
    return collation;
}

bool TableColumn::getCanNull() const
{
    return canNull;
}

QString TableColumn::getKey() const
{
    return key;
}

QString TableColumn::getDefaultValue() const
{
    return defaultValue;
}

QString TableColumn::getPrivileges() const
{
    return privileges;
}

QString TableColumn::getComment() const
{
    return comment;
}

void TableColumn::setName(const QString &newName)
{
    name = newName;
}

void TableColumn::setType(const QString &newType)
{
    type = newType;
}

void TableColumn::setCollation(const QString &newCollation)
{
    collation = newCollation;
}

void TableColumn::setCanNull(bool newCanNull)
{
    canNull = newCanNull;
}

void TableColumn::setKey(const QString &newKey)
{
    key = newKey;
}

void TableColumn::setDefaultValue(const QString &newDefaultValue)
{
    defaultValue = newDefaultValue;
}

void TableColumn::setPrivileges(const QString &newPrivileges)
{
    privileges = newPrivileges;
}

void TableColumn::setComment(const QString &newComment)
{
    comment = newComment;
}
