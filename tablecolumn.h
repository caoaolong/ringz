#ifndef TABLECOLUMN_H
#define TABLECOLUMN_H

#include <QString>

class TableColumn
{
public:
    TableColumn(QString name);
    TableColumn(QString name, QString type);
    TableColumn(QString name, QString type, QString comment);
    QString getName() const;

    QString getType() const;
    QString getCollation() const;
    bool getCanNull() const;
    QString getKey() const;
    QString getDefaultValue() const;
    QString getPrivileges() const;
    QString getComment() const;

    void setName(const QString &newName);
    void setType(const QString &newType);
    void setCollation(const QString &newCollation);
    void setCanNull(bool newCanNull);
    void setKey(const QString &newKey);
    void setDefaultValue(const QString &newDefaultValue);
    void setPrivileges(const QString &newPrivileges);
    void setComment(const QString &newComment);

private:
    QString name;
    QString type;
    QString collation;
    bool canNull;
    QString key;
    QString defaultValue;
    QString privileges;
    QString comment;
};

#endif // TABLECOLUMN_H
