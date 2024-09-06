#include "tableline.h"
#include "sqldesignscene.h"
#include "ringz.h"

#define ANCHOR_SPACE    50

TableLine::TableLine(QGraphicsPathItem *line)
{
    this->line = line;
    this->line->setData(ItemType, ItemLine);
    this->line->setPath(path);
}

QGraphicsPathItem *TableLine::getLine() const
{
    return line;
}

void TableLine::update()
{
    if (!this->from || !this->to) return;
    updateFrom(this->from);
    updateTo(this->to);
}

void TableLine::updateFrom(QGraphicsItem *from)
{
    this->path.clear();
    this->from = from;
    this->fromName = from->data(ItemColumn).toString();
    auto startRect = from->mapRectToScene(from->boundingRect());
    QPointF start(startRect.x() + ANCHOR_SIZE / 2, startRect.y() + ANCHOR_SIZE / 2);
    this->start = start;
}

void TableLine::updateTo(QGraphicsItem *to)
{
    this->to = to;
    this->toName = to->data(ItemColumn).toString();
    auto stopRect = to->mapRectToScene(to->boundingRect());
    QPointF stop(stopRect.x() + ANCHOR_SIZE / 2, stopRect.y() +  + ANCHOR_SIZE / 2);
    this->stop = stop;
    this->draw(stop);
}

void TableLine::updateDraw(QPointF stop)
{
    this->draw(stop);
}

void TableLine::draw(QPointF stop)
{
    this->path.clear();
    this->stop = stop;
    QPointF c1(this->start.x(), this->start.y());
    QPointF c2(this->stop.x(), this->stop.y());
    this->path.moveTo(start);
    auto position = this->from->data(ItemPosition);
    if (position == ItemLeft) {
        c1.setX(c1.x() - ANCHOR_SPACE);
    } else if (position == ItemRight) {
        c1.setX(c1.x() + ANCHOR_SPACE);
    }
    position = this->to->data(ItemPosition);
    if (position == ItemLeft) {
        c2.setX(c2.x() - ANCHOR_SPACE);
    } else if (position == ItemRight) {
        c2.setX(c2.x() + ANCHOR_SPACE);
    }
    this->path.cubicTo(c1, c2, stop);
    this->line->setPath(this->path);
}
