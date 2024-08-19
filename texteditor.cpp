#include "ringz.h"
#include "texteditor.h"
#include "ui_texteditor.h"
#include <QFile>
#include <QMessageBox>

TextEditor::TextEditor(EditorType type, QJsonValue editorPref, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::TextEditor)
{
    ui->setupUi(this);

    this->type = type;
    // 代码高亮设置
    this->initSyntaxFormat(editorPref["highLight"]);
    this->shl = new SyntaxHighLight(this->rules);
    this->shl->setDocument(ui->textEdit->document());
    // 初始化参数
    auto editor = ui->textEdit;
    editor->setTextColor(Qt::white);
    auto props = editorPref["font"].toString().split(",");
    QFont font(props[0], props[1].toInt());
    font.setBold(parseBool(props[2]));
    font.setItalic(parseBool(props[3]));
    editor->setFont(font);
    editor->selectAll();
    QTextBlockFormat block;
    block.setLineHeight(editorPref["lineHeight"].toInt(), QTextBlockFormat::LineDistanceHeight);
    editor->textCursor().setBlockFormat(block);
    editor->textCursor().clearSelection();
    setWindowModified(true);
}

TextEditor::~TextEditor()
{
    delete ui;
}

void TextEditor::initSyntaxFormat(QJsonValue hlPref)
{
    rules.clear();
    auto format = parseSyntaxFormat(hlPref["keyword"].toString());
    SyntaxFormat rule;
    for (auto item : getKeywords()) {
        rule.pattern = QRegularExpression(item);
        rule.format = format;
        rules.append(rule);
    }
}

QTextCharFormat TextEditor::parseSyntaxFormat(QString value)
{
    QTextCharFormat format;
    auto vs = value.split(",");
    format.setForeground(QBrush(parseColor(vs[0])));
    format.setFontWeight(parseBool(vs[1]) ? QFont::Bold : QFont::Normal);
    format.setFontItalic(parseBool(vs[2]));
    return format;
}

QColor TextEditor::parseColor(QString color)
{
    int r = color.mid(1, 2).toInt(nullptr, 16);
    int g = color.mid(3, 2).toInt(nullptr, 16);
    int b = color.mid(5, 2).toInt(nullptr, 16);
    return QColor(r, g, b);
}

bool TextEditor::parseBool(QString value)
{
    return value.toLower() == "true";
}

QStringList TextEditor::getKeywords()
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
