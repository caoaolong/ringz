#ifndef DATAVIEW_H
#define DATAVIEW_H

#include <QWidget>
#include <QSqlDatabase>
#include <QSqlRecord>
#include <QSqlTableModel>
#include <QItemSelectionModel>

namespace Ui {
class DataView;
}

class DataView : public QWidget
{
    Q_OBJECT

public:
    explicit DataView(QSqlDatabase db, QString table, QWidget *parent = nullptr);
    ~DataView();

private slots:
    void on_refresh_clicked();

    void on_insert_clicked();

    void on_remove_clicked();

private:
    Ui::DataView *ui;
    QSqlDatabase db;
    QSqlTableModel *model;
    QItemSelectionModel *selectionModel;
    int newRecordCount = 0;
    void load();
    void initTheme();
};

#endif // DATAVIEW_H
