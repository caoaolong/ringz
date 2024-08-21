#ifndef DATASOURCE_H
#define DATASOURCE_H

#include <QDialog>
#include <QJsonObject>

namespace Ui {
class Datasource;
}

class DatasourceInfo
{
public:
    DatasourceInfo();
    DatasourceInfo(QJsonObject value);
    QString getType() const;
    void setType(const QString &newType);
    QString getHost() const;
    void setHost(const QString &newHost);
    QString getUsername() const;
    void setUsername(const QString &newUsername);
    QString getPassword() const;
    void setPassword(const QString &newPassword);
    QString getDatabase() const;
    void setDatabase(const QString &newDatabase);
    quint32 getPort() const;
    void setPort(quint32 newPort);
    bool getActive() const;
    void setActive(bool newActive);
    bool getConnect() const;
    void setConnect(bool newConnect);

private:
    QString type;
    QString host;
    QString username;
    QString password;
    QString database;
    quint32 port;
    bool active;
    bool connect;
};

class Datasource : public QDialog
{
    Q_OBJECT

public:
    explicit Datasource(QWidget *parent = nullptr);
    ~Datasource();

    static DatasourceInfo* create(DatasourceInfo *info);

private slots:
    void on_dbType_currentIndexChanged(int index);

    void on_select_clicked();

private:
    Ui::Datasource *ui;
};

#endif // DATASOURCE_H
