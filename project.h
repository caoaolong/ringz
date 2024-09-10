#ifndef PROJECT_H
#define PROJECT_H

#include "projectitem.h"
#include <QDialog>

namespace Ui {
class Project;
}

enum ProjectType {
    MavenProject,
    Unknown
};

class ProjectInfo {
public:  
    ProjectInfo(QString dir, ProjectType type = Unknown);
    ProjectItem *getRoot() const;
    ProjectType getType() const;
    QString getWorkspace() { return workspace; };
    QString getLanguage();
    static QIcon getIcon(ProjectType type);
    QIcon getIcon();
    bool getActive() const;
    void setActive(bool newActive);
private:
    bool active;
    QString workspace;
    ProjectItem* root;
    ProjectType type;
    void scan(ProjectItem *parent, QString dir);
    void scanPackage();
    // parse project structure
    void parseMavenProject();
};

class Project : public QDialog
{
    Q_OBJECT

public:
    explicit Project(QWidget *parent = nullptr);
    ~Project();
private:
    Ui::Project *ui;
};

#endif // PROJECT_H
