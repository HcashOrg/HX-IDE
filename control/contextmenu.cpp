#include "contextmenu.h"

#include <QClipboard>
#include <QMimeData>
#include <QApplication>
#include <QDebug>
#include <QUrl>

ContextMenu::ContextMenu(MenuType type,QWidget * parent) :
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
    switch (type) {
    case ContractItemType:
    {
        QAction* newAction = new QAction(QString::fromLocal8Bit("新建合约"),this);
        connect(newAction,SIGNAL(triggered()),this,SIGNAL(newFileTriggered()));
        this->addAction(newAction);

        QAction* pasteAction = new QAction(QString::fromLocal8Bit("粘贴"),this);
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
                    QString fileName = url.fileName();
                    if( fileName.endsWith(".glua"))
                    {
                        pasteAction->setEnabled(true);
                        break;
                    }
                }
            }
        }
        connect(pasteAction,SIGNAL(triggered()),this,SIGNAL(pasteTriggered()));
        this->addAction(pasteAction);

        QAction* importAction = new QAction(QString::fromLocal8Bit("导入合约"),this);
        connect(importAction,SIGNAL(triggered()),this,SIGNAL(importTriggered()));
        this->addAction(importAction);

        break;
    }
    case ContractLuaType:
    {
        QAction* newAction = new QAction(QString::fromLocal8Bit("新建合约"),this);
        connect(newAction,SIGNAL(triggered()),this,SIGNAL(newFileTriggered()));
        this->addAction(newAction);

        QAction* copyAction = new QAction(QString::fromLocal8Bit("复制"),this);
        connect(copyAction,SIGNAL(triggered()),this,SIGNAL(copyTriggered()));
        this->addAction(copyAction);


        QAction* pasteAction = new QAction(QString::fromLocal8Bit("粘贴"),this);
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
                    QString fileName = url.fileName();
                    if( fileName.endsWith(".glua"))
                    {
                        pasteAction->setEnabled(true);
                        break;
                    }
                }
            }
        }
        connect(pasteAction,SIGNAL(triggered()),this,SIGNAL(pasteTriggered()));
        this->addAction(pasteAction);

        QAction* deleteAction = new QAction(QString::fromLocal8Bit("删除"),this);
        connect(deleteAction,SIGNAL(triggered()),this,SIGNAL(deleteTriggered()));
        this->addAction(deleteAction);

        QAction* compileAction = new QAction(QString::fromLocal8Bit("编译"),this);
        connect(compileAction,SIGNAL(triggered()),this,SIGNAL(compileTriggered()));
        this->addAction(compileAction);

        QAction* importAction = new QAction(QString::fromLocal8Bit("导入合约"),this);
        connect(importAction,SIGNAL(triggered()),this,SIGNAL(importTriggered()));
        this->addAction(importAction);

        break;
    }
    case ContractCompiledType:
    {
        QAction* newAction = new QAction(QString::fromLocal8Bit("新建合约"),this);
        connect(newAction,SIGNAL(triggered()),this,SIGNAL(newFileTriggered()));
        this->addAction(newAction);

        QAction* copyAction = new QAction(QString::fromLocal8Bit("复制"),this);
        connect(copyAction,SIGNAL(triggered()),this,SIGNAL(copyTriggered()));
        this->addAction(copyAction);

        QAction* pasteAction = new QAction(QString::fromLocal8Bit("粘贴"),this);
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
                    QString fileName = url.fileName();
                    if( fileName.endsWith(".glua"))
                    {
                        pasteAction->setEnabled(true);
                        break;
                    }
                }
            }
        }
        connect(pasteAction,SIGNAL(triggered()),this,SIGNAL(pasteTriggered()));
        this->addAction(pasteAction);

        QAction* deleteAction = new QAction(QString::fromLocal8Bit("删除"),this);
        connect(deleteAction,SIGNAL(triggered()),this,SIGNAL(deleteTriggered()));
        this->addAction(deleteAction);

        QAction* registerContractAction = new QAction(QString::fromLocal8Bit("注册合约"),this);
        connect(registerContractAction,SIGNAL(triggered()),this,SIGNAL(registerContractTriggered()));
        this->addAction(registerContractAction);

        QAction* importAction = new QAction(QString::fromLocal8Bit("导入合约"),this);
        connect(importAction,SIGNAL(triggered()),this,SIGNAL(importTriggered()));
        this->addAction(importAction);

        break;
    }
    case MyTempContractPathType:
    {
        QAction* copyAction = new QAction(QString::fromLocal8Bit("复制"),this);
        connect(copyAction,SIGNAL(triggered()),this,SIGNAL(copyTriggered()));
        this->addAction(copyAction);

        QAction* callAction = new QAction(QString::fromLocal8Bit("调用合约"),this);
        connect(callAction,SIGNAL(triggered()),this,SIGNAL(callTriggered()));
        this->addAction(callAction);

        QAction* upgradeAction = new QAction(QString::fromLocal8Bit("升级合约"),this);
        connect(upgradeAction,SIGNAL(triggered()),this,SIGNAL(upgradeTriggered()));
        this->addAction(upgradeAction);

        QAction* withdrawAction = new QAction(QString::fromLocal8Bit("销毁合约"),this);
        connect(withdrawAction,SIGNAL(triggered()),this,SIGNAL(withdrawTriggered()));
        this->addAction(withdrawAction);

        QAction* transferAction = new QAction(QString::fromLocal8Bit("充值"),this);
        connect(transferAction,SIGNAL(triggered()),this,SIGNAL(transferTriggered()));
        this->addAction(transferAction);

        break;
    }
    case MyTempContractAddressType:
    {
        QAction* copyAction = new QAction(QString::fromLocal8Bit("复制地址"),this);
        connect(copyAction,SIGNAL(triggered()),this,SIGNAL(copyTriggered()));
        this->addAction(copyAction);

        QAction* callAction = new QAction(QString::fromLocal8Bit("调用合约"),this);
        connect(callAction,SIGNAL(triggered()),this,SIGNAL(callTriggered()));
        this->addAction(callAction);

        QAction* upgradeAction = new QAction(QString::fromLocal8Bit("升级合约"),this);
        connect(upgradeAction,SIGNAL(triggered()),this,SIGNAL(upgradeTriggered()));
        this->addAction(upgradeAction);

        QAction* withdrawAction = new QAction(QString::fromLocal8Bit("销毁合约"),this);
        connect(withdrawAction,SIGNAL(triggered()),this,SIGNAL(withdrawTriggered()));
        this->addAction(withdrawAction);

        QAction* transferAction = new QAction(QString::fromLocal8Bit("充值"),this);
        connect(transferAction,SIGNAL(triggered()),this,SIGNAL(transferTriggered()));
        this->addAction(transferAction);

        break;
    }
    case MyForeverContractPathType:
    {
        QAction* copyAction = new QAction(QString::fromLocal8Bit("复制"),this);
        connect(copyAction,SIGNAL(triggered()),this,SIGNAL(copyTriggered()));
        this->addAction(copyAction);

        QAction* callAction = new QAction(QString::fromLocal8Bit("调用合约"),this);
        connect(callAction,SIGNAL(triggered()),this,SIGNAL(callTriggered()));
        this->addAction(callAction);

        QAction* transferAction = new QAction(QString::fromLocal8Bit("充值"),this);
        connect(transferAction,SIGNAL(triggered()),this,SIGNAL(transferTriggered()));
        this->addAction(transferAction);
        break;
    }
    case MyForeverContractAddressType:
    {
        QAction* copyAction = new QAction(QString::fromLocal8Bit("复制地址"),this);
        connect(copyAction,SIGNAL(triggered()),this,SIGNAL(copyTriggered()));
        this->addAction(copyAction);

        QAction* callAction = new QAction(QString::fromLocal8Bit("调用合约"),this);
        connect(callAction,SIGNAL(triggered()),this,SIGNAL(callTriggered()));
        this->addAction(callAction);

        QAction* transferAction = new QAction(QString::fromLocal8Bit("充值"),this);
        connect(transferAction,SIGNAL(triggered()),this,SIGNAL(transferTriggered()));
        this->addAction(transferAction);
        break;
    }
    case ForeverContractType:
    {
        QAction* copyAction = new QAction(QString::fromLocal8Bit("复制地址"),this);
        connect(copyAction,SIGNAL(triggered()),this,SIGNAL(copyTriggered()));
        this->addAction(copyAction);

        QAction* callAction = new QAction(QString::fromLocal8Bit("调用合约"),this);
        connect(callAction,SIGNAL(triggered()),this,SIGNAL(callTriggered()));
        this->addAction(callAction);

        QAction* transferAction = new QAction(QString::fromLocal8Bit("充值"),this);
        connect(transferAction,SIGNAL(triggered()),this,SIGNAL(transferTriggered()));
        this->addAction(transferAction);

        QAction* exportAction = new QAction(QString::fromLocal8Bit("导出合约"),this);
        connect(exportAction,SIGNAL(triggered()),this,SIGNAL(exportTriggered()));
        this->addAction(exportAction);
        break;
    }
    case EventBindScriptType:
    {
        QAction* eventBindAction = new QAction(QString::fromLocal8Bit("绑定脚本"),this);
        connect(eventBindAction,SIGNAL(triggered()),this,SIGNAL(eventBindTriggered()));
        this->addAction(eventBindAction);
        break;
    }
    case BindedScriptType:
    {
        QAction* unbindAction = new QAction(QString::fromLocal8Bit("解除绑定"),this);
        connect(unbindAction,SIGNAL(triggered()),this,SIGNAL(unbindTriggered()));
        this->addAction(unbindAction);
        break;
    }
    case ScriptItemType:
    {
        QAction* newAction = new QAction(QString::fromLocal8Bit("新建脚本"),this);
        connect(newAction,SIGNAL(triggered()),this,SIGNAL(newFileTriggered()));
        this->addAction(newAction);

        QAction* pasteAction = new QAction(QString::fromLocal8Bit("粘贴"),this);
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
                    QString fileName = url.fileName();
                    if( fileName.endsWith(".glua"))
                    {
                        pasteAction->setEnabled(true);
                        break;
                    }
                }
            }
        }
        connect(pasteAction,SIGNAL(triggered()),this,SIGNAL(pasteTriggered()));
        this->addAction(pasteAction);

        break;
    }
    case ScriptLuaType:
    {
        QAction* newAction = new QAction(QString::fromLocal8Bit("新建脚本"),this);
        connect(newAction,SIGNAL(triggered()),this,SIGNAL(newFileTriggered()));
        this->addAction(newAction);

        QAction* copyAction = new QAction(QString::fromLocal8Bit("复制"),this);
        connect(copyAction,SIGNAL(triggered()),this,SIGNAL(copyTriggered()));
        this->addAction(copyAction);

        QAction* pasteAction = new QAction(QString::fromLocal8Bit("粘贴"),this);
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
                    QString fileName = url.fileName();
                    if( fileName.endsWith(".glua"))
                    {
                        pasteAction->setEnabled(true);
                        break;
                    }
                }
            }
        }
        connect(pasteAction,SIGNAL(triggered()),this,SIGNAL(pasteTriggered()));
        this->addAction(pasteAction);

        QAction* deleteAction = new QAction(QString::fromLocal8Bit("删除"),this);
        connect(deleteAction,SIGNAL(triggered()),this,SIGNAL(deleteTriggered()));
        this->addAction(deleteAction);

        QAction* compileAction = new QAction(QString::fromLocal8Bit("编译"),this);
        connect(compileAction,SIGNAL(triggered()),this,SIGNAL(compileTriggered()));
        this->addAction(compileAction);

        break;
    }
    case ScriptCompiledType:
    {
        QAction* newAction = new QAction(QString::fromLocal8Bit("新建脚本"),this);
        connect(newAction,SIGNAL(triggered()),this,SIGNAL(newFileTriggered()));
        this->addAction(newAction);

        QAction* copyAction = new QAction(QString::fromLocal8Bit("复制"),this);
        connect(copyAction,SIGNAL(triggered()),this,SIGNAL(copyTriggered()));
        this->addAction(copyAction);

        QAction* pasteAction = new QAction(QString::fromLocal8Bit("粘贴"),this);
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
                    QString fileName = url.fileName();
                    if( fileName.endsWith(".glua"))
                    {
                        pasteAction->setEnabled(true);
                        break;
                    }
                }
            }
        }
        connect(pasteAction,SIGNAL(triggered()),this,SIGNAL(pasteTriggered()));
        this->addAction(pasteAction);

        QAction* deleteAction = new QAction(QString::fromLocal8Bit("删除"),this);
        connect(deleteAction,SIGNAL(triggered()),this,SIGNAL(deleteTriggered()));
        this->addAction(deleteAction);

        QAction* addScriptAction = new QAction(QString::fromLocal8Bit("注册脚本"),this);
        connect(addScriptAction,SIGNAL(triggered()),this,SIGNAL(addScriptTriggered()));
        this->addAction(addScriptAction);

        break;
    }
    case AddedScriptType:
    {
        QAction* copyAction = new QAction(QString::fromLocal8Bit("复制"),this);
        connect(copyAction,SIGNAL(triggered()),this,SIGNAL(copyTriggered()));
        this->addAction(copyAction);

        QAction* removeAction = new QAction(QString::fromLocal8Bit("注销脚本"),this);
        connect(removeAction,SIGNAL(triggered()),this,SIGNAL(removeTriggered()));
        this->addAction(removeAction);

        QAction* bindAction = new QAction(QString::fromLocal8Bit("绑定脚本"),this);
        connect(bindAction,SIGNAL(triggered()),this,SIGNAL(bindTriggered()));
        this->addAction(bindAction);

        break;
    }
    default:
        break;
    }
}

