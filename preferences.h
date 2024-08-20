#ifndef PREFERENCES_H
#define PREFERENCES_H

#include <QDialog>

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
        ItemEditor
    };
private slots:
    void on_listWidget_currentRowChanged(int currentRow);

    void on_fontComboBox_currentIndexChanged(int index);

private:
    Ui::Preferences *ui;

    void initData();
};

#endif // PREFERENCES_H
