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
    explicit AceEditor( QString path, QWidget *parent = 0);
    ~AceEditor();

    QString filePath;

    void hide();
    void show();

    void setText(QString text);
    QString getText();
    QString getSelectedText();
    void setMode(QString mode);
    void setReadOnly(bool bIs);
    void insert(QString text);
    void removeLines(int firstRow, int lastRow);
    void moveCursorTo(int row,int column);
    void initFinish();    // 将js中的inited 置为true  之后可以触发onTextChanged()


    bool isUndoAvailable();
    bool isRedoAvailable();

    bool isEditable();
    void setEditable(bool bIs);

    void setSaved(bool isSaved);
    bool isSaved();

    void setInitTemplate();    //  新建的合约显示 初始模板
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
    bool loaded;
    bool editable;
    bool hasSaved;

    bool eventFilter(QObject *watched, QEvent *e);
    QVariant syncRunJavaScript(const QString &javascript, int msec = 3000);

};

#endif // ACEEDITOR_H
