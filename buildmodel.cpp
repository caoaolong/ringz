#include "buildmodel.h"
#include "rz.h"
#include "ui_buildmodel.h"
#include "buildinfo.h"

BuildModel::BuildModel(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::BuildModel)
{
    ui->setupUi(this);
}

BuildModel::~BuildModel()
{
    delete ui;
}

bool BuildModel::showTable(QWidget *parent, QSqlDatabase db, QString table, QList<ProjectInfo*> *projects)
{
    BuildModel dialog(parent);
    auto columns = Rz::tableDesc(db, table);
    dialog.ui->tableWidget->setRowCount(columns.count());
    dialog.ui->tableWidget->horizontalHeader()->setStretchLastSection(true);
    ProjectType pType = Unknown;
    QString workspace;
    QString language;
    for (int i = 0; i < projects->length(); i++) {
        auto item = projects->at(i);
        dialog.ui->projects->addItem(item->getIcon(), item->getRoot()->getName());
        if (item->getActive()) {
            dialog.ui->projects->setCurrentIndex(i);
            pType = item->getType();
            workspace = item->getWorkspace();
            language = item->getLanguage();
        }
    }
    dialog.ui->filename->setText(getName(true, table));
    for (int i = 0; i < columns.count(); i++) {
        auto column = columns.at(i);
        QTableWidgetItem *item = new QTableWidgetItem(column.getName());
        item->setCheckState(Qt::Checked);
        dialog.ui->tableWidget->setItem(i, 0, item);
        QTableWidgetItem *type = new QTableWidgetItem(column.getType());
        type->setFlags(type->flags() & ~Qt::ItemIsEditable);
        dialog.ui->tableWidget->setItem(i, 1, type);

        QComboBox *comboBox = new QComboBox(dialog.parentWidget());
        QStringList entries = Rz::languageMapping(language);
        comboBox->addItems(entries);
        QString dataType = column.getType().split("(")[0].toLower();
        comboBox->setCurrentIndex(entries.indexOf(
            Rz::mapType(db.driverName(), language, dataType)));
        dialog.ui->tableWidget->setCellWidget(i, 2, comboBox);

        dialog.ui->tableWidget->setItem(i, 3, new QTableWidgetItem(column.getComment()));
    }
    QStringList entries;
    int rc = dialog.ui->tableWidget->rowCount();
    for (int i = 0; i < rc; i++) {
        auto item = dialog.ui->tableWidget->item(i, 0);
        if (item->checkState() == Qt::Checked) {
            auto value = dialog.ui->tableWidget->item(i, 0)->text();
            if (!dialog.ui->reservePrefix->isChecked()) {
                value = value.replace(dialog.ui->prefix->text(), "");
            }
            if (!dialog.ui->reserveSuffix->isChecked()) {
                value = value.replace(dialog.ui->prefix->text(), "");
            }
            entries.append(value);
        }
    }
    if (dialog.exec() == QDialog::Accepted) {
        auto info = new BuildInfo(
                        dialog.ui->filename->text(), entries,
                        dialog.ui->nameTranslate->isChecked(),
            workspace, pType);
        return info->build();
    }
    return false;
}

QString BuildModel::getName(bool firstUpper, QString name)
{
    QString value;
    bool toUpper = firstUpper;
    for (auto c : name) {
        if (c == '_') {
            toUpper = true;
            continue;
        }
        if (toUpper) {
            value.append(c.toUpper());
            toUpper = false;
        } else {
            value.append(c);
        }
    }
    return value;
}
