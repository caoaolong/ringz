#include "entrylist.h"
#include "ui_entrylist.h"

EntryList::EntryList(QWidget *parent)
    :QDialog(parent), ui(new Ui::EntryList)
{
    ui->setupUi(this);
}

EntryList::~EntryList()
{
    delete ui;
}

QString EntryList::show(QStringList list)
{
    EntryList dialog;
    foreach(auto item, list)
        dialog.ui->listWidget->addItem(item);
    dialog.ui->listWidget->setCurrentRow(0);
    if(dialog.exec() == QDialog::Accepted) {
        QList<QListWidgetItem*> selectedItems = dialog.ui->listWidget->selectedItems();
        if (selectedItems.count() > 0) {
            return selectedItems.at(0)->text();
        }
    }
    return "";
}

