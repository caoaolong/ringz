#ifndef BUILDMODEL_H
#define BUILDMODEL_H

#include "project.h"
#include "buildinfo.h"
#include <QDialog>
#include <QSqlDatabase>

namespace Ui {
class BuildModel;
}

class BuildModel : public QDialog
{
    Q_OBJECT

public:
    explicit BuildModel(QWidget *parent = nullptr);
    ~BuildModel();

    static bool showTable(QWidget *parent, QSqlDatabase db, QString table, QList<ProjectInfo*> *projects);
    static QString getName(bool firstUpper, QString name);
    bool build(BuildInfo *info);
private:
    Ui::BuildModel *ui;
};

#endif // BUILDMODEL_H
