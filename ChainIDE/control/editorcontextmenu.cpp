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

    QAction* undoAction = new QAction(QString::fromLocal8Bit("撤销"),this);
    connect(undoAction,SIGNAL(triggered()),this,SIGNAL(undoTriggered()));
    this->addAction(undoAction);
    undoAction->setEnabled(isUndoEnabled);

    QAction* redoAction = new QAction(QString::fromLocal8Bit("恢复"),this);
    connect(redoAction,SIGNAL(triggered()),this,SIGNAL(redoTriggered()));
    this->addAction(redoAction);
    redoAction->setEnabled(isRedoEnabled);

    this->addSeparator();

    QAction* cutAction = new QAction(QString::fromLocal8Bit("剪切"),this);
    connect(cutAction,SIGNAL(triggered()),this,SIGNAL(cutTriggered()));
    this->addAction(cutAction);
    cutAction->setEnabled(isCutEnabled);

    QAction* copyAction = new QAction(QString::fromLocal8Bit("复制"),this);
    connect(copyAction,SIGNAL(triggered()),this,SIGNAL(copyTriggered()));
    this->addAction(copyAction);
    copyAction->setEnabled(isCopyEnable);

    QAction* pasteAction = new QAction(QString::fromLocal8Bit("粘贴"),this);
    connect(pasteAction,SIGNAL(triggered()),this,SIGNAL(pasteTriggered()));
    this->addAction(pasteAction);
    pasteAction->setEnabled(isPasteEnabled);

    QAction* deleteAction = new QAction(QString::fromLocal8Bit("删除"),this);
    connect(deleteAction,SIGNAL(triggered()),this,SIGNAL(deleteTriggered()));
    this->addAction(deleteAction);
    deleteAction->setEnabled(isDeleteEnabled);

    this->addSeparator();

    QAction* selectAllAction = new QAction(QString::fromLocal8Bit("选择全部"),this);
    connect(selectAllAction,SIGNAL(triggered()),this,SIGNAL(selectAllTriggered()));
    this->addAction(selectAllAction);

}

EditorContextMenu::~EditorContextMenu()
{

}



