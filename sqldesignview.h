#ifndef SQLDESIGNVIEW_H
#define SQLDESIGNVIEW_H

#include <QWidget>
#include <QSqlDatabase>

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
    // QWidget interface
protected:
    virtual void showEvent(QShowEvent *event) override;
private slots:
    void on_width_valueChanged(int w);
    void on_height_valueChanged(int h);
};

#endif // SQLDESIGNVIEW_H
