#ifndef SQLDESIGNWIDGET_H
#define SQLDESIGNWIDGET_H

#include <QGraphicsView>
#include <QObject>
#include <QDragEnterEvent>
#include <QSqlDatabase>

class SqlDesignWidget : public QGraphicsView
{
    Q_OBJECT
public:
    SqlDesignWidget(QWidget *parent = nullptr);

    // QWidget interface
    void setDb(const QSqlDatabase &newDb);

protected:
    virtual void dropEvent(QDropEvent *event) override;
    virtual void dragEnterEvent(QDragEnterEvent *event) override;
private:
    QSqlDatabase db;
};

#endif // SQLDESIGNWIDGET_H
