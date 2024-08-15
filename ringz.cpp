#include "ringz.h"
#include "ui_ringz.h"
#include "datasource.h"
#include <QList>

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
}

Ringz::~Ringz()
{
    delete ui;
}

void Ringz::on_actionCreate_triggered()
{
    DatasourceInfo *info = new DatasourceInfo();
    if (Datasource::create(info)) {
        DatabaseConnection *conn = new DatabaseConnection(info);
        this->connections->append(conn);
        QTreeWidgetItem *database = new QTreeWidgetItem();
        database->setIcon(0, databaseIcon);
        database->setText(0, info->getDatabase());
        ui->dbTree->addTopLevelItem(database);
        auto tables = conn->tables();
        for (Table* &item : *tables) {
            QTreeWidgetItem *table = new QTreeWidgetItem();
            table->setIcon(0, tableIcon);
            table->setText(0, item->getName());

            QTreeWidgetItem *primaryKeysItem = new QTreeWidgetItem();
            primaryKeysItem->setIcon(0, keysIcon);
            primaryKeysItem->setText(0, "键");
            auto primaryKeys = item->getPrimaryKeys();
            for (TableKey* &key : *primaryKeys) {
                QTreeWidgetItem *keyItem = new QTreeWidgetItem();
                keyItem->setIcon(0, primaryKeyIcon);
                keyItem->setText(0, key->getName());
                primaryKeysItem->addChild(keyItem);
            }
            table->addChild(primaryKeysItem);

            QTreeWidgetItem *indexesItem = new QTreeWidgetItem();
            indexesItem->setIcon(0, keysIcon);
            indexesItem->setText(0, "索引");
            auto indexes = item->getIndexes();
            for (TableKey* &key : *indexes) {
                QTreeWidgetItem *keyItem = new QTreeWidgetItem();
                keyItem->setIcon(0, indexIcon);
                keyItem->setText(0, key->getName());
                indexesItem->addChild(keyItem);
            }
            table->addChild(indexesItem);

            QTreeWidgetItem *columnItem = new QTreeWidgetItem();
            columnItem->setIcon(0, keysIcon);
            columnItem->setText(0, "字段");
            auto columns = item->getColumns();
            for (TableColumn* &column : *columns) {
                QTreeWidgetItem *keyItem = new QTreeWidgetItem();
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
    } else {
        ui->dbDock->close();
    }
}

