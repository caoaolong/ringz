#include "ringz.h"
#include "ui_ringz.h"
#include "datasource.h"
#include "databaseconnection.h"

Ringz::Ringz(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Ringz)
{
    ui->setupUi(this);

    this->connections = new QList<DatabaseConnection*>();
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
        database->setIcon(0, QIcon(":/ui/icons/database.png"));
        database->setText(0, info->getDatabase());
        ui->dsTree->addTopLevelItem(database);
        QStringList tables = conn->tables();
        foreach(auto item, tables) {
            QTreeWidgetItem *table = new QTreeWidgetItem();
            table->setIcon(0, QIcon(":/ui/icons/table.png"));
            table->setText(0, item);
            database->addChild(table);
        }
    }
}
