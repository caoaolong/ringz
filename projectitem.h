#ifndef PROJECTITEM_H
#define PROJECTITEM_H

#include <QString>
#include <QList>

enum ProjectItemType {
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
    ProjectItem(QString name, ProjectItemType type = File);
    QList<ProjectItem*> *getChildren() const;

    QString getName() const;

    ProjectItemType getType() const;
    void setType(ProjectItemType newType);

    void setName(const QString &newName);

private:
    ProjectItemType type;
    QString name;
    QList<ProjectItem*>* children;
};

#endif // PROJECTITEM_H
