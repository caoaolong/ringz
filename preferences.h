#ifndef PREFERENCES_H
#define PREFERENCES_H

#include <QDialog>
#include <QJsonObject>

namespace Ui {
class Preferences;
}

class Preferences : public QDialog
{
    Q_OBJECT

public:
    explicit Preferences(QWidget *parent = nullptr);
    ~Preferences();
    enum ItemType {
        ItemAppearance,
        ItemEditor,
        ItemMapping
    };
private slots:
    void on_listWidget_currentRowChanged(int currentRow);

    void on_fontComboBox_currentIndexChanged(int index);

    void on_dbType_currentIndexChanged(int index);

    void on_language_currentIndexChanged(int index);

private:
    Ui::Preferences *ui;

    QJsonObject mapping;
    void initData();
    void showMapping();
};

#endif // PREFERENCES_H
