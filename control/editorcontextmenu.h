#ifndef EDITORCONTEXTMENU_H
#define EDITORCONTEXTMENU_H

#include <QWidget>
#include <QMenu>


class EditorContextMenu : public QMenu
{
    Q_OBJECT
public:
    EditorContextMenu(bool isUndoEnabled, bool isRedoEnabled, bool isCutEnabled, bool isCopyEnable,
                       bool isPasteEnabled, bool isDeleteEnabled, QWidget * parent = 0);
    ~EditorContextMenu();

signals:
    void undoTriggered();
    void redoTriggered();
    void cutTriggered();
    void copyTriggered();
    void pasteTriggered();
    void deleteTriggered();
    void selectAllTriggered();

};


#endif // EDITORCONTEXTMENU_H
