#include "ringz.h"

#include <QApplication>
#include <QDir>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Ringz w;
    w.show();
    return a.exec();
}
