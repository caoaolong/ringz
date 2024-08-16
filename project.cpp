#include "project.h"
#include "ui_project.h"
#include <QDir>

Project::Project(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Project)
{
    ui->setupUi(this);
}

Project::~Project()
{
    delete ui;
}

ProjectInfo::ProjectInfo(QString workspace)
{
    this->workspace = workspace;
    this->root = new ProjectItem(workspace.split("/").last(), ProjectItem::Folder);
    this->scan(this->root, workspace);
    this->type = Unknown;

    auto items = this->root->getChildren();
    for (auto item : *items) {
        if (item->getName() == "pom.xml") {
            this->type = MavenProject;
            this->parseMavenProject();
            break;
        }
    }
}

ProjectItem *ProjectInfo::getRoot() const
{
    return root;
}

ProjectInfo::ProjectType ProjectInfo::getType() const
{
    return type;
}

QIcon ProjectInfo::getIcon(ProjectType type)
{
    switch(type) {
    case MavenProject:
        return QIcon(":/ui/logo/maven.png");
    default:
        return QIcon(":/ui/icons/folder.png");
    }
}

void ProjectInfo::scan(ProjectItem *parent, QString workspace)
{
    QDir dir(workspace);
    QFileInfoList files = dir.entryInfoList(QDir::NoDotAndDotDot | QDir::AllEntries, QDir::DirsFirst);
    if (files.length() == 1 && files.at(0).isDir()) {
        auto item = files.at(0);
        parent->setName(parent->getName() + "/" + item.fileName());
        this->scan(parent, item.filePath());
    } else {
        for (auto item: files){
            ProjectItem *pItem = new ProjectItem(item.fileName());
            parent->getChildren()->append(pItem);
            if (item.isDir()) {
                pItem->setType(ProjectItem::Folder);
                this->scan(pItem, item.filePath());
            }
        }
    }
}

void ProjectInfo::parseMavenProject()
{
}
