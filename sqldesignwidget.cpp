#include "sqldesignwidget.h"
#include "structtable.h"
#include "ringz.h"
#include "rz.h"
#include <QMimeData>
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QScrollBar>
#include <QApplication>

SqlDesignWidget::SqlDesignWidget(QWidget *parent)
    :QGraphicsView(parent) {
    setAcceptDrops(true);
    QGraphicsScene *scene = new QGraphicsScene(this);
    setScene(scene);
    this->initTheme();
    this->initColors();
    // 禁止滚动条
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    this->setResizeAnchor(QGraphicsView::AnchorUnderMouse);

    createTable(QPointF(0, 0), "test", QStringList("字段1") << "字段2");
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

void SqlDesignWidget::mousePressEvent(QMouseEvent *event)
{
    switch (state) {
    case Moving:
        startPoint = event->pos();
        break;
    default:
        break;
    }
    mState = MousePressed;
    event->accept();
    QGraphicsView::mousePressEvent(event);
}

void SqlDesignWidget::mouseReleaseEvent(QMouseEvent *event)
{
    mState = MouseReleased;
    event->accept();
    QGraphicsView::mouseReleaseEvent(event);
}

void SqlDesignWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    event->accept();
    QGraphicsView::mouseDoubleClickEvent(event);
}

void SqlDesignWidget::mouseMoveEvent(QMouseEvent *event)
{
    switch (state) {
    case Moving:
        moveViewport(event->pos());
        break;
    default:
        break;
    }
    event->accept();
    QGraphicsView::mouseMoveEvent(event);
}

void SqlDesignWidget::moveViewport(QPoint point)
{
    if (mState == MousePressed) {
        auto df = point.toPointF() - startPoint;
        auto rect = this->sceneRect();
        rect.translate(-df);
        setSceneRect(rect);
        ensureVisible(rect);
        startPoint = point.toPointF();
    }
}

void SqlDesignWidget::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Space) {
        state = Moving;
        setCursor(QCursor(Qt::OpenHandCursor));
    }
}

void SqlDesignWidget::keyReleaseEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Space) {
        state = None;
        unsetCursor();
    }
}

void SqlDesignWidget::wheelEvent(QWheelEvent *event)
{
    const double factor = 1.1;
    // 根据滚动方向决定缩放
    if (event->angleDelta().y() > 0) {
        // 向上滚动，放大
        scale(factor, factor);
    } else {
        // 向下滚动，缩小
        scale(1.0 / factor, 1.0 / factor);
    }
    event->accept();
}

void SqlDesignWidget::setDb(const QSqlDatabase &newDb)
{
    db = newDb;
}

void SqlDesignWidget::initTheme()
{
    auto colors = Ringz::getTheme("colors");
    QBrush bg(QColor(colors["editor.background"].toString()));
    scene()->setBackgroundBrush(bg);
}

void SqlDesignWidget::initColors()
{
    borderColor = QColor(Rz::tokenColor("keyword"));
    textColor = QColor(Rz::tokenColor("meta.embedded"));
    contentColor = QColor(Rz::tokenColor("keyword.operator"));
    headerColor = QColor(Rz::tokenColor("variable.language"));
}

void SqlDesignWidget::createTable(QPointF position, QString table, QStringList columns)
{
    QPainterPath path;
    auto font = QApplication::font();
    font.setBold(false);
    font.setPixelSize(30);
    path.addText(position, font, table);
    path.addRoundedRect(position.x(), position.y(), 100, 80, 4, 4);
    QGraphicsPathItem *item = new QGraphicsPathItem(path);
    item->setBrush(QBrush(headerColor));
    item->setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable);
    scene()->addItem(item);
}
