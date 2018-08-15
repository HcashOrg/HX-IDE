#include "editorcontextmenu.h"

#include <QClipboard>
#include <QMimeData>
#include <QApplication>
#include <QDebug>
#include <QUrl>

EditorContextMenu::EditorContextMenu(bool isUndoEnabled, bool isRedoEnabled, bool isCutEnabled, bool isCopyEnable,
                                     bool isPasteEnabled, bool isDeleteEnabled, QWidget * parent)
    :QMenu(parent)
{

    QAction* undoAction = new QAction(tr("撤销"),this);
    connect(undoAction,&QAction::triggered,this,&EditorContextMenu::undoTriggered);
    this->addAction(undoAction);
    undoAction->setEnabled(isUndoEnabled);

    QAction* redoAction = new QAction(tr("恢复"),this);
    connect(redoAction,&QAction::triggered,this,&EditorContextMenu::redoTriggered);
    this->addAction(redoAction);
    redoAction->setEnabled(isRedoEnabled);

    this->addSeparator();

    QAction* cutAction = new QAction(tr("剪切"),this);
    connect(cutAction,&QAction::triggered,this,&EditorContextMenu::cutTriggered);
    this->addAction(cutAction);
    cutAction->setEnabled(isCutEnabled);

    QAction* copyAction = new QAction(tr("复制"),this);
    connect(copyAction,&QAction::triggered,this,&EditorContextMenu::copyTriggered);
    this->addAction(copyAction);
    copyAction->setEnabled(isCopyEnable);

    QAction* pasteAction = new QAction(tr("粘贴"),this);
    connect(pasteAction,&QAction::triggered,this,&EditorContextMenu::pasteTriggered);
    this->addAction(pasteAction);
    pasteAction->setEnabled(isPasteEnabled);

    QAction* deleteAction = new QAction(tr("删除"),this);
    connect(deleteAction,&QAction::triggered,this,&EditorContextMenu::deleteTriggered);
    this->addAction(deleteAction);
    deleteAction->setEnabled(isDeleteEnabled);

    this->addSeparator();

    QAction* selectAllAction = new QAction(tr("选择全部"),this);
    connect(selectAllAction,&QAction::triggered,this,&EditorContextMenu::selectAllTriggered);
    this->addAction(selectAllAction);

}

EditorContextMenu::~EditorContextMenu()
{

}



