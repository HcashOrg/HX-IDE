#ifndef ACEEDITOR_H
#define ACEEDITOR_H

#include <QWidget>

namespace Ui {
class AceEditor;
}
class QWebEnginePage;

class AceEditor : public QWidget
{
    Q_OBJECT

public:
    explicit AceEditor( QString path, bool iseditable,QWidget *parent = 0);
    ~AceEditor();

    void hide();
    void show();

    void setText(QString text);
    QString getText();
    QString getSelectedText();
    void setMode(QString mode);
    void insert(QString text);
    void removeLines(int firstRow, int lastRow);
    void moveCursorTo(int row,int column);
    void initFinish();    // 将js中的inited 置为true  之后可以触发onTextChanged()
    void setTheme(bool black);//是=黑色主题  否=白色主题

    bool isUndoAvailable();
    bool isRedoAvailable();

    bool isEditable();
    void setEditable(bool bIs);

    void setSaved(bool isSaved);
    bool isSaved();

    bool saveFile();
public slots:
    void undo();
    void redo();
    void cut();
    void copy();
    void paste();
    void deleteText();
    void selectAll();

private:
    Ui::AceEditor *ui;

    bool eventFilter(QObject *watched, QEvent *e);
    QVariant syncRunJavaScript(const QString &javascript, int msec = 3000);
private:
    void initEditor();
    void checkFile(const QString &filePath);
    void openFile(const QString &filePath);
private:
    class DataPrivate;
    DataPrivate *_p;

};

#endif // ACEEDITOR_H
