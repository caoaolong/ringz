#include "queryresult.h"
#include "rz.h"
#include "ringz.h"
#include <QMessageBox>
#include <QSqlError>

QueryResult::QueryResult(QWidget *parent)
    :QTableView(parent)
{
    this->initTheme();
}

QueryResult::~QueryResult()
{
    delete model;
    delete selectionModel;
}

void QueryResult::show(QString sql)
{
    model = new QSqlQueryModel(this);
    model->setQuery(sql);
    if (model->lastError().isValid()) {
        QMessageBox::warning(this, "错误", model->lastError().text());
        return;
    }
    setModel(model);
    selectionModel = new QItemSelectionModel(model, this);
    setSelectionModel(selectionModel);
}

void QueryResult::initTheme()
{
    // 设置背景前景色
    QPalette pallete = this->palette();
    auto colors = Ringz::getTheme("colors");
    pallete.setColor(QPalette::Base, QColor(colors["terminal.background"].toString()));
    pallete.setColor(QPalette::Text, QColor(colors["terminal.foreground"].toString()));
    this->setPalette(pallete);
}
