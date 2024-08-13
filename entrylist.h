#ifndef ENTRYLIST_H
#define ENTRYLIST_H

#include <QDialog>

namespace Ui {
class EntryList;
}

class EntryList : public QDialog
{
    Q_OBJECT

public:
    explicit EntryList(QWidget *parent = nullptr);
    ~EntryList();

    static QString show(QStringList list);

private:
    Ui::EntryList *ui;
};

#endif // ENTRYLIST_H
