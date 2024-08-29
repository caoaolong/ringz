#include "sqldesignview.h"
#include "ui_sqldesignview.h"
#include <QGraphicsItem>

SqlDesignView::SqlDesignView(QSqlDatabase db, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::SqlDesignView)
{
    ui->setupUi(this);
    this->scene = new SqlDesignScene(db, this, this);
    scene->setSceneRect(-400, -300, 800, 600);
    ui->graphicsView->setScene(this->scene);
    ui->graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

SqlDesignView::~SqlDesignView()
{
    delete ui;
}
