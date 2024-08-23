#include "projectitem.h"

ProjectItem::ProjectItem(QString name, ItemType type)
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

ItemType ProjectItem::getType() const
{
    return type;
}

void ProjectItem::setType(ItemType newType)
{
    type = newType;
}

void ProjectItem::setName(const QString &newName)
{
    name = newName;
}
