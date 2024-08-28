#ifndef SQLDESIGNWIDGET_H
#define SQLDESIGNWIDGET_H

#include <QGraphicsView>
#include <QObject>
#include <QDragEnterEvent>
#include <QSqlDatabase>
#include <QPen>
#include <QBrush>

class SqlDesignWidget : public QGraphicsView
{
    Q_OBJECT
public:
    SqlDesignWidget(QWidget *parent = nullptr);
    void setDb(const QSqlDatabase &newDb);
    void initTheme();
    // QWidget interface
protected:
    virtual void dropEvent(QDropEvent *event) override;
    virtual void dragEnterEvent(QDragEnterEvent *event) override;
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;
    virtual void mouseDoubleClickEvent(QMouseEvent *event) override;
    virtual void mouseMoveEvent(QMouseEvent *event) override;
    virtual void keyPressEvent(QKeyEvent *event) override;
    virtual void keyReleaseEvent(QKeyEvent *event) override;
    virtual void wheelEvent(QWheelEvent *event) override;
private:
    QSqlDatabase db;

    QColor borderColor, textColor, headerColor, contentColor;
    enum State {
        Moving,
        None
    };
    enum MouseState {
        MousePressed,
        MouseReleased
    };

    State state = None;
    MouseState mState = MouseReleased;
    QPointF startPoint;
    qreal scaler = 1.0f;
    void moveViewport(QPoint point);
    void initColors();

    void createTable(QPointF position, QString table, QStringList columns);
};

#endif // SQLDESIGNWIDGET_H
