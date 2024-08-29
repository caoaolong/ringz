#ifndef TABLELINE_H
#define TABLELINE_H
#include <QGraphicsLineItem>

class TableLine
{
public:
    TableLine(QGraphicsLineItem *line);
    QGraphicsItemGroup *getFrom() { return from->group(); };
    QGraphicsItemGroup *getTo() { return to->group(); };
    QGraphicsLineItem *getLine() const;

    void update();
    void updateFrom(QGraphicsItem *from);
    void updateTo(QGraphicsItem *to);
    void updateDraw(QPointF stop);
private:
    QPointF start, stop;
    QGraphicsItem *from, *to;
    QGraphicsLineItem *line;
};

#endif // TABLELINE_H
