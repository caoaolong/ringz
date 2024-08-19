#ifndef TEXTEDITOR_H
#define TEXTEDITOR_H

#include "syntaxhighlight.h"
#include <QWidget>
#include <QFile>
#include <QVector>
#include <QRegularExpression>

namespace Ui {
class TextEditor;
}

class TextEditor : public QWidget
{
    Q_OBJECT
public:
    enum EditorType {
        SqlEditor,
        JavaEditor,
        XmlEditor
    };
    explicit TextEditor(EditorType type, QJsonValue editorPref, QWidget *parent = nullptr);
    ~TextEditor();

private:
    Ui::TextEditor *ui;
    EditorType type;
    QVector<SyntaxFormat> rules;

    // Syntax Formater
    SyntaxHighLight *shl;

    // Text Char Format
    QTextCharFormat keywordForamt;
    QTextCharFormat commentForamt;
    QTextCharFormat numberForamt;
    QTextCharFormat stringForamt;

    void initSyntaxFormat(QJsonValue hlPref);
    QTextCharFormat parseSyntaxFormat(QString value);
    // parse functions
    QColor parseColor(QString color);
    bool parseBool(QString value);
    QStringList getKeywords();
};

#endif // TEXTEDITOR_H
