#ifndef DATASOURCE_H
#define DATASOURCE_H

#include <QDialog>

namespace Ui {
class Datasource;
}

class DatasourceInfo
{
public:
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

private:
    QString type;
    QString host;
    QString username;
    QString password;
    QString database;
    quint32 port;
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
