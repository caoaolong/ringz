#include "buildinfo.h"
#include "buildmodel.h"
#include "ringz.h"

BuildInfo::BuildInfo(QString filename, QStringList entries, bool translate, QString path, ProjectType type)
{
    this->filename = filename;
    if (translate) {
        QStringList tes;
        for (const auto &item : entries) {
            tes.append(BuildModel::getName(false, item));
        }
        this->entries = tes;
    } else {
        this->entries = entries;
    }
    this->path = path;
    switch (type) {
    case MavenProject:
        loadJavaTemplates();
        break;
    case Unknown:
        break;
    }
}

bool BuildInfo::build()
{
    return true;
}

void BuildInfo::loadJavaTemplates()
{
    this->mt.append(Ringz::getTemplate("java_model.tpl"));
    this->ft.append(Ringz::getTemplate("java_model_field.tpl"));
}
