#include "datasource.h"
#include "ui_datasource.h"
#include "entrylist.h"
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QMessageBox>

Datasource::Datasource(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Datasource)
{
    ui->setupUi(this);

    ui->dbType->addItem("MySQL", "QMYSQL");

    setTabOrder(ui->dbType, ui->host);
    setTabOrder(ui->host, ui->port);
    setTabOrder(ui->port, ui->username);
    setTabOrder(ui->username, ui->password);
    setTabOrder(ui->password, ui->database);
}

Datasource::~Datasource()
{
    delete ui;
}

DatasourceInfo* Datasource::create(DatasourceInfo *info)
{
    Datasource dialog;
    auto *ui = dialog.ui;
    if (ui->dbType->currentIndex() < 0 || ui->password->text().isEmpty() || ui->username->text().isEmpty())
        return nullptr;

    if (dialog.exec() == QDialog::Accepted) {
        info->setType(ui->dbType->currentData().toString());
        info->setPort(ui->port->value());
        info->setHost(ui->host->text());
        info->setUsername(ui->username->text());
        info->setPassword(ui->password->text());
        info->setDatabase(ui->database->text());
        return info;
    }
    return nullptr;
}


QString DatasourceInfo::getType() const
{
    return type;
}

void DatasourceInfo::setType(const QString &newType)
{
    type = newType;
}

QString DatasourceInfo::getHost() const
{
    return host;
}

void DatasourceInfo::setHost(const QString &newHost)
{
    host = newHost;
}

QString DatasourceInfo::getUsername() const
{
    return username;
}

void DatasourceInfo::setUsername(const QString &newUsername)
{
    username = newUsername;
}

QString DatasourceInfo::getPassword() const
{
    return password;
}

void DatasourceInfo::setPassword(const QString &newPassword)
{
    password = newPassword;
}

QString DatasourceInfo::getDatabase() const
{
    return database;
}

void DatasourceInfo::setDatabase(const QString &newDatabase)
{
    database = newDatabase;
}

quint32 DatasourceInfo::getPort() const
{
    return port;
}

void DatasourceInfo::setPort(quint32 newPort)
{
    port = newPort;
}

void Datasource::on_dbType_currentIndexChanged(int index)
{
    ui->host->setText("localhost");
    Q_UNUSED(index);
    if (ui->dbType->currentData() == "QMYSQL") {
        ui->port->setValue(3306);
        ui->username->setText("root");
        ui->password->setText("root1234");
    }
}


void Datasource::on_select_clicked()
{
    auto dbType = ui->dbType->currentData().toString();
    QSqlDatabase db = QSqlDatabase::addDatabase(dbType);
    db.setHostName(ui->host->text());
    db.setUserName(ui->username->text());
    db.setPassword(ui->password->text());
    db.setPort(ui->port->value());

    if (!db.open()) {
        QMessageBox::information(this, "错误", db.lastError().text());
        return;
    }
    QSqlQuery query;
    if (!query.exec("show databases")) {
        QMessageBox::information(this, "错误", db.lastError().text());
        db.close();
        return;
    }

    QStringList databases;
    while (query.next()) {
        databases << query.value(0).toString();
    }
    ui->database->setText(EntryList::show(databases));
    db.close();
}
