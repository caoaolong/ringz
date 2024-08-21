#ifndef CODETEXTEDIT_H
#define CODETEXTEDIT_H

#include "syntaxhighlight.h"
#include "linewidget.h"
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
    LineWidget *getLineWidget() const;
    void setLineWidget(LineWidget *newLineWidth);

private slots:
    void insertCompletion(const QString &completion);

private:
    // ui widget
    QCompleter *c = nullptr;
    LineWidget *lineWidget = nullptr;

    // properties
    EditorType type;
    QVector<SyntaxFormat> rules;
    QJsonObject theme;
    QFont font;
    int lineHeight;

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
    void initTheme();
    // parse functions
    QTextCharFormat parseSyntaxFormat(QString value);
    QStringList getKeywords();

    // QWidget interface
protected:
    virtual void keyPressEvent(QKeyEvent *event) override;
    virtual void focusInEvent(QFocusEvent *event) override;
};

#endif // CODETEXTEDIT_H
