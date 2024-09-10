#include "project.h"
#include "rz.h"
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

ProjectInfo::ProjectInfo(QString workspace, ProjectType type)
{
    this->workspace = workspace;
    this->root = new ProjectItem(workspace.split("/").last(), Folder);
    this->scan(this->root, workspace);
    if (type == Unknown) {
        auto items = this->root->getChildren();
        for (auto item : *items) {
            if (item->getName() == "pom.xml") {
                this->type = MavenProject;
                this->parseMavenProject();
                break;
            }
        }
    } else {
        this->type = type;
    }
}

ProjectItem *ProjectInfo::getRoot() const
{
    return root;
}

ProjectType ProjectInfo::getType() const
{
    return type;
}

QString ProjectInfo::getLanguage()
{
    switch(type) {
    case MavenProject:
        return LANG_JAVA;
    default:
        return "";
    }
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

QIcon ProjectInfo::getIcon()
{
    return getIcon(this->type);
}

bool ProjectInfo::getActive() const
{
    return active;
}

void ProjectInfo::setActive(bool newActive)
{
    active = newActive;
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
                pItem->setType(Folder);
                this->scan(pItem, item.filePath());
            }
        }
    }
}

void ProjectInfo::parseMavenProject()
{
}
