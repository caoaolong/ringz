#include "syntaxhighlight.h"

SyntaxHighLight::SyntaxHighLight(QVector<SyntaxFormat> rules, QObject *parent)
    : QSyntaxHighlighter{parent}
{
    this->rules = rules;
}

void SyntaxHighLight::highlightBlock(const QString &text)
{
    for (auto rule : rules) {
        QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
        while (matchIterator.hasNext()) {
            QRegularExpressionMatch match = matchIterator.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }
    setCurrentBlockState(0);
}


