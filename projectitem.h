#ifndef PROJECTITEM_H
#define PROJECTITEM_H

#include <QString>
#include <QList>

enum ItemType {
    Folder = 1001,
    SourceFolder,
    ResourceFolder,
    TestFolder,
    PackageFolder,
    File
};

class ProjectItem
{
public:
    ProjectItem(QString name, ItemType type = File);
    QList<ProjectItem*> *getChildren() const;

    QString getName() const;

    ItemType getType() const;
    void setType(ItemType newType);

    void setName(const QString &newName);

private:
    ItemType type;
    QString name;
    QList<ProjectItem*>* children;
};

#endif // PROJECTITEM_H
