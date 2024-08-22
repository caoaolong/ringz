#ifndef RINGZ_H
#define RINGZ_H

#include "databaseconnection.h"
#include "texteditor.h"
#include "projectitem.h"
#include <QMainWindow>
#include <QSqlDatabase>
#include <QSqlError>
#include <QTreeWidgetItem>
#include <QJsonObject>
#include <QJsonArray>
#include <QMap>

#define RINGZ_HOME      "/Users/calong/ringz"
#define RINGZ_CONFIG    "/config.json"
#define RINGZ_THEME     "/themes/"
#define RINGZ_DATA      "/data.json"

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
    static QJsonValue getPreference(QString key);
    static QJsonValue getTheme(QString key);
    static QJsonValue getData(QString key);
private slots:
    void on_actionDbCreate_triggered();

    void on_actionDsView_triggered(bool checked);

    void on_actionProView_toggled(bool checked);

    void on_dbTree_customContextMenuRequested(const QPoint &pos);

    void on_actionProOpen_triggered();

    void on_actionMdCreate_triggered();

    void on_actionSqlCreate_triggered();

    void on_actionSettings_triggered();

    void on_actionFileOpen_triggered();

    void on_dbTree_itemDoubleClicked(QTreeWidgetItem *item, int column);

private:
    void showProjectTree(QTreeWidgetItem *parent, ProjectItem *item);
    void loadPreferences();
    void loadUserData();
    void loadTheme();
    void loadDatasource(QJsonArray connections);
    void createEditor(EditorType type, QFile *fp);
    void createDatasource(DatasourceInfo *info);
    void showDatasourceTree(QTreeWidgetItem *parent, DatabaseConnection *conn);
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
    DatabaseConnection* activeConnection;
    // Icons
    QIcon databaseIcon;
    QIcon databaseActiveIcon;
    QIcon tableIcon;
    QIcon keysIcon;
    QIcon primaryKeyIcon;
    QIcon indexIcon;
    QIcon columnIcon;
    QIcon folderIcon;
    QIcon fileIcon;

    static QJsonObject preferences;
    static QJsonObject data;
    static QJsonObject theme;
    QMap<QString, QWidget*> windows;
};
#endif // RINGZ_H
