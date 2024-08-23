#include "sqldesignview.h"
#include "ui_sqldesignview.h"

SqlDesignView::SqlDesignView(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::SqlDesignView)
{
    ui->setupUi(this);
}

SqlDesignView::~SqlDesignView()
{
    delete ui;
}

void SqlDesignView::initTheme()
{

}
