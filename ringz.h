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
#include <QMap>

#define RINGZ_HOME      "/Users/calong/ringz"
#define RINGZ_CONFIG    "/config.json"
#define RINGZ_THEME     "/themes/"

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

private:
    void showProjectTree(QTreeWidgetItem *parent, ProjectItem *item);

    void loadPreferences();

    void createEditor(EditorType type, QFile *fp);

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

    static QJsonObject preferences;
    QMap<QString, TextEditor*> editors;
};
#endif // RINGZ_H
