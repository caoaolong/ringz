#ifndef RZ_H
#define RZ_H

#include <QColor>

class Rz
{
public:
    Rz();
    static QColor parseColor(QString colorValue);
    static bool parseBool(QString value);
};

#endif // RZ_H
