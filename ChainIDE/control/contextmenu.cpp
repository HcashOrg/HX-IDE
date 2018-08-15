#include "contextmenu.h"

#include <QClipboard>
#include <QMimeData>
#include <QApplication>
#include <QDebug>
#include <QUrl>

ContextMenu::ContextMenu(MenuTypes type,QWidget * parent) :
    QMenu(parent),
    type(type)
{
    createActions();
}

ContextMenu::~ContextMenu()
{

}

void ContextMenu::createActions()
{
    if(type & ContextMenu::NewFile)
    {
        QAction* newAction = new QAction(tr("New Contract"),this);
        connect(newAction,&QAction::triggered,this,&ContextMenu::newFileTriggered);
        addAction(newAction);

    }
    if(type & ContextMenu::Copy)
    {
        QAction* copyAction = new QAction(tr("Copy"),this);
        connect(copyAction,&QAction::triggered,this,&ContextMenu::copyTriggered);
        this->addAction(copyAction);

    }
    if(type & ContextMenu::Paste)
    {
        QAction* pasteAction = new QAction(tr("Paste"),this);
        pasteAction->setEnabled(false);
        QClipboard* cb = QApplication::clipboard();
        const QMimeData* mimeData = cb->mimeData();
        if( mimeData->hasUrls())
        {
            QList<QUrl> urls = mimeData->urls();

            foreach (QUrl url, urls)
            {
                if( url.isLocalFile())
                {
                    pasteAction->setEnabled(true);
                    break;
                }
            }
        }
        connect(pasteAction,&QAction::triggered,this,&ContextMenu::pasteTriggered);
        this->addAction(pasteAction);

    }
    if(type & ContextMenu::Import)
    {
        QAction* importAction = new QAction(tr("Import Contract"),this);
        connect(importAction,&QAction::triggered,this,&ContextMenu::importTriggered);
        this->addAction(importAction);
    }
    if(type & ContextMenu::Export)
    {
        QAction* exportAction = new QAction(tr("Export Contract"),this);
        connect(exportAction,&QAction::triggered,this,&ContextMenu::exportTriggered);
        this->addAction(exportAction);
    }
    if(type & ContextMenu::Compile)
    {
        QAction* importAction = new QAction(tr("Compile"),this);
        connect(importAction,&QAction::triggered,this,&ContextMenu::compileTriggered);
        this->addAction(importAction);
    }
    if(type & ContextMenu::Delete)
    {
        QAction* deleteAction = new QAction(tr("Delete"),this);
        connect(deleteAction,&QAction::triggered,this,&ContextMenu::deleteTriggered);
        this->addAction(deleteAction);
    }
    if(type & ContextMenu::Show)
    {
        QAction* showAction = new QAction(tr("Show In Explorer"),this);
        connect(showAction,&QAction::triggered,this,&ContextMenu::showInExplorer);
        this->addAction(showAction);
    }
}

