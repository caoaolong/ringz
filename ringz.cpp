#include "ringz.h"
#include "ui_ringz.h"
#include "project.h"
#include "datasource.h"
#include "texteditor.h"
#include "preferences.h"
#include <QList>
#include <QFileDialog>
#include <QMessageBox>
#include <QJsonDocument>

QJsonObject Ringz::preferences = QJsonObject();

Ringz::Ringz(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Ringz)
{
    ui->setupUi(this);
    // 读取配置文件
    this->loadPreferences();

    this->connections = new QList<DatabaseConnection*>();

    this->databaseIcon = QIcon(":/ui/icons/database.png");
    this->tableIcon = QIcon(":/ui/icons/table.png");
    this->keysIcon = QIcon(":/ui/icons/folder.png");
    this->primaryKeyIcon = QIcon(":/ui/icons/primary_key.png");
    this->indexIcon = QIcon(":/ui/icons/key.png");
    this->columnIcon = QIcon(":/ui/icons/column.png");
    this->folderIcon = QIcon(":/ui/icons/folder.png");
    this->fileIcon = QIcon(":/ui/icons/file.png");

    ui->mdiArea->setViewMode(QMdiArea::TabbedView);
    ui->mdiArea->setTabsClosable(true);
    ui->mdiArea->setTabsMovable(true);

    ui->dbTree->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->dbTree->setSelectionMode(QAbstractItemView::SingleSelection);

    ui->actionDsView->setChecked(!ui->dbDock->isHidden());
    ui->actionProView->setChecked(!ui->projectDock->isHidden());

    this->tabifyDockWidget(ui->dbDock, ui->projectDock);
}

Ringz::~Ringz()
{
    delete ui;
}

QJsonValue Ringz::getPreference(QString key)
{
    return preferences[key];
}

void Ringz::on_actionDbCreate_triggered()
{
    DatasourceInfo *info = new DatasourceInfo();
    if (Datasource::create(info)) {
        DatabaseConnection *conn = new DatabaseConnection(info);
        this->connections->append(conn);
        QTreeWidgetItem *database = new QTreeWidgetItem(DatabaseItem);
        database->setIcon(0, databaseIcon);
        database->setText(0, info->getDatabase());
        ui->dbTree->addTopLevelItem(database);
        auto tables = conn->tables();
        for (Table* &item : *tables) {
            QTreeWidgetItem *table = new QTreeWidgetItem(TableItem);
            table->setIcon(0, tableIcon);
            table->setText(0, item->getName());

            QTreeWidgetItem *primaryKeysItem = new QTreeWidgetItem(PrimaryKeysItem);
            primaryKeysItem->setIcon(0, keysIcon);
            primaryKeysItem->setText(0, "键");
            auto primaryKeys = item->getPrimaryKeys();
            for (TableKey* &key : *primaryKeys) {
                QTreeWidgetItem *keyItem = new QTreeWidgetItem(PrimaryKeyItem);
                keyItem->setIcon(0, primaryKeyIcon);
                keyItem->setText(0, key->getName());
                primaryKeysItem->addChild(keyItem);
            }
            table->addChild(primaryKeysItem);

            QTreeWidgetItem *indexesItem = new QTreeWidgetItem(IndexesItem);
            indexesItem->setIcon(0, keysIcon);
            indexesItem->setText(0, "索引");
            auto indexes = item->getIndexes();
            for (TableKey* &key : *indexes) {
                QTreeWidgetItem *keyItem = new QTreeWidgetItem(IndexItem);
                keyItem->setIcon(0, indexIcon);
                keyItem->setText(0, key->getName());
                indexesItem->addChild(keyItem);
            }
            table->addChild(indexesItem);

            QTreeWidgetItem *columnItem = new QTreeWidgetItem(ColumnsItem);
            columnItem->setIcon(0, keysIcon);
            columnItem->setText(0, "字段");
            auto columns = item->getColumns();
            for (TableColumn* &column : *columns) {
                QTreeWidgetItem *keyItem = new QTreeWidgetItem(ColumnItem);
                keyItem->setIcon(0, columnIcon);
                keyItem->setText(0, column->getName());
                columnItem->addChild(keyItem);
            }
            table->addChild(columnItem);

            database->addChild(table);
        }
        // TODO: 保存数据源信息

    }
}

