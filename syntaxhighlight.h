#ifndef SYNTAXHIGHLIGHT_H
#define SYNTAXHIGHLIGHT_H

#include <QSyntaxHighlighter>
#include <QRegularExpression>

struct SyntaxFormat {
    QRegularExpression pattern;
    QTextCharFormat format;
};

class SyntaxHighLight : public QSyntaxHighlighter
{
public:
    explicit SyntaxHighLight(QVector<SyntaxFormat> rules, QObject *parent = nullptr);
private:
    QVector<SyntaxFormat> rules;


    // QSyntaxHighlighter interface
protected:
    virtual void highlightBlock(const QString &text) override;
};

#endif // SYNTAXHIGHLIGHT_H
