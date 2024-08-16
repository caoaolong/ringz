#ifndef RINGZ_H
#define RINGZ_H

#include "databaseconnection.h"
#include "projectitem.h"
#include <QMainWindow>
#include <QSqlDatabase>
#include <QSqlError>
#include <QTreeWidgetItem>

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
    void on_actionDbCreate_triggered();

    void on_actionDsView_triggered(bool checked);

    void on_actionProView_toggled(bool checked);

    void on_dbTree_customContextMenuRequested(const QPoint &pos);

    void on_actionProOpen_triggered();

    void on_actionMdCreate_triggered();

private:
    void showProjectTree(QTreeWidgetItem *parent, ProjectItem *item);

private:
    // Tree Item Type
    enum DbItemType {
        DatabaseItem = 1001,
        TableItem,
        PrimaryKeysItem,
        PrimaryKeyItem,
        IndexesItem,
        IndexItem,
        ColumnsItem,
        ColumnItem
    };
    enum FileItemType {
        FolderItem = 1001,
        FileItem
    };

    Ui::Ringz *ui;
    QList<DatabaseConnection*> *connections;
    // Icons
    QIcon databaseIcon;
    QIcon tableIcon;
    QIcon keysIcon;
    QIcon primaryKeyIcon;
    QIcon indexIcon;
    QIcon columnIcon;
    QIcon folderIcon;
    QIcon fileIcon;
};
#endif // RINGZ_H
