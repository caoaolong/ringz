#include "ringz.h"
#include "ui_ringz.h"
#include "project.h"
#include "datasource.h"
#include <QList>
#include <QFileDialog>

Ringz::Ringz(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Ringz)
{
    ui->setupUi(this);

    this->connections = new QList<DatabaseConnection*>();

    this->databaseIcon = QIcon(":/ui/icons/database.png");
    this->tableIcon = QIcon(":/ui/icons/table.png");
    this->keysIcon = QIcon(":/ui/icons/folder.png");
    this->primaryKeyIcon = QIcon(":/ui/icons/primary_key.png");
    this->indexIcon = QIcon(":/ui/icons/key.png");
    this->columnIcon = QIcon(":/ui/icons/column.png");
    this->folderIcon = QIcon(":/ui/icons/folder.png");
    this->fileIcon = QIcon(":/ui/icons/file.png");

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

void Ringz::on_dbTree_customContextMenuRequested(const QPoint &pos)
{
    Q_UNUSED(pos);
    if (ui->dbTree->selectedItems().isEmpty()) return;
    auto item = ui->dbTree->selectedItems().at(0);
    qDebug() << item->text(0);
    qDebug() << item->type();
}
