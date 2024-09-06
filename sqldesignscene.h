#ifndef SQLDESIGNSCENE_H
#define SQLDESIGNSCENE_H

#include <QGraphicsScene>
#include <QObject>
#include <QWidget>
#include "tableline.h"
#include <QGraphicsSceneDragDropEvent>
#include <QSqlDatabase>
#include <QGraphicsItemGroup>

enum ItemDataIndex {
    ItemType = 1001,
    ItemColumn,
    ItemName,
    ItemData,
    ItemPosition
};
enum ViewItemType {
    ItemAnchor = 1001,
    ItemTable,
    ItemColumns,
    ItemEntry,
    ItemLine
};
enum ViewOperation {
    SeeView,
    DrawLine
};

enum ViewItemPosition {
    ItemCenter,
    ItemLeft,
    ItemRight,
    ItemTop,
    ItemBottom
};

struct TableRect {
    QRect headerRect;
    QRect bodyRect;
    QFont hf;
    QFont cf;
    int width;
};

class SqlDesignScene : public QGraphicsScene
{
    Q_OBJECT
public:
    explicit SqlDesignScene(QSqlDatabase db, QWidget *view, QObject *parent = nullptr);
private:
    QPen defaultLinePen, activeLinePen;

    ViewOperation operation;
    QColor tableContentBgColor, tableBorderColor, tableHeaderBgColor,
        anchorBorderColor, anchorBgColor, lineColor, lineActiveColor;
    QSqlDatabase db;
    QWidget *view;
    QList<TableLine*> lines;
    QMap<QGraphicsItemGroup*, TableRect> rects;
    TableRect tableRect(QPointF position, QString table, QStringList columns);
    void createTable(QPointF position, QString table, QStringList columns);
    void editTable(QGraphicsItemGroup *group, QString table, QStringList columns);
    TableLine* line(QGraphicsEllipseItem *anchor, int *type);
    void setLine(QGraphicsEllipseItem *anchor, TableLine* line, int lineType);
    void appendColumn(QGraphicsItemGroup *group, QRect bodyRect, QString item, int i,
                      QBrush anchorBg, QPen anchorBorder, QFont cf);
    QGraphicsItemGroup *appendTable(QGraphicsItemGroup *group, TableRect tr, QString table, QStringList columns);
    // QGraphicsScene interface
protected:
    virtual void dragEnterEvent(QGraphicsSceneDragDropEvent *event) override;
    virtual void dropEvent(QGraphicsSceneDragDropEvent *event) override;
    virtual void dragMoveEvent(QGraphicsSceneDragDropEvent *event) override;
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;
};

#endif // SQLDESIGNSCENE_H
