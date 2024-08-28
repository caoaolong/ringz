#include "codetextedit.h"
#include "rz.h"
#include "ringz.h"
#include <QAbstractItemView>
#include <QKeyEvent>
#include <QScrollBar>
#include <QJsonValue>
#include <QMessageBox>
#include <QJsonDocument>

CodeTextEdit::CodeTextEdit(QWidget *parent)
    :QTextEdit(parent)
{

}

void CodeTextEdit::initialize(EditorType type)
{
    this->type = type;
    // 设置字体
    this->font = Rz::parseFont(Ringz::getPreference("editor")["font"].toString());
    setFont(this->font);
    // 代码高亮设置
    this->initTheme();
    this->shl = new SyntaxHighLight(this->rules);
    this->shl->setDocument(document());

    QTextBlockFormat block;
    this->lineHeight = Ringz::getPreference("editor")["lineHeight"].toInt();
    block.setLineHeight(this->lineHeight, QTextBlockFormat::LineDistanceHeight);
    selectAll();
    auto cursor = textCursor();
    cursor.setBlockFormat(block);
    setTextCursor(cursor);

    cursor.clearSelection();
    setTextCursor(cursor);
}

void CodeTextEdit::setCompleter(QCompleter *completer)
{
    if (c)
        c->disconnect(this);

    c = completer;

    if (!c)
        return;

    auto popup = c->popup();
    popup->setFont(font);
    c->setWidget(this);
    c->setCompletionMode(QCompleter::PopupCompletion);
    c->setCaseSensitivity(Qt::CaseInsensitive);
    QObject::connect(c, QOverload<const QString &>::of(&QCompleter::activated),
                     this, &CodeTextEdit::insertCompletion);
}

QCompleter *CodeTextEdit::completer() const
{
    return c;
}

void CodeTextEdit::insertCompletion(const QString &completion)
{
    if (c->widget() != this)
        return;
    QTextCursor tc = textCursor();
    int extra = completion.length() - c->completionPrefix().length();
    tc.movePosition(QTextCursor::Left);
    tc.movePosition(QTextCursor::EndOfWord);
    tc.insertText(completion.right(extra));
    setTextCursor(tc);
}

LineWidget *CodeTextEdit::getLineWidget() const
{
    return lineWidget;
}

void CodeTextEdit::setLineWidget(LineWidget *lineWidget)
{
    this->lineWidget = lineWidget;
}

QString CodeTextEdit::textUnderCursor() const
{
    QTextCursor tc = textCursor();
    tc.select(QTextCursor::WordUnderCursor);
    return tc.selectedText();
}

void CodeTextEdit::keyPressEvent(QKeyEvent *e)
{
    if (c && c->popup()->isVisible()) {
        // The following keys are forwarded by the completer to the widget
        switch (e->key()) {
        case Qt::Key_Return:
        case Qt::Key_Enter:
        case Qt::Key_Escape:
        case Qt::Key_Tab:
        case Qt::Key_Backtab:
            e->ignore();
            return; // let the completer do default behavior
        default:
            break;
        }
    }

    if (e->key() == Qt::Key_Return || e->key() == Qt::Key_Enter) {
        lineWidget->newLine();
    }

    const bool isShortcut = (e->modifiers().testFlag(Qt::ControlModifier) && e->key() == Qt::Key_E); // CTRL+E
    if (!c || !isShortcut) // do not process the shortcut when we have a completer
        QTextEdit::keyPressEvent(e);

    const bool ctrlOrShift = e->modifiers().testFlag(Qt::ControlModifier) ||
                             e->modifiers().testFlag(Qt::ShiftModifier);
    if (!c || (ctrlOrShift && e->text().isEmpty()))
        return;

    static QString eow("~!@#$%^&*()_+{}|:\"<>?,./;'[]\\-="); // end of word
    const bool hasModifier = (e->modifiers() != Qt::NoModifier) && !ctrlOrShift;
    QString completionPrefix = textUnderCursor();
    if (!isShortcut && (hasModifier || e->text().isEmpty()|| completionPrefix.length() < 3
                        || eow.contains(e->text().right(1)))) {
        c->popup()->hide();
        return;
    }

    if (completionPrefix != c->completionPrefix()) {
        c->setCompletionPrefix(completionPrefix);
        c->popup()->setCurrentIndex(c->completionModel()->index(0, 0));
    }
    QRect cr = cursorRect();
    cr.setWidth(c->popup()->sizeHintForColumn(0)
                + c->popup()->verticalScrollBar()->sizeHint().width());
    c->complete(cr); // popup it up!
}


void CodeTextEdit::focusInEvent(QFocusEvent *event)
{
    if (c)
        c->setWidget(this);
    QTextEdit::focusInEvent(event);
}

void CodeTextEdit::initTheme()
{
    // 设置背景前景色
    QPalette pallete = this->palette();
    auto colors = Ringz::getTheme("colors");
    pallete.setColor(QPalette::Base, QColor(colors["editor.background"].toString()));
    pallete.setColor(QPalette::Text, QColor(colors["editor.foreground"].toString()));
    this->setPalette(pallete);
    // rules.clear();
    // auto format = parseSyntaxFormat(hlPref["keyword"].toString());
    // SyntaxFormat rule;
    // for (auto item : getKeywords()) {
    //     rule.pattern = QRegularExpression(item);
    //     rule.format = format;
    //     rules.append(rule);
    // }
}

QTextCharFormat CodeTextEdit::parseSyntaxFormat(QString value)
{
    QTextCharFormat format;
    auto vs = value.split(",");
    format.setForeground(QBrush(QColor(vs[0])));
    format.setFontWeight(Rz::parseBool(vs[1]) ? QFont::Bold : QFont::Normal);
    format.setFontItalic(Rz::parseBool(vs[2]));
    return format;
}

QStringList CodeTextEdit::getKeywords()
{
    switch(type){
    case SqlEditor:
        return QStringList() << "\\bselect\\b" << "\\binsert\\b" << "\\bupdate\\b" << "\\bdelete\\b";
    case JavaEditor:
        return QStringList() << "";
    case XmlEditor:
        return QStringList() << "";
    }
}

