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
        connect(newAction,SIGNAL(triggered()),this,SIGNAL(newFileTriggered()));
        addAction(newAction);

    }
    if(type & ContextMenu::Copy)
    {
        QAction* copyAction = new QAction(tr("Copy"),this);
        connect(copyAction,SIGNAL(triggered()),this,SIGNAL(copyTriggered()));
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
        connect(pasteAction,SIGNAL(triggered()),this,SIGNAL(pasteTriggered()));
        this->addAction(pasteAction);

    }
    if(type & ContextMenu::Import)
    {
        QAction* importAction = new QAction(tr("Import Contract"),this);
        connect(importAction,SIGNAL(triggered()),this,SIGNAL(importTriggered()));
        this->addAction(importAction);
    }
    if(type & ContextMenu::Export)
    {
        QAction* exportAction = new QAction(tr("Export Contract"),this);
        connect(exportAction,SIGNAL(triggered()),this,SIGNAL(exportTriggered()));
        this->addAction(exportAction);
    }
    if(type & ContextMenu::Compile)
    {
        QAction* importAction = new QAction(tr("Compile"),this);
        connect(importAction,SIGNAL(triggered()),this,SIGNAL(compileTriggered()));
        this->addAction(importAction);
    }
    if(type & ContextMenu::Delete)
    {
        QAction* deleteAction = new QAction(tr("Delete"),this);
        connect(deleteAction,SIGNAL(triggered()),this,SIGNAL(deleteTriggered()));
        this->addAction(deleteAction);
    }
}

