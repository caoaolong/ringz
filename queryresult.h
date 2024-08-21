#ifndef QUERYRESULT_H
#define QUERYRESULT_H

#include <QObject>
#include <QTableWidget>
#include <QWidget>
#include <QSqlDatabase>
#include <QSqlTableModel>
#include <QSqlQueryModel>
#include <QJsonObject>
#include <QItemSelectionModel>

class QueryResult : public QTableView
{
    Q_OBJECT
public:
    QueryResult(QWidget *parent = nullptr);
    ~QueryResult();
    void show(QSqlDatabase db);
    void show(QString sql);
    void initTheme();
private:
    QJsonObject theme;
    QSqlQueryModel *model;
    QItemSelectionModel *selectionModel;
};

#endif // QUERYRESULT_H
