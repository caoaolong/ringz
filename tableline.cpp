#include "tableline.h"
#include "ringz.h"

TableLine::TableLine(QGraphicsLineItem *line)
{
    this->line = line;
}

QGraphicsLineItem *TableLine::getLine() const
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
    this->from = from;
    auto startRect = from->mapRectToScene(from->boundingRect());
    QPointF start(startRect.x() + ANCHOR_SIZE / 2, startRect.y() + ANCHOR_SIZE / 2);
    this->start = start;
    this->line->setLine(start.x(), start.y(), start.x(), start.y());
}

void TableLine::updateTo(QGraphicsItem *to)
{
    this->to = to;
    auto stopRect = to->mapRectToScene(to->boundingRect());
    QPointF stop(stopRect.x() + ANCHOR_SIZE / 2, stopRect.y() +  + ANCHOR_SIZE / 2);
    this->stop = stop;
    this->line->setLine(start.x(), start.y(), stop.x(), stop.y());
}

void TableLine::updateDraw(QPointF stop)
{
    this->stop = stop;
    this->line->setLine(start.x(), start.y(), stop.x(), stop.y());
}
