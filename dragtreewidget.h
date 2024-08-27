#ifndef DRAGTREEWIDGET_H
#define DRAGTREEWIDGET_H

#include <QObject>
#include <QTreeWidget>
#include <QWidget>

class DragTreeWidget : public QTreeWidget
{
    Q_OBJECT
public:
    DragTreeWidget(QWidget *parent = nullptr);
    // QAbstractItemView interface
protected:
    virtual void startDrag(Qt::DropActions supportedActions) override;
};

#endif // DRAGTREEWIDGET_H
