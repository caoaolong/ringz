#include "sqldesignview.h"
#include "ui_sqldesignview.h"

SqlDesignView::SqlDesignView(QSqlDatabase db, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::SqlDesignView)
{
    ui->setupUi(this);
    ui->graphicsView->setDb(db);
}

SqlDesignView::~SqlDesignView()
{
    delete ui;
}

void SqlDesignView::initTheme()
{

}