void Ringz::on_actionDsView_triggered(bool checked)
{
    if (checked) {
        ui->dbDock->show();
        ui->dbDock->raise();
    } else {
        ui->dbDock->hide();
    }
}

void Ringz::on_actionProView_toggled(bool checked)
{
    if (checked) {
        ui->projectDock->show();
        ui->projectDock->raise();
    } else {
        ui->projectDock->hide();
    }
}

void Ringz::on_actionProOpen_triggered()
{
    QString projectDir = QFileDialog::getExistingDirectory(this, "打开项目", QDir::homePath());
    if (projectDir.isEmpty()) return;

    ProjectInfo *info = new ProjectInfo(projectDir);
    QTreeWidgetItem *project = new QTreeWidgetItem(FolderItem);
    project->setIcon(0, ProjectInfo::getIcon(info->getType()));
    project->setText(0, info->getRoot()->getName());
    ui->projectTree->addTopLevelItem(project);

    this->showProjectTree(project, info->getRoot());
}

void Ringz::showProjectTree(QTreeWidgetItem *parent, ProjectItem *item)
{
    for (auto item : *item->getChildren()) {
        if (item->getType() == ProjectItem::Folder) {
            QTreeWidgetItem *treeItem = new QTreeWidgetItem(FolderItem);
            treeItem->setText(0, item->getName());
            treeItem->setIcon(0, folderIcon);
            parent->addChild(treeItem);
            this->showProjectTree(treeItem, item);
        } else {
            QTreeWidgetItem *treeItem = new QTreeWidgetItem(FileItem);
            treeItem->setText(0, item->getName());
            treeItem->setIcon(0, fileIcon);
            parent->addChild(treeItem);
        }
    }
}

void Ringz::loadPreferences()
{
    QFile preferencesFile(QString(RINGZ_HOME).append(RINGZ_CONFIG));
    if (!preferencesFile.open(QFile::ReadOnly)) {
        QMessageBox::warning(this, "错误", "无法读取配置文件");
        return;
    }
    QByteArray preferencesData = preferencesFile.readAll();
    preferencesFile.close();
    QJsonDocument preferenceDoc = QJsonDocument::fromJson(preferencesData);
    if (preferenceDoc.isObject()) {
        this->preferences = preferenceDoc.object();
    }
}

void Ringz::createEditor(EditorType type, QFile *fp)
{
    QString key = "";
    if (fp) {
        key = fp->fileName();
    } else {
        key = QString::number(QDateTime::currentMSecsSinceEpoch());
    }
    TextEditor *editor = new TextEditor(key, type, fp);
    if (fp) {
        editor->appendContent(fp->readAll());
    }
    connect(editor, &TextEditor::windowClosed, this, [=](QString key){
        this->editors.remove(key);
    });
    this->editors.insert(key, editor);
    ui->mdiArea->addSubWindow(editor);
    editor->show();
}

void Ringz::on_dbTree_customContextMenuRequested(const QPoint &pos)
{
    Q_UNUSED(pos);
    if (ui->dbTree->selectedItems().isEmpty()) return;
    auto item = ui->dbTree->selectedItems().at(0);
    qDebug() << item->text(0);
    qDebug() << item->type();
}

void Ringz::on_actionMdCreate_triggered()
{

}


void Ringz::on_actionSqlCreate_triggered()
{
    createEditor(SqlEditor, nullptr);
}

void Ringz::on_actionSettings_triggered()
{
    Preferences *window = new Preferences();
    window->show();
}

void Ringz::on_actionFileOpen_triggered()
{
    QString filename = QFileDialog::getOpenFileName(
        this, "打开文件", QDir::homePath(),
        "文本文件(*.txt *.sql *.java *.xml *.yaml *.yml);;"
        "SQL文件(*.sql);;"
        "Java文件(*.sql);;"
        "Xml文件(*.xml);;"
        "Yaml文件(*.yaml *.yml)");
    if (filename.isEmpty()) return;
    QFile *file = new QFile(filename);
    if (!file->exists()) return;
    if (this->editors.contains(filename)) return;
    if (!file->open(QIODevice::ReadWrite | QIODevice::ExistingOnly)) {
        QMessageBox::warning(this, "错误", "文件打开失败");
        return;
    }
    createEditor(SqlEditor, file);
}
