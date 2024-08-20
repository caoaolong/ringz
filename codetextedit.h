#ifndef CODETEXTEDIT_H
#define CODETEXTEDIT_H

#include "syntaxhighlight.h"
#include "texteditor.h"
#include <QTextEdit>
#include <QCompleter>
#include <QJsonObject>

class CodeTextEdit : public QTextEdit
{
public:
    CodeTextEdit(QWidget *parent = nullptr);

    void initialize(EditorType type);
    void setCompleter(QCompleter *c);
    QCompleter *completer() const;

private slots:
    void insertCompletion(const QString &completion);

private:
    QCompleter *c = nullptr;
    EditorType type;
    QVector<SyntaxFormat> rules;
    QJsonObject theme;
    QFont font;

    // Syntax Formater
    SyntaxHighLight *shl;

    // Text Char Format
    QTextCharFormat keywordForamt;
    QTextCharFormat commentForamt;
    QTextCharFormat numberForamt;
    QTextCharFormat stringForamt;

    QString textUnderCursor() const;

    // init functions
    void initCompleter(EditorType type);
    void initSyntaxFormat(QString theme);
    // parse functions
    QTextCharFormat parseSyntaxFormat(QString value);
    QColor parseColor(QString color);
    bool parseBool(QString value);
    QStringList getKeywords();

    // QWidget interface
protected:
    virtual void keyPressEvent(QKeyEvent *event) override;
    virtual void focusInEvent(QFocusEvent *event) override;
};

#endif // CODETEXTEDIT_H
