#ifndef RINGZ_H
#define RINGZ_H

#include "databaseconnection.h"

#include <QMainWindow>
#include <QSqlDatabase>
#include <QSqlError>

QT_BEGIN_NAMESPACE
namespace Ui {
class Ringz;
}
QT_END_NAMESPACE

class Ringz : public QMainWindow
{
    Q_OBJECT

public:
    Ringz(QWidget *parent = nullptr);
    ~Ringz();

private slots:
    void on_actionCreate_triggered();

private:
    Ui::Ringz *ui;
    QList<DatabaseConnection*> *connections;
};
#endif // RINGZ_H
