#include "projectitem.h"

ProjectItem::ProjectItem(QString name, ProjectItemType type)
{
    this->name = name;
    this->type = type;
    this->children = new QList<ProjectItem*>();
}

QList<ProjectItem*> *ProjectItem::getChildren() const
{
    return children;
}

QString ProjectItem::getName() const
{
    return name;
}

ProjectItemType ProjectItem::getType() const
{
    return type;
}

void ProjectItem::setType(ProjectItemType newType)
{
    type = newType;
}

void ProjectItem::setName(const QString &newName)
{
    name = newName;
}
