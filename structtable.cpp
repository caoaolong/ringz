#include "structtable.h"
#include "ui_structtable.h"
#include "tablecolumn.h"
#include "rz.h"
#include <QTableWidget>
#include <QTableWidgetItem>

StructTable::StructTable(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::StructTable)
{
    ui->setupUi(this);
}

StructTable::~StructTable()
{
    delete ui;
}

bool StructTable::showTable(QWidget *parent, QSqlDatabase db, QString table, QStringList *selectedcolumns)
{
    StructTable dialog(parent);
    QTableWidget *tw = dialog.ui->tableWidget;
    tw->setColumnCount(3);
    tw->setHorizontalHeaderLabels(QStringList("名称") << "类型" << "备注");
    // 查询表结构
    QList<TableColumn> columns = Rz::tableDesc(db, table);
    tw->setRowCount(columns.count());
    for (int i = 0; i < columns.count(); i++) {
        auto item = columns.at(i);
        QTableWidgetItem *checkbox = new QTableWidgetItem(item.getName());
        checkbox->setFlags(checkbox->flags() | Qt::ItemIsUserCheckable);
        checkbox->setFlags(checkbox->flags() & ~Qt::ItemIsEditable);
        checkbox->setCheckState(Qt::Checked);
        tw->setItem(i, 0, checkbox);
        tw->setItem(i, 1, new QTableWidgetItem(item.getType()));
        tw->setItem(i, 2, new QTableWidgetItem(item.getComment()));
    }
    tw->horizontalHeader()->setStretchLastSection(true);
    bool result = dialog.exec() == QDialog::Accepted;
    if (result) {
        for (int i = 0; i < tw->rowCount(); i++) {
            if (tw->item(i, 0)->checkState() == Qt::Checked) {
                selectedcolumns->append(tw->item(i, 0)->text());
            }
        }
    }
    return result;
}


void StructTable::on_selectAll_clicked()
{
    int rows = ui->tableWidget->rowCount();
    for (int i = 0; i < rows; i++) {
        auto item = ui->tableWidget->item(i, 0);
        if (item->flags() & Qt::ItemIsUserCheckable) {
            item->setCheckState(Qt::Checked);
        }
    }
}


void StructTable::on_invert_clicked()
{
    int rows = ui->tableWidget->rowCount();
    for (int i = 0; i < rows; i++) {
        auto item = ui->tableWidget->item(i, 0);
        if (item->flags() & Qt::ItemIsUserCheckable) {
            if (item->checkState() == Qt::Checked) {
                item->setCheckState(Qt::Unchecked);
            } else {
                item->setCheckState(Qt::Checked);
            }
        }
    }
}
