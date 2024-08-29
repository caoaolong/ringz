#ifndef SQLDESIGNEDITOR_H
#define SQLDESIGNEDITOR_H

#include <QGraphicsView>
#include <QObject>
#include <QWidget>
#include <QMouseEvent>

class SqlDesignEditor : public QGraphicsView
{
    Q_OBJECT
public:
    SqlDesignEditor(QWidget *parent = nullptr);
private:
    QPoint lastPoint;
    qreal factor = 1.0f;
    enum ViewOperation {
        ViewNull,
        ViewMove,
        ViewSelect
    };
    ViewOperation operation;
    DragMode dm;
    // QWidget interface
protected:
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;
    virtual void mouseMoveEvent(QMouseEvent *event) override;
    virtual void wheelEvent(QWheelEvent *event) override;

    // QWidget interface
protected:
    virtual void keyPressEvent(QKeyEvent *event) override;
    virtual void keyReleaseEvent(QKeyEvent *event) override;
};

#endif // SQLDESIGNEDITOR_H
