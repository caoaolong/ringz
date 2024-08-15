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

    void on_actionDsView_triggered(bool checked);

private:
    Ui::Ringz *ui;
    QList<DatabaseConnection*> *connections;
    // Icons
    QIcon databaseIcon;
    QIcon tableIcon;
    QIcon keysIcon;
    QIcon primaryKeyIcon;
    QIcon indexIcon;
    QIcon columnIcon;
};
#endif // RINGZ_H
