#ifndef ACEEDITOR_H
#define ACEEDITOR_H

#include <QWidget>
#include "Editor.h"
#include "DataDefine.h"

namespace Ui {
class AceEditor;
}
class QWebEnginePage;

class AceEditor : public Editor
{
    Q_OBJECT

public:
    explicit AceEditor( const QString &path, DataDefine::ThemeStyle theme = DataDefine::Black_Theme,QWidget *parent = 0);
    ~AceEditor();
public:
    void checkState()override final;

    void setText(QString text)override final;
    QString getText()override final;
    QString getSelectedText()override final;

    void setMode(QString mode);
    void initFinish();    // 将js中的inited 置为true  之后可以触发onTextChanged()
    void setTheme(bool black);//是=黑色主题  否=白色主题

    void setEditable(bool bIs)override final;

public slots:
    void undo()override final;
    void redo()override final;
    void cut()override final;
    void copy()override final;
    void paste()override final;
    void deleteText()override final;
    void selectAll()override final;

private:
    Ui::AceEditor *ui;

};

#endif // ACEEDITOR_H
