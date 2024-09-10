#include "preferences.h"
#include "ringz.h"
#include "rz.h"
#include "ui_preferences.h"
#include <QListWidgetItem>
#include <QTableWidgetItem>
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
    QListWidgetItem *mapping = new QListWidgetItem();
    mapping->setText("映射表");
    mapping->setData(Qt::UserRole, ItemMapping);
    ui->listWidget->addItem(mapping);
    initData();

    ui->language->addItems(QStringList("Java") << "Golang");
    ui->listWidget->setCurrentRow(2);

    ui->tableWidget->horizontalHeader()->setStretchLastSection(true);
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
    case ItemMapping:
        ui->stackedWidget->setCurrentWidget(ui->mappingPage);
        break;
    }
}

void Preferences::initData()
{
    // 加载映射表
    this->mapping = Ringz::getPreference("mapping").toObject();
    ui->dbType->addItems(mapping.keys());
}

void Preferences::showMapping()
{
    auto table = ui->tableWidget;
    QString dbType = ui->dbType->currentText();
    QString language = ui->language->currentText();
    QJsonObject mv = this->mapping[dbType].toObject();
    auto types = mv.keys();
    table->clearContents();
    table->setRowCount(types.length());
    for(int i = 0; i < types.length(); i++) {
        auto type = types.at(i);
        table->setItem(i, 0, new QTableWidgetItem(type));
        auto ms = mv[type].toString().split(";");
        for (const auto &m : ms) {
            if (m.startsWith(language)) {
                QString value = m.split(":")[1];
                QComboBox *comboBox = new QComboBox(this);
                QStringList entries = Rz::languageMapping(language);
                comboBox->addItems(entries);
                comboBox->setCurrentIndex(entries.indexOf(value));
                table->setCellWidget(i, 1, comboBox);
                break;
            }
        }
    }
}

void Preferences::on_fontComboBox_currentIndexChanged(int index)
{
    Q_UNUSED(index);
    auto font = ui->fontComboBox->currentFont();
    qDebug() << font.family();
}


void Preferences::on_dbType_currentIndexChanged(int index)
{
    Q_UNUSED(index);
    if (ui->language->currentIndex() < 0) return;
    showMapping();
}


void Preferences::on_language_currentIndexChanged(int index)
{
    Q_UNUSED(index);
    if (ui->dbType->currentIndex() < 0) return;
    showMapping();
}

