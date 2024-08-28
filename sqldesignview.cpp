#include "sqldesignview.h"
#include "ui_sqldesignview.h"

SqlDesignView::SqlDesignView(QSqlDatabase db, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::SqlDesignView)
{
    ui->setupUi(this);
    ui->graphicsView->setDb(db);
    ui->graphicsView->initTheme();
}

SqlDesignView::~SqlDesignView()
{
    delete ui;
}

void SqlDesignView::showEvent(QShowEvent *event)
{
    int w = ui->graphicsView->width() - 10;
    int h = ui->graphicsView->height() - 10;
    QRectF view(-w/2, -h/2, w, h);
    ui->graphicsView->setSceneRect(view);
    ui->width->setValue(ui->graphicsView->width());
    ui->height->setValue(ui->graphicsView->height());
    event->accept();
}

void SqlDesignView::on_width_valueChanged(int w)
{
    auto rect = ui->graphicsView->sceneRect();
    rect.setWidth(w);
    rect.setLeft(-w / 2);
    ui->graphicsView->setSceneRect(rect);
}

void SqlDesignView::on_height_valueChanged(int h)
{
    auto rect = ui->graphicsView->sceneRect();
    rect.setHeight(h);
    rect.setTop(-h / 2);
    ui->graphicsView->setSceneRect(rect);
}
