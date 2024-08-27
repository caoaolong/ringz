#include "sqldesignwidget.h"
#include "structtable.h"
#include <QMimeData>

SqlDesignWidget::SqlDesignWidget(QWidget *parent)
    :QGraphicsView(parent) {
    setAcceptDrops(true);
}

void SqlDesignWidget::dropEvent(QDropEvent *event)
{
    auto mime = event->mimeData();
    QString table = mime->data("table");
    if (StructTable::showTable(this, this->db, table)) {
        qDebug() << table;
    }
    QGraphicsView::dropEvent(event);
}

void SqlDesignWidget::dragEnterEvent(QDragEnterEvent *event)
{
    event->accept();
    QGraphicsView::dragEnterEvent(event);
}

void SqlDesignWidget::setDb(const QSqlDatabase &newDb)
{
    db = newDb;
}
