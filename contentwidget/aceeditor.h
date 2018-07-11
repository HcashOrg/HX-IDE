#ifndef ACEEDITOR_H
#define ACEEDITOR_H

#include <QWidget>
#include "Editor.h"

namespace Ui {
class AceEditor;
}
class QWebEnginePage;

class AceEditor : public Editor
{
    Q_OBJECT

public:
    explicit AceEditor( const QString &path, QWidget *parent = 0);
    ~AceEditor();
public:
    void checkState();

    void setText(QString text);
    QString getText();
    QString getSelectedText();

    void setMode(QString mode);
    void initFinish();    // 将js中的inited 置为true  之后可以触发onTextChanged()
    void setTheme(bool black);//是=黑色主题  否=白色主题

    void setEditable(bool bIs);

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

};

#endif // ACEEDITOR_H
