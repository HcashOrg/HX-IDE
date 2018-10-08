#ifndef CODEEDITOR_H
#define CODEEDITOR_H

#include "Editor.h"
#include "DataDefine.h"

namespace Ui {
class codeeditor;
}

class codeeditor : public Editor
{
    Q_OBJECT

public:
    explicit codeeditor(const QString &path, DataDefine::ThemeStyle theme = DataDefine::Black_Theme, QWidget *parent = 0);
    ~codeeditor();

public:
    void checkState()override final;

    void setText(QString text)override final;
    QString getText()override;
    QString getSelectedText()override final;

    void setMode(QString mode);
    void initFinish();    // 将js中的inited 置为true  之后可以触发onTextChanged()
    void setTheme(bool black);//是=黑色主题  否=白色主题

    void setEditable(bool bIs)override final;

    void SetBreakPoint(int line)override final;//强制添加某一行为断点
    void RemoveBreakPoint(int line)override final;//强制删除某一行断点
    void TabBreakPoint()override final;
    void ClearBreakPoint()override final;

    void SetDebuggerLine(int linenumber)override final;
    void ClearDebuggerLine()override final;
public slots:
    void undo()override final;
    void redo()override final;
    void cut()override final;
    void copy()override final;
    void paste()override final;
    void deleteText()override final;
    void selectAll()override final;
private:
    Ui::codeeditor *ui;
};

#endif // CODEEDITOR_H
