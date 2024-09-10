#ifndef BUILDINFO_H
#define BUILDINFO_H

#include "project.h"
#include <QString>
#include <QStringList>

class BuildInfo
{
public:
    BuildInfo(QString filename, QStringList entries, bool translate, QString path, ProjectType type);
    bool build();
private:
    QString filename;
    QStringList entries;
    QString path;
    QStringList mt, ft;

    void loadJavaTemplates();
};

#endif // BUILDINFO_H
