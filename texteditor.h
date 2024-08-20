#ifndef TEXTEDITOR_H
#define TEXTEDITOR_H

#include <QWidget>
#include <QFile>
#include <QVector>
#include <QRegularExpression>
#include <QCompleter>

enum EditorType {
    SqlEditor,
    JavaEditor,
    XmlEditor
};

namespace Ui {
class TextEditor;
}

class TextEditor : public QWidget
{
    Q_OBJECT
public:
    explicit TextEditor(QString key, EditorType type, QFile *fp, QWidget *parent = nullptr);
    ~TextEditor();

    void appendContent(QString content);
private:
    Ui::TextEditor *ui;
    EditorType type;
    QCompleter *completer;
    QFile *fp;
    QString key;

signals:
    void windowClosed(QString name);
    // QWidget interface
protected:
    virtual void closeEvent(QCloseEvent *event) override;
};

#endif // TEXTEDITOR_H
