#include "dragtreewidget.h"
#include "ringz.h"
#include <QMimeData>
#include <QDrag>

DragTreeWidget::DragTreeWidget(QWidget *parent)
    :QTreeWidget(parent)
{
    setDragEnabled(true);
    setDragDropMode(QAbstractItemView::DragOnly);
}

void DragTreeWidget::startDrag(Qt::DropActions supportedActions)
{
    auto item = currentItem();
    if (item->type() == TableItem) {
        QMimeData *mime = new QMimeData();
        mime->setData("table", item->text(ColumnLabel).toUtf8());
        QDrag *drag = new QDrag(this);
        drag->setMimeData(mime);
        drag->exec(supportedActions);
    }
}
