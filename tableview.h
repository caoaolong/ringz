#ifndef TABLEVIEW_H
#define TABLEVIEW_H

#include "project.h"
#include <QWidget>
#include <QSqlDatabase>
#include <QSqlQueryModel>
#include <QItemSelectionModel>

namespace Ui {
class TableView;
}

class TableView : public QWidget
{
    Q_OBJECT

public:
    explicit TableView(QSqlDatabase db, QString table, ProjectInfo *project, QWidget *parent = nullptr);
    ~TableView();

private:
    Ui::TableView *ui;
    QSqlQueryModel *model;
    QItemSelectionModel *selectionModel;
    void initTheme();
};

#endif // TABLEVIEW_H
