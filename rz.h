#ifndef RZ_H
#define RZ_H

#include <QColor>
#include <QFont>

class Rz
{
public:
    Rz();
    static QColor parseColor(QString colorValue);
    static bool parseBool(QString value);
    static QFont parseFont(QString fontValue);
};

#endif // RZ_H
