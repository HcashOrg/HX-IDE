#ifndef CODEEDITOR_H
#define CODEEDITOR_H

#include "Editor.h"

namespace Ui {
class codeeditor;
}

class codeeditor : public Editor
{
    Q_OBJECT

public:
    explicit codeeditor(const QString &path,QWidget *parent = 0);
    ~codeeditor();

public:
    void checkState();

    void setText(QString text);
    QString getText();
    QString getSelectedText();

    void setMode(QString mode);
    void initFinish();    // 将js中的inited 置为true  之后可以触发onTextChanged()
    void setTheme(bool black);//是=黑色主题  否=白色主题

    void setEditable(bool bIs);

    void TabBreakPoint();
    void ClearBreakPoint();

public slots:
    void undo();
    void redo();
    void cut();
    void copy();
    void paste();
    void deleteText();
    void selectAll();
private:
    Ui::codeeditor *ui;
};

#endif // CODEEDITOR_H
