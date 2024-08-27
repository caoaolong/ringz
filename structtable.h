#ifndef STRUCTTABLE_H
#define STRUCTTABLE_H

#include <QDialog>
#include <QSqlDatabase>

namespace Ui {
class StructTable;
}

class StructTable : public QDialog
{
    Q_OBJECT

public:
    explicit StructTable(QWidget *parent = nullptr);
    ~StructTable();
    static bool showTable(QWidget *parent, QSqlDatabase db, QString table);

private slots:
    void on_selectAll_clicked();

    void on_invert_clicked();

private:
    Ui::StructTable *ui;
};

#endif // STRUCTTABLE_H
