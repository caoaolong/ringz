#include "rz.h"

Rz::Rz() {}

QColor Rz::parseColor(QString color)
{
    int r = color.mid(1, 2).toInt(nullptr, 16);
    int g = color.mid(3, 2).toInt(nullptr, 16);
    int b = color.mid(5, 2).toInt(nullptr, 16);
    return QColor(r, g, b);
}

bool Rz::parseBool(QString value)
{
    return value.toLower() == "true";
}
