#include "sqldesigneditor.h"

SqlDesignEditor::SqlDesignEditor(QWidget *parent)
    :QGraphicsView(parent)
{
    setAcceptDrops(true);
    setMouseTracking(true);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    setResizeAnchor(QGraphicsView::AnchorUnderMouse);
    setRenderHint(QPainter::Antialiasing);
    this->dm = dragMode();
}

void SqlDesignEditor::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        if (operation == ViewMove) {
            lastPoint = event->pos();
        }
    }
    QGraphicsView::mousePressEvent(event);
}

void SqlDesignEditor::mouseReleaseEvent(QMouseEvent *event)
{
    QGraphicsView::mouseReleaseEvent(event);
}

void SqlDesignEditor::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton) {
        if (operation == ViewMove) {
            QPoint delta = (event->pos() - lastPoint) * (1.0 / factor);
            QRectF sceneRect = this->sceneRect();
            sceneRect.translate(-delta);
            this->setSceneRect(sceneRect);
            ensureVisible(sceneRect);
            lastPoint = event->pos();
        }
    }

    QGraphicsView::mouseMoveEvent(event);
}

void SqlDesignEditor::wheelEvent(QWheelEvent *event)
{
    QPoint scrollAmount = event->angleDelta();
    double factor = 1.15;
    if (scrollAmount.y() > 0) {
        scale(factor, factor);
        this->factor *= factor;
    }
    else if (scrollAmount.y() < 0) {
        scale(1.0 / factor, 1.0 / factor);
        this->factor *= (1.0 / factor);
    }
    centerOn(event->position());
    event->accept();
}

void SqlDesignEditor::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Space:
        this->operation = ViewMove;
        setCursor(Qt::OpenHandCursor);
        break;
    case Qt::Key_Shift:
        this->operation = ViewSelect;
        setCursor(Qt::CrossCursor);
        this->setDragMode(DragMode::RubberBandDrag);
        break;
    }
}

void SqlDesignEditor::keyReleaseEvent(QKeyEvent *event)
{
    switch(event->key()) {
    case Qt::Key_Shift:
        this->setDragMode(this->dm);
        break;
    }
    this->operation = ViewNull;
    unsetCursor();
}
