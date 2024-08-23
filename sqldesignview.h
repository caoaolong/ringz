#ifndef SQLDESIGNVIEW_H
#define SQLDESIGNVIEW_H

#include <QWidget>

namespace Ui {
class SqlDesignView;
}

class SqlDesignView : public QWidget
{
    Q_OBJECT

public:
    explicit SqlDesignView(QWidget *parent = nullptr);
    ~SqlDesignView();

private:
    Ui::SqlDesignView *ui;

    void initTheme();
};

#endif // SQLDESIGNVIEW_H
