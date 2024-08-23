#ifndef PROJECT_H
#define PROJECT_H

#include "projectitem.h"
#include <QDialog>

namespace Ui {
class Project;
}

class ProjectInfo {
public:
    enum ProjectType {
        MavenProject,
        Unknown
    };
    ProjectInfo(QString dir, ProjectType type = Unknown);
    ProjectItem *getRoot() const;
    ProjectType getType() const;
    static QIcon getIcon(ProjectType type);
    bool getActive() const;
    void setActive(bool newActive);

private:
    bool active;
    QString workspace;
    ProjectItem* root;
    ProjectType type;
    void scan(ProjectItem *parent, QString dir);
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
