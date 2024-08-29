#ifndef SQLDESIGNVIEW_H
#define SQLDESIGNVIEW_H

#include <QWidget>
#include <QSqlDatabase>
#include <QGraphicsScene>
#include "sqldesignscene.h"

namespace Ui {
class SqlDesignView;
}

class SqlDesignView : public QWidget
{
    Q_OBJECT

public:
    explicit SqlDesignView(QSqlDatabase db, QWidget *parent = nullptr);
    ~SqlDesignView();

private:
    Ui::SqlDesignView *ui;
    SqlDesignScene *scene;
};

#endif // SQLDESIGNVIEW_H
