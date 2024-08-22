#include "dataview.h"
#include "ui_dataview.h"
#include "ringz.h"
#include "rz.h"
#include <QSqlTableModel>
#include <QMessageBox>
#include <QSqlError>
#include <QSqlRecord>

DataView::DataView(QSqlDatabase db, QString table, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::DataView)
{
    ui->setupUi(this);
    this->initTheme();

    this->db = db;
    this->model = new QSqlTableModel(this, db);
    this->selectionModel = new QItemSelectionModel(this->model, this);
    model->setTable(table);
    ui->tableView->setModel(model);
    this->load();
}

DataView::~DataView()
{
    delete ui;
}


void DataView::on_refresh_clicked()
{
    this->load();
}

void DataView::load()
{
    if (!model->select()) {
        QMessageBox::warning(this, "错误", model->lastError().text());
        return;
    }
}

void DataView::initTheme()
{
    // 设置背景前景色
    QPalette pallete = ui->tableView->palette();
    auto colors = Ringz::getTheme("colors");
    pallete.setColor(QPalette::Base, Rz::parseColor(colors["terminal.background"].toString()));
    pallete.setColor(QPalette::Text, Rz::parseColor(colors["terminal.foreground"].toString()));
    ui->tableView->setPalette(pallete);
}

void DataView::on_insert_clicked()
{
    QModelIndex index = ui->tableView->currentIndex();
    model->insertRow(index.row());
    selectionModel->clearSelection();
    selectionModel->setCurrentIndex(index, QItemSelectionModel::Select);
}


void DataView::on_remove_clicked()
{
    QModelIndex index = ui->tableView->currentIndex();
    model->removeRow(index.row());
    this->load();
}
