#include "tableview.h"
#include "ui_tableview.h"
#include "ringz.h"
#include "rz.h"
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QMessageBox>
#include <QSqlRecord>

TableView::TableView(QSqlDatabase db, QString table, ProjectInfo *project, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::TableView)
{
    ui->setupUi(this);
    this->initTheme();

    this->model = new QSqlQueryModel(this);
    this->model->setQuery(QString("desc %1").arg(table));
    auto err = model->lastError();
    if(err.isValid()) {
        QMessageBox::warning(this, "错误", err.text());
        return;
    }
    this->selectionModel = new QItemSelectionModel(model, this);
    ui->tableView->setModel(model);
    ui->tableView->setSelectionModel(selectionModel);

    QSqlRecord record = model->record();
    model->setHeaderData(record.indexOf("Field"), Qt::Horizontal, "字段");
    model->setHeaderData(record.indexOf("Type"), Qt::Horizontal, "数据类型");
    model->setHeaderData(record.indexOf("Null"), Qt::Horizontal, "是否可以为空");
    model->setHeaderData(record.indexOf("Key"), Qt::Horizontal, "键");
    model->setHeaderData(record.indexOf("Default"), Qt::Horizontal, "默认值");
    model->setHeaderData(record.indexOf("Extra"), Qt::Horizontal, "拓展属性");
}

TableView::~TableView()
{
    delete ui;
}

void TableView::initTheme()
{
    // 设置背景前景色
    QPalette pallete = ui->tableView->palette();
    auto colors = Ringz::getTheme("colors");
    pallete.setColor(QPalette::Base, QColor(colors["terminal.background"].toString()));
    pallete.setColor(QPalette::Text, QColor(colors["terminal.foreground"].toString()));
    ui->tableView->setPalette(pallete);
}
