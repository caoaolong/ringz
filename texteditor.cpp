#include "texteditor.h"
#include "ui_texteditor.h"
#include "queryresult.h"
#include <QFile>
#include <QMessageBox>
#include <QSqlQuery>
#include <QJsonValue>
#include <QAbstractItemView>
#include <QStringListModel>
#include <QSqlError>

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
    // 设置行组件
    ui->textEdit->setLineWidget(ui->lineNumber);
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
    // ui->dockWidget->hide();
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

QSqlDatabase TextEditor::getConn() const
{
    return conn;
}

void TextEditor::setConn(QSqlDatabase newConn)
{
    conn = newConn;
}

void TextEditor::closeEvent(QCloseEvent *event)
{
    if (this->fp)
        fp->close();
    emit windowClosed(this->key);
    QWidget::closeEvent(event);
}

void TextEditor::on_run_clicked()
{
    QString sql = ui->textEdit->textCursor().selectedText();
    if (sql.isEmpty())
        return;
    if (ui->dockWidget->isHidden()) {
        ui->dockWidget->show();
    }
    ui->result->show(sql);
}
