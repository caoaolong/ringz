#include "preferences.h"
#include "ringz.h"
#include "ui_preferences.h"
#include <QListWidgetItem>
#include <QFile>
#include <QDir>

Preferences::Preferences(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Preferences)
{
    ui->setupUi(this);

    QListWidgetItem *appearence = new QListWidgetItem();
    appearence->setText("外观");
    appearence->setData(Qt::UserRole, ItemAppearance);
    ui->listWidget->addItem(appearence);
    QListWidgetItem *editor = new QListWidgetItem();
    editor->setText("编辑器");
    editor->setData(Qt::UserRole, ItemEditor);
    ui->listWidget->addItem(editor);

    initData();
}

Preferences::~Preferences()
{
    delete ui;
}


void Preferences::on_listWidget_currentRowChanged(int currentRow)
{
    Q_UNUSED(currentRow);
    int item = ui->listWidget->currentItem()->data(Qt::UserRole).toInt();
    switch (item) {
    case ItemAppearance:
        ui->stackedWidget->setCurrentWidget(ui->appearencePage);
        break;
    case ItemEditor:
        ui->stackedWidget->setCurrentWidget(ui->editorPage);
        break;
    }
}

void Preferences::initData()
{
    QFile conf(QString(RINGZ_HOME).append("/config.json"));
}

void Preferences::on_fontComboBox_currentIndexChanged(int index)
{
    auto font = ui->fontComboBox->currentFont();
    qDebug() << font.family();
}

