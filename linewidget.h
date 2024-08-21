#ifndef LINEWIDGET_H
#define LINEWIDGET_H

#include <QWidget>

class LineWidget : public QWidget
{
    Q_OBJECT
public:
    explicit LineWidget(QWidget *parent = nullptr);
    void newLine();
};

#endif // LINEWIDGET_H
