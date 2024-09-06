#ifndef TABLELINE_H
#define TABLELINE_H
#include <QGraphicsPathItem>
#include <QPainterPath>

class TableLine
{
public:
    TableLine(QGraphicsPathItem *line);
    QGraphicsItem *getSource() { return from; };
    QGraphicsItem *getTarget() { return to; };
    QString getFromName() { return fromName; };
    QString getToName() { return toName; };
    QGraphicsItemGroup *getFrom() { return from->group(); };
    QGraphicsItemGroup *getTo() { return to->group(); };
    QGraphicsPathItem *getLine() const;

    void update();
    void updateFrom(QGraphicsItem *from);
    void updateTo(QGraphicsItem *to);
    void updateDraw(QPointF stop);
private:
    QPointF start, stop;
    QGraphicsItem *from, *to;
    QString fromName, toName;
    QGraphicsPathItem *line;
    QPainterPath path;
    void draw(QPointF stop);
};

#endif // TABLELINE_H
