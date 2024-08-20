#include "texteditor.h"
#include "ui_texteditor.h"
#include <QFile>
#include <QMessageBox>
#include <QJsonValue>
#include <QAbstractItemView>
#include <QStringListModel>

TextEditor::TextEditor(QString key, EditorType type, QFile *fp, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::TextEditor)
{
    ui->setupUi(this);
    // 初始化成员变量
    if (fp) {
        this->fp = fp;
        setWindowTitle(this->fp->fileName());
    }
    this->key = key;
    // 设置代码补全
    this->completer = new QCompleter(this);
    QStringList words;
    words << "update" << "insert" << "delete" << "select" << "dela12";
    QStringListModel *model = new QStringListModel(words, this->completer);
    this->completer->setModel(model);
    // this->completer->setModelSorting(QCompleter::CaseInsensitivelySortedModel);
    this->completer->setCaseSensitivity(Qt::CaseInsensitive);
    this->completer->setWrapAround(false);

    ui->textEdit->initialize(type);
    ui->textEdit->setCompleter(completer);
}

TextEditor::~TextEditor()
{
    delete ui;
}

void TextEditor::appendContent(QString content)
{
    ui->textEdit->clear();
    ui->textEdit->insertPlainText(content);
}

void TextEditor::closeEvent(QCloseEvent *event)
{
    if (this->fp)
        fp->close();
    emit windowClosed(this->key);
    QWidget::closeEvent(event);
}
