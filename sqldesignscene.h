#ifndef SQLDESIGNSCENE_H
#define SQLDESIGNSCENE_H

#include <QGraphicsScene>
#include <QObject>
#include <QWidget>
#include "tableline.h"
#include <QGraphicsSceneDragDropEvent>
#include <QSqlDatabase>
#include <QGraphicsItemGroup>

class SqlDesignScene : public QGraphicsScene
{
    Q_OBJECT
public:
    explicit SqlDesignScene(QSqlDatabase db, QWidget *view, QObject *parent = nullptr);
private:
    enum ItemDataIndex {
        ItemType = 1001
    };
    enum ItemType {
        ItemAnchor = 1001
    };
    enum ViewOperation {
        SeeView,
        DrawLine
    };
    ViewOperation operation;
    QColor tableContentBgColor, tableBorderColor, tableHeaderBgColor,
        anchorBorderColor, anchorBgColor, lineColor;
    QSqlDatabase db;
    QWidget *view;
    QList<TableLine*> lines;
    void createTable(QPointF position, QString table, QStringList columns);
    // QGraphicsScene interface
protected:
    virtual void dragEnterEvent(QGraphicsSceneDragDropEvent *event) override;
    virtual void dropEvent(QGraphicsSceneDragDropEvent *event) override;
    virtual void dragMoveEvent(QGraphicsSceneDragDropEvent *event) override;
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
};

#endif // SQLDESIGNSCENE_H
