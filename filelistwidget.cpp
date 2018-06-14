#include "filelistwidget.h"
#include "ui_filelistwidget.h"
#include "interfacewidget.h"

#include "hxchain.h"
#include "control/contextmenu.h"

#include <QDir>
#include <QDebug>
#include <QFileInfoList>
#include <QLayout>
#include <QMessageBox>
#include <QClipboard>
#include <QMimeData>
#include <QUrl>
#include <QTextCodec>
#include <QScrollBar>
#include <QTimer>

FileListWidget::FileListWidget(QWidget *parent) :
    QWidget(parent),
    currentIndex(0),
    fileUpdating(false),
    scriptUpdating(false),
    ui(new Ui::FileListWidget)
{
    ui->setupUi(this);

    connect(HXChain::getInstance(),SIGNAL(jsonDataUpdated(QString)),this,SLOT(jsonDataUpdated(QString)));


    verticalSplitter = new QSplitter(Qt::Vertical, ui->tab1);
    verticalSplitter->addWidget(ui->fileTreeWidget);
    interfaceWidget = new InterfaceWidget(verticalSplitter);

    QList<int> widgetSize;
    widgetSize << 350 << 350;
    verticalSplitter->setSizes(widgetSize);
    verticalSplitter->setHandleWidth(10);

    contractItem = new QTreeWidgetItem(ui->fileTreeWidget, QStringList() << QString::fromLocal8Bit("合约源码") );
    registeredItem = new QTreeWidgetItem(ui->fileTreeWidget, QStringList() << QString::fromLocal8Bit("我的合约") );
    onChainItem = new QTreeWidgetItem(ui->fileTreeWidget, QStringList() << QString::fromLocal8Bit("合约库") );
    scriptItem = new QTreeWidgetItem(ui->scriptsTreeWidget, QStringList() << QString::fromLocal8Bit("脚本源码") );
    addedScriptItem = new QTreeWidgetItem(ui->scriptsTreeWidget, QStringList() << QString::fromLocal8Bit("已注册脚本") );

//    connect( ui->fileTreeWidget, SIGNAL(itemClicked(QTreeWidgetItem*,int)), this, SLOT(fileChanged(QTreeWidgetItem*,int)));
//    connect( ui->scriptsTreeWidget, SIGNAL(itemClicked(QTreeWidgetItem*,int)), this, SLOT(scriptChanged(QTreeWidgetItem*,int)));


    QHBoxLayout* hbLayout = new QHBoxLayout;
    hbLayout->addWidget(ui->contractBtn);
    hbLayout->addWidget(ui->scriptBtn);
    hbLayout->setMargin(0);
    hbLayout->setSpacing(0);

    QVBoxLayout* vbLayout = new QVBoxLayout;
    vbLayout->addLayout(hbLayout);
    vbLayout->addWidget(ui->tabWidget);
    vbLayout->setMargin(0);
    vbLayout->setSpacing(0);
    this->setLayout( vbLayout);

    QVBoxLayout* vbLayout2 = new QVBoxLayout;
    vbLayout2->addWidget(verticalSplitter);
    vbLayout2->setMargin(0);
    ui->tab1->setLayout( vbLayout2);

    QVBoxLayout* vbLayout3 = new QVBoxLayout;
    vbLayout3->addWidget(ui->scriptsTreeWidget);
    vbLayout3->setMargin(0);
    ui->tab2->setLayout( vbLayout3);

    ui->tabWidget->tabBar()->hide();
    ui->fileTreeWidget->setHeaderHidden(true);
    ui->fileTreeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->scriptsTreeWidget->setHeaderHidden(true);
    ui->scriptsTreeWidget->setContextMenuPolicy(Qt::CustomContextMenu);

    on_contractBtn_clicked();

    getContractsList();
    getForeverContracts();
    getScriptsList();
    getAddedScripts();

    QTimer::singleShot(200,this,SLOT(getRegisteredContracts()));  // 第一次获取前等一段时间
}

FileListWidget::~FileListWidget()
{
    delete ui;
}

void FileListWidget::getContractsList()
{
    QDir dir("contracts");
    QStringList fileNameList = dir.entryList( QStringList() << "*.glua" << "*.gpc");

    fileUpdating = true;
    treeWidgetItemRemoveAllChildren(contractItem);
    fileUpdating = false;

    foreach (QString fileName, fileNameList)
    {
        FileRecord frcd;
        frcd.type = FileRecord::CONTRACT_FILE;
        frcd.path = dir.absoluteFilePath(fileName);
        frcd.newBuilt = false;
        HXChain::getInstance()->fileRecordMap.insert(dir.absoluteFilePath(fileName), frcd);
    }


    foreach (QString path, HXChain::getInstance()->fileRecordMap.keys())
    {
        QFileInfo info(path);
        if( dir.absolutePath() == info.absolutePath())
        {
            // 如果路径在 contracts 下 添加到列表中
            QTreeWidgetItem* item = new QTreeWidgetItem( QStringList() << info.fileName());
            contractItem->addChild(item);
        }
    }

}

void FileListWidget::getRegisteredContracts()
{
    QDir dir;
    if( HXChain::getInstance()->currenChain() == 1)
    {
        dir.setPath("registered/test");
    }
    else if( HXChain::getInstance()->currenChain() == 2)
    {
        dir.setPath("registered/formal");
    }
    QStringList fileNameList = dir.entryList( QStringList() << "*.glua" << "*.gpc");

    QTreeWidgetItem* currentItem = ui->fileTreeWidget->currentItem();
    QString preText;
    int scrollBarValue = ui->fileTreeWidget->verticalScrollBar()->value();      // 保存之前scrollbar的位置
    bool isExpanded = registeredItem->isExpanded();
    if(  currentItem != NULL)
    {
        if( currentItem->parent() == registeredItem)
        {
            // 如果刷新之前当前项在registeredItem下 刷新之后也应该选中那个项
            preText = currentItem->text(0);
        }
    }

    fileUpdating = true;
    treeWidgetItemRemoveAllChildren( registeredItem);
    fileUpdating = false;

    QStringList contractAdded;
    foreach (QString fileName, fileNameList)
    {
        if( fileName.endsWith("_removed.glua") || fileName.endsWith("_removed.gpc"))   continue;

        QString address = HXChain::getInstance()->configGetContractAddress(dir.absoluteFilePath(fileName));

        if( address.isEmpty())   continue;

        FileRecord frcd;
        frcd.type = FileRecord::CONTRACT_FILE;
        frcd.path = dir.absoluteFilePath(fileName);
        frcd.newBuilt = false;
        HXChain::getInstance()->fileRecordMap.insert(dir.absoluteFilePath(fileName), frcd);

        if( !HXChain::getInstance()->contractInfoMap.contains(address))
        {
            // 如果未获取到该合约的信息
            continue;
        }


        if( !HXChain::getInstance()->currentAccount.isEmpty())
        {
            // 切换链时有可能当前账户为空
            if( HXChain::getInstance()->contractInfoMap.value(address).owner.isEmpty() && !HXChain::getInstance()->contractInfoMap.value(address).ownerAddress.isEmpty())
            {
                // 如果是刚注册的，获取不到合约信息
                if( HXChain::getInstance()->accountInfoMap.value(HXChain::getInstance()->currentAccount).address != HXChain::getInstance()->contractInfoMap.value(address).ownerAddress)
                {
                    // 如果ownerAddress不是当前账户的address
                    continue;
                }

            }
            else
            {
                if( HXChain::getInstance()->ownerKeyAccountNameMap.value(HXChain::getInstance()->contractInfoMap.value(address).owner) != HXChain::getInstance()->currentAccount )
                {
                    // 如果owner不是当前账户

                    continue;
                }
            }
        }
        else
        {
            continue;
        }



        if( HXChain::getInstance()->contractInfoMap.value(address).state == "deleted")
        {
            // 已销毁的合约不显示
            continue;
        }


        QTreeWidgetItem* item = new QTreeWidgetItem( QStringList() << fileName);
        registeredItem->addChild(item);
        if( HXChain::getInstance()->contractInfoMap.value(address).level == "forever")
        {
            item->setTextColor(0,QColor(255,0,0));
        }
        contractAdded += address;


    }

    // 对于我的合约 却未在本地保存注册记录的
    foreach (QString myContract, HXChain::getInstance()->myContracts)
    {
        QString ownerName = HXChain::getInstance()->ownerKeyAccountNameMap.value(HXChain::getInstance()->contractInfoMap.value( myContract).owner);

        if( ownerName == HXChain::getInstance()->currentAccount)
        {
            // 如果是当前账户的合约

            // 如果本地有注册记录，已经添加了
            if( contractAdded.contains(myContract))  continue;

            if( HXChain::getInstance()->contractInfoMap.value(myContract).state == "deleted")
            {
                // 已销毁的合约不显示
                continue;
            }

            QTreeWidgetItem* item = new QTreeWidgetItem( QStringList() << myContract);
            registeredItem->addChild(item);
            if( HXChain::getInstance()->contractInfoMap.value(myContract).level == "forever")
            {
                item->setTextColor(0,QColor(255,0,0));
            }
        }
    }


    QStringList texts = treeWidgetItemGetAllChildrenText(registeredItem);
    if( !preText.isEmpty() && texts.contains(preText))
    {
        treeWidgetSetCurrentItem(0,1, preText);
    }

    registeredItem->sortChildren(0, Qt::AscendingOrder);
    registeredItem->setExpanded(isExpanded);
    ui->fileTreeWidget->verticalScrollBar()->setValue(scrollBarValue);
}

void FileListWidget::getForeverContracts()
{
    HXChain::getInstance()->postRPC(  "id_blockchain_get_forever_contracts",toJsonFormat( "blockchain_get_forever_contracts", QJsonArray() << "" ));
}

void FileListWidget::getScriptsList()
{
    QDir dir("scripts");
    QStringList fileNameList = dir.entryList( QStringList() << "*.glua" << "*.script");

    scriptUpdating = true;
    treeWidgetItemRemoveAllChildren(scriptItem);
    scriptUpdating = false;

    foreach (QString fileName, fileNameList)
    {
        FileRecord frcd;
        frcd.type = FileRecord::SCRIPT_FILE;
        frcd.path = dir.absoluteFilePath(fileName);
        frcd.newBuilt = false;
        HXChain::getInstance()->fileRecordMap.insert(dir.absoluteFilePath(fileName), frcd);
    }

    foreach (QString path, HXChain::getInstance()->fileRecordMap.keys())
    {
        QFileInfo info(path);
        if( dir.absolutePath() == info.absolutePath())
        {
            // 如果路径在 contracts 下 添加到列表中
            QTreeWidgetItem* item = new QTreeWidgetItem( QStringList() << info.fileName());
            scriptItem->addChild(item);
        }
    }

}

void FileListWidget::getAddedScripts()
{
    QDir dir;
    if( HXChain::getInstance()->currenChain() == 1)
    {
        dir.setPath("addedScripts/test");
    }
    else if( HXChain::getInstance()->currenChain() == 2)
    {
        dir.setPath("addedScripts/formal");
    }
    QStringList fileNameList = dir.entryList( QStringList() << "*.glua" << "*.script");

    scriptUpdating = true;
    treeWidgetItemRemoveAllChildren( addedScriptItem);
    scriptUpdating = false;

    foreach (QString fileName, fileNameList)
    {
        QTreeWidgetItem* item = new QTreeWidgetItem( QStringList() << fileName);
        addedScriptItem->addChild(item);

        FileRecord frcd;
        frcd.type = FileRecord::SCRIPT_FILE;
        frcd.path = dir.absoluteFilePath(fileName);
        frcd.newBuilt = false;
        HXChain::getInstance()->fileRecordMap.insert(dir.absoluteFilePath(fileName), frcd);
    }
}

void FileListWidget::addFile(int tabIndex, int index, QString fileName)
{
    if( tabIndex == 0)
    {
        QTreeWidgetItem* item = ui->fileTreeWidget->topLevelItem(index);
        QTreeWidgetItem* childItem = new QTreeWidgetItem( QStringList() << fileName);
        item->addChild( childItem);

        ui->fileTreeWidget->setCurrentItem(childItem);

        switch (index)
        {
        case 0:
            contractItem->sortChildren(0, Qt::AscendingOrder);
            break;
        case 1:
            registeredItem->sortChildren(0, Qt::AscendingOrder);
            break;
        case 2:
//            onChainItem->sortChildren(0, Qt::AscendingOrder);
            break;
        default:
            break;
        }
    }
    else if( tabIndex == 1)
    {
        QTreeWidgetItem* item = ui->scriptsTreeWidget->topLevelItem(index);
        QTreeWidgetItem* childItem = new QTreeWidgetItem( QStringList() << fileName);
        item->addChild( childItem);

        ui->scriptsTreeWidget->setCurrentItem(childItem);

        switch (index)
        {
        case 0:
            scriptItem->sortChildren(0, Qt::AscendingOrder);
            break;
        case 1:
            addedScriptItem->sortChildren(0, Qt::AscendingOrder);
            break;
        default:
            break;
        }
    }

}

void FileListWidget::treeWidgetSetCurrentItem(int tabIndex, int index, QString text)
{
    if( tabIndex == 0)
    {
        QTreeWidgetItem* item = ui->fileTreeWidget->topLevelItem(index);
        QTreeWidgetItem* cItem = NULL;
        int num = item->childCount();
        for( int i = 0; i < num; i++)
        {
            QTreeWidgetItem* childItem = item->child(i);
            if( childItem->text(0) == text)
            {
                cItem = childItem;
                break;
            }
        }

        if( cItem != NULL)
        {
            fileUpdating = true;
            ui->fileTreeWidget->setCurrentItem(cItem);
//            ui->fileTreeWidget->scrollToItem(cItem);
            ui->scriptsTreeWidget->setCurrentIndex(ui->fileTreeWidget->model()->index(-1,-1));   // 清除scripttreewidget的当前选中
            on_contractBtn_clicked();
            fileUpdating = false;

        }
    }
    else if( tabIndex == 1)
    {
        QTreeWidgetItem* item = ui->scriptsTreeWidget->topLevelItem(index);
        QTreeWidgetItem* cItem = NULL;
        int num = item->childCount();
        for( int i = 0; i < num; i++)
        {
            QTreeWidgetItem* childItem = item->child(i);
            if( childItem->text(0) == text)
            {
                cItem = childItem;
                break;
            }
        }

        if( cItem != NULL)
        {
            scriptUpdating = true;
            ui->fileTreeWidget->setCurrentIndex(ui->fileTreeWidget->model()->index(-1,-1));   // 清除filetreewidget的当前选中
            ui->scriptsTreeWidget->setCurrentItem(cItem);
//            ui->scriptsTreeWidget->scrollToItem(cItem);
            on_scriptBtn_clicked();
            scriptUpdating = false;
        }
    }
}

void FileListWidget::setSelectionEmpty()
{
    ui->fileTreeWidget->setCurrentIndex(ui->fileTreeWidget->model()->index(-1,-1));   // 清除filetreewidget的当前选中
    ui->scriptsTreeWidget->setCurrentIndex(ui->fileTreeWidget->model()->index(-1,-1));   // 清除scripttreewidget的当前选中
}

void FileListWidget::setCurrentIndex(int index)
{
    if( index == 0)
    {
        on_contractBtn_clicked();
    }
    else
    {
        on_scriptBtn_clicked();
    }
}

void FileListWidget::newFileSaved(QString oldPath, QString newPath)
{
    if( oldPath == newPath) return;
    if( HXChain::getInstance()->isInContracts(newPath) )
    {
        // 如果新文件保存在contracts 文件夹下  则把 fileList名字改成新名字
        QFileInfo oldFileInfo( oldPath);
        QFileInfo newFileInfo( newPath);

        FileRecord frcd;
        frcd.type = FileRecord::CONTRACT_FILE;
        frcd.path = newFileInfo.absoluteFilePath();
        frcd.newBuilt = false;
        HXChain::getInstance()->fileRecordMap.remove(oldFileInfo.absoluteFilePath());
        HXChain::getInstance()->fileRecordMap.insert(newFileInfo.absoluteFilePath(), frcd);

        getContractsList();

        treeWidgetSetCurrentItem(0,0,newFileInfo.fileName());
        contractItem->sortChildren(0, Qt::AscendingOrder);

    }
    else if( HXChain::getInstance()->isInScripts(newPath))
    {
        // 如果新文件保存在scripts 文件夹下  则把 script名字改成新名字
        QFileInfo oldFileInfo( oldPath);
        QFileInfo newFileInfo( newPath);

        FileRecord frcd;
        frcd.type = FileRecord::SCRIPT_FILE;
        frcd.path = newFileInfo.absoluteFilePath();
        frcd.newBuilt = false;
        HXChain::getInstance()->fileRecordMap.remove(oldFileInfo.absoluteFilePath());
        HXChain::getInstance()->fileRecordMap.insert(newFileInfo.absoluteFilePath(), frcd);

        getScriptsList();

        treeWidgetSetCurrentItem(1,0,newFileInfo.fileName());
        scriptItem->sortChildren(0, Qt::AscendingOrder);


    }
}

void FileListWidget::contractInfoUpdated(QString addressOrName)
{
    getRegisteredContracts();
    interfaceWidget->contractInfoUpdated(addressOrName);
}

void FileListWidget::fileChanged(QTreeWidgetItem *item, int column)
{
    if( item->parent() == contractItem)  // 如果点击的是contracts下的item
    {
        QDir dir("contracts/" + item->text(column));
        emit showFile(dir.absolutePath());
    }
    else if( item->parent() == registeredItem)
    {
        if( item->text(column).startsWith("CON") && !item->text(column).contains("."))
        {
            HXChain::getInstance()->postRPC(  "id_get_contract_info_fileListWidget", toJsonFormat("get_contract_info", QJsonArray() << item->text(column) ));
            return;
        }

        if( HXChain::getInstance()->currenChain() == 1)
        {
            // 如果是测试链
            QDir dir("registered/test/" + item->text(column));
            emit showFile(dir.absolutePath());
        }
        else if( HXChain::getInstance()->currenChain() == 2)
        {
            // 如果是正式链
            QDir dir("registered/formal/" + item->text(column));
            emit showFile(dir.absolutePath());
        }

    }
    else if( item->parent() == onChainItem)  // 如果点击的是链上合约下的item
    {
        HXChain::getInstance()->postRPC(  "id_get_contract_info_fileListWidget", toJsonFormat("get_contract_info", QJsonArray() << item->text(column) ));
    }

}

void FileListWidget::scriptChanged(QTreeWidgetItem *item, int column)
{
    if( item->parent() == scriptItem)  // 如果包含"." 则点击的是文件
    {
        QDir dir("scripts/" + item->text(column));
        emit showFile(dir.absolutePath());
    }
    else if( item->parent() == addedScriptItem)
    {
        if( HXChain::getInstance()->currenChain() == 1)
        {
            // 如果是测试链
            QDir dir("addedScripts/test/" + item->text(column));
            emit showFile(dir.absolutePath());
        }
        else if( HXChain::getInstance()->currenChain() == 2)
        {
            // 如果是正式链
            QDir dir("addedScripts/formal/" + item->text(column));
            emit showFile(dir.absolutePath());
        }

    }
}


void FileListWidget::on_tabWidget_currentChanged(int index)
{
    currentIndex = index;
}

void FileListWidget::jsonDataUpdated(QString id)
{
    if( id == "id_blockchain_get_forever_contracts")
    {
        QString result = HXChain::getInstance()->jsonDataValue(id);


        if( result.startsWith("\"result\":"))
        {
            QTreeWidgetItem* currentItem = ui->fileTreeWidget->currentItem();
            QString preText;
            int scrollBarValue = ui->fileTreeWidget->verticalScrollBar()->value();
            bool isExpanded = onChainItem->isExpanded();
            if(  currentItem != NULL)
            {
                if( currentItem->parent() == onChainItem)
                {
                    // 如果刷新之前当前项在onchainItem下 刷新之后也应该选中那个项
                    preText = currentItem->text(0);
                }
            }
            fileUpdating = true;
            treeWidgetItemRemoveAllChildren(onChainItem);
            fileUpdating = false;

            QString str = result.mid(9);
            QStringList addressInfoList = str.split("],[");
            foreach (QString addressInfo, addressInfoList)
            {
                addressInfo.remove("[");
                addressInfo.remove("]");
                addressInfo.remove("\"");

                if( !addressInfo.isEmpty())
                {
                    QStringList strList = addressInfo.split(",");
                    QString address = strList.at(0);
                    QString name    = strList.at(1);
                    ContractInfo info;
                    info.address = address;
                    info.name    = name;
                    HXChain::getInstance()->foreverContractInfoMap.insert(address,info);

                    QTreeWidgetItem* item = new QTreeWidgetItem( QStringList() << address);
                    onChainItem->addChild(item);
                }
            }

            QStringList texts = treeWidgetItemGetAllChildrenText(onChainItem);
            if( !preText.isEmpty() && texts.contains(preText) && ui->tabWidget->currentIndex() == 0)
            {
                treeWidgetSetCurrentItem(0,2, preText);
            }

            onChainItem->sortChildren(0, Qt::AscendingOrder);
            onChainItem->setExpanded(isExpanded);
            ui->fileTreeWidget->verticalScrollBar()->setValue(scrollBarValue);
        }

        return;
    }

    if( id == "id_get_contract_info_fileListWidget")
    {
        QString result = HXChain::getInstance()->jsonDataValue(id);

        if( result.startsWith("\"error\":"))
        {
            return;
        }

        ContractInfo info;

        int pos = result.indexOf("\"id\":\"") + 6;
        info.address = result.mid(pos, result.indexOf("\"", pos) - pos);

        pos = result.indexOf("\"contract_name\":\"") + 17;
        info.name = result.mid(pos, result.indexOf("\"", pos) - pos);

        pos = result.indexOf("\"description\":\"") + 15;
        info.description = result.mid(pos, result.indexOf("\"", pos) - pos);

        pos = result.indexOf("\"level\":\"") + 9;
        info.level = result.mid(pos, result.indexOf("\"", pos) - pos);

        pos = result.indexOf("\"owner\":\"") + 9;
        info.owner = result.mid(pos, result.indexOf("\"", pos) - pos);

        pos = result.indexOf("\"owner_address\":\"") + 17;
        info.ownerAddress = result.mid(pos, result.indexOf("\"", pos) - pos);

        pos = result.indexOf("\"owner_name\":\"") + 14;
        info.ownerName = result.mid(pos, result.indexOf("\"", pos) - pos);

        pos = result.indexOf("\"state\":\"") + 9;
        info.state = result.mid(pos, result.indexOf("\"", pos) - pos);

        pos = result.indexOf("\"abi\":[") + 7;
        QString abis = result.mid(pos, result.indexOf("]", pos) - pos);
        if(!abis.isEmpty())
        {
            abis.remove("\"");
            info.abiList = abis.split(",");
        }

        pos = result.indexOf("\"events\":[") + 10;
        QString events = result.mid(pos, result.indexOf("]", pos) - pos);
        if(!events.isEmpty())
        {
            events.remove("\"");
            info.eventList = events.split(",");
        }

        HXChain::getInstance()->foreverContractInfoMap.insert(info.address, info);

        emit showContract( info.address);

        return;
    }
}

void FileListWidget::treeWidgetItemRemoveAllChildren(QTreeWidgetItem *item)
{
    int num = item->childCount();
    for( int i = 0; i < num; i++)
    {
        item->removeChild( item->child(0));
    }
}

QStringList FileListWidget::treeWidgetItemGetAllChildrenText(QTreeWidgetItem *item)
{
    QStringList textList;
    int num = item->childCount();
    for( int i = 0; i < num; i++)
    {
        textList += item->child(i)->text(0);
    }

    return textList;
}

void FileListWidget::on_fileTreeWidget_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
    if( current == NULL)    return;
    if( !fileUpdating)
    {
        fileChanged( current, 0);

        ui->scriptsTreeWidget->setCurrentIndex(ui->fileTreeWidget->model()->index(-1,-1));   // 清除scripttreewidget的当前选中

    }
}

void FileListWidget::on_scriptsTreeWidget_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
    if( current == NULL)    return;
    if( !scriptUpdating)
    {
        scriptChanged( current, 0);
        ui->fileTreeWidget->setCurrentIndex(ui->fileTreeWidget->model()->index(-1,-1));   // 清除filetreewidget的当前选中
    }
}

void FileListWidget::on_contractBtn_clicked()
{
    ui->tabWidget->setCurrentIndex(0);
    ui->contractBtn->setStyleSheet( QString::fromLocal8Bit(BTN_SELECTED_STYLESHEET) );
    ui->scriptBtn->setStyleSheet( QString::fromLocal8Bit(BTN_UNSELECTED_STYLESHEET) );
}

void FileListWidget::on_scriptBtn_clicked()
{
    ui->tabWidget->setCurrentIndex(1);
    ui->contractBtn->setStyleSheet( QString::fromLocal8Bit(BTN_UNSELECTED_STYLESHEET) );
    ui->scriptBtn->setStyleSheet( QString::fromLocal8Bit(BTN_SELECTED_STYLESHEET));
}

void FileListWidget::on_fileTreeWidget_customContextMenuRequested(const QPoint &pos)
{
    QTreeWidgetItem* currentItem =  ui->fileTreeWidget->itemAt(pos);

    if( currentItem == NULL)    return;

    if( currentItem == contractItem)
    {
        ContextMenu menu(ContextMenu::ContractItemType);
        connect(&menu,SIGNAL(newFileTriggered()),this,SIGNAL(newContractFile()));
        connect(&menu,SIGNAL(pasteTriggered()),this,SLOT(paste()));
        connect(&menu,SIGNAL(importTriggered()),this,SIGNAL(importTriggered()));
        menu.exec(QCursor::pos());
        return;
    }


    if( currentItem->parent() == contractItem)
    {
        if( currentItem->text(0).endsWith(".glua"))
        {
            ContextMenu menu(ContextMenu::ContractLuaType);
            connect(&menu,SIGNAL(newFileTriggered()),this,SIGNAL(newContractFile()));
            connect(&menu,SIGNAL(copyTriggered()),this,SLOT(copy()));
            connect(&menu,SIGNAL(pasteTriggered()),this,SLOT(paste()));
            connect(&menu,SIGNAL(deleteTriggered()),this,SLOT(deleteFile()));
            connect(&menu,SIGNAL(compileTriggered()),this,SIGNAL(compileTriggered()));
            connect(&menu,SIGNAL(importTriggered()),this,SIGNAL(importTriggered()));
            menu.exec(QCursor::pos());
        }
        else if( currentItem->text(0).endsWith(".gpc"))
        {
            ContextMenu menu(ContextMenu::ContractCompiledType);
            connect(&menu,SIGNAL(newFileTriggered()),this,SIGNAL(newContractFile()));
            connect(&menu,SIGNAL(copyTriggered()),this,SLOT(copy()));
            connect(&menu,SIGNAL(pasteTriggered()),this,SLOT(paste()));
            connect(&menu,SIGNAL(deleteTriggered()),this,SLOT(deleteFile()));
            connect(&menu,SIGNAL(registerContractTriggered()),this,SIGNAL(registerContractTriggered()));
            connect(&menu,SIGNAL(importTriggered()),this,SIGNAL(importTriggered()));
            menu.exec(QCursor::pos());
        }

        return;
    }

    if( currentItem == registeredItem)  return;
    if( currentItem->parent() == registeredItem)
    {
        QDir dir;
        if( HXChain::getInstance()->currenChain() == 1)
        {
            // 如果是测试链
            dir.setPath("registered/test");
        }
        else if( HXChain::getInstance()->currenChain() == 2)
        {
            // 如果是正式链
            dir.setPath("registered/formal");
        }

        if( !HXChain::getInstance()->isContractFileUpgraded( dir.absoluteFilePath(currentItem->text(0)) ))
        {
            if( currentItem->text(0).startsWith("CON") && !currentItem->text(0).contains("."))
            {
                ContextMenu menu(ContextMenu::MyTempContractAddressType);
                connect(&menu,SIGNAL(copyTriggered()),this,SLOT(copy()));
                connect(&menu,SIGNAL(callTriggered()),this,SIGNAL(callTriggered()));
                connect(&menu,SIGNAL(upgradeTriggered()),this,SIGNAL(upgradeTriggered()));
                connect(&menu,SIGNAL(withdrawTriggered()),this,SIGNAL(withdrawTriggered()));
                connect(&menu,SIGNAL(transferTriggered()),this,SIGNAL(transferTriggered()));
                menu.exec(QCursor::pos());
            }
            else
            {
                ContextMenu menu(ContextMenu::MyTempContractPathType);
                connect(&menu,SIGNAL(copyTriggered()),this,SLOT(copy()));
                connect(&menu,SIGNAL(callTriggered()),this,SIGNAL(callTriggered()));
                connect(&menu,SIGNAL(upgradeTriggered()),this,SIGNAL(upgradeTriggered()));
                connect(&menu,SIGNAL(withdrawTriggered()),this,SIGNAL(withdrawTriggered()));
                connect(&menu,SIGNAL(transferTriggered()),this,SIGNAL(transferTriggered()));
                menu.exec(QCursor::pos());
            }


        }
        else
        {
            if( currentItem->text(0).startsWith("CON") && !currentItem->text(0).contains("."))
            {
                ContextMenu menu(ContextMenu::MyForeverContractAddressType);
                connect(&menu,SIGNAL(copyTriggered()),this,SLOT(copy()));
                connect(&menu,SIGNAL(callTriggered()),this,SIGNAL(callTriggered()));
                connect(&menu,SIGNAL(transferTriggered()),this,SIGNAL(transferTriggered()));
                menu.exec(QCursor::pos());
            }
            else
            {
                ContextMenu menu(ContextMenu::MyForeverContractPathType);
                connect(&menu,SIGNAL(copyTriggered()),this,SLOT(copy()));
                connect(&menu,SIGNAL(callTriggered()),this,SIGNAL(callTriggered()));
                connect(&menu,SIGNAL(transferTriggered()),this,SIGNAL(transferTriggered()));
                menu.exec(QCursor::pos());
            }

        }

        return;
    }

    if( currentItem == onChainItem)  return;
    if( currentItem->parent() == onChainItem)
    {
        ContextMenu menu(ContextMenu::ForeverContractType);
        connect(&menu,SIGNAL(copyTriggered()),this,SLOT(copy()));
        connect(&menu,SIGNAL(callTriggered()),this,SIGNAL(callTriggered()));
        connect(&menu,SIGNAL(transferTriggered()),this,SIGNAL(transferTriggered()));
        connect(&menu,SIGNAL(exportTriggered()),this,SIGNAL(exportTriggered()));
        menu.exec(QCursor::pos());
        return;
    }


}



void FileListWidget::on_scriptsTreeWidget_customContextMenuRequested(const QPoint &pos)
{
    QTreeWidgetItem* currentItem =  ui->scriptsTreeWidget->itemAt(pos);

    if( currentItem == NULL)    return;

    if( currentItem == scriptItem)
    {
        ContextMenu menu(ContextMenu::ScriptItemType);
        connect(&menu,SIGNAL(newFileTriggered()),this,SIGNAL(newScriptFile()));
        connect(&menu,SIGNAL(pasteTriggered()),this,SLOT(paste()));
        menu.exec(QCursor::pos());

        return;
    }

    if( currentItem->parent() == scriptItem)
    {
        if( currentItem->text(0).endsWith(".glua"))
        {
            ContextMenu menu(ContextMenu::ScriptLuaType);
            connect(&menu,SIGNAL(newFileTriggered()),this,SIGNAL(newScriptFile()));
            connect(&menu,SIGNAL(copyTriggered()),this,SLOT(copy()));
            connect(&menu,SIGNAL(pasteTriggered()),this,SLOT(paste()));
            connect(&menu,SIGNAL(deleteTriggered()),this,SLOT(deleteFile()));
            connect(&menu,SIGNAL(compileTriggered()),this,SIGNAL(compileTriggered()));
            menu.exec(QCursor::pos());
        }
        else if( currentItem->text(0).endsWith(".script"))
        {
            ContextMenu menu(ContextMenu::ScriptCompiledType);
            connect(&menu,SIGNAL(newFileTriggered()),this,SIGNAL(newScriptFile()));
            connect(&menu,SIGNAL(copyTriggered()),this,SLOT(copy()));
            connect(&menu,SIGNAL(pasteTriggered()),this,SLOT(paste()));
            connect(&menu,SIGNAL(deleteTriggered()),this,SLOT(deleteFile()));
            connect(&menu,SIGNAL(addScriptTriggered()),this,SIGNAL(addScriptTriggered()));
            menu.exec(QCursor::pos());
        }

        return;
    }

    if( currentItem == addedScriptItem)  return;
    if( currentItem->parent() == addedScriptItem)
    {
        ContextMenu menu(ContextMenu::AddedScriptType);
        connect(&menu,SIGNAL(copyTriggered()),this,SLOT(copy()));
        connect(&menu,SIGNAL(removeTriggered()),this,SIGNAL(removeTriggered()));
        connect(&menu,SIGNAL(bindTriggered()),this,SIGNAL(bindTriggered()));
        menu.exec(QCursor::pos());
        return;
    }

}

void FileListWidget::copy()
{
    if( ui->tabWidget->currentIndex() == 0)
    {
        QTreeWidgetItem* item = ui->fileTreeWidget->currentItem();

        QClipboard* cb = QApplication::clipboard();

        if( item->parent() == contractItem)  // 如果点击的是contracts下的item
        {
            QDir dir("contracts/" + item->text(0));
            QMimeData* mimeData = new QMimeData();
            QTextCodec* gbkCodec = QTextCodec::codecForName("GBK");
            QTextCodec* utfCodec = QTextCodec::codecForName("utf-8");
            QByteArray ba = QString("file:///" + dir.absolutePath()).toLocal8Bit();
            mimeData->setData("text/uri-list",utfCodec->fromUnicode( gbkCodec->toUnicode(ba)));
            cb->setMimeData(mimeData);

        }
        else if( item->parent() == registeredItem)
        {
            if( item->text(0).startsWith("CON") && !item->text(0).contains("."))
            {
                cb->setText(item->text(0));
                return;
            }

            if( HXChain::getInstance()->currenChain() == 1)
            {
                // 如果是测试链
                QDir dir("registered/test/" + item->text(0));
                QMimeData* mimeData = new QMimeData();
                QTextCodec* gbkCodec = QTextCodec::codecForName("GBK");
                QTextCodec* utfCodec = QTextCodec::codecForName("utf-8");
                QByteArray ba = QString("file:///" + dir.absolutePath()).toLocal8Bit();
                mimeData->setData("text/uri-list",utfCodec->fromUnicode( gbkCodec->toUnicode(ba)));
                cb->setMimeData(mimeData);
            }
            else if( HXChain::getInstance()->currenChain() == 2)
            {
                // 如果是正式链
                QDir dir("registered/formal/" + item->text(0));
                QMimeData* mimeData = new QMimeData();
                QTextCodec* gbkCodec = QTextCodec::codecForName("GBK");
                QTextCodec* utfCodec = QTextCodec::codecForName("utf-8");
                QByteArray ba = QString("file:///" + dir.absolutePath()).toLocal8Bit();
                mimeData->setData("text/uri-list",utfCodec->fromUnicode( gbkCodec->toUnicode(ba)));
                cb->setMimeData(mimeData);
            }

        }
        else if( item->parent() == onChainItem)  // 如果点击的是链上合约下的item
        {
            cb->setText(item->text(0));
        }

    }
    else if( ui->tabWidget->currentIndex() == 1)
    {
        QTreeWidgetItem* item = ui->scriptsTreeWidget->currentItem();
        QClipboard* cb = QApplication::clipboard();

        if( item->parent() == scriptItem)  // 如果点击的是scripts下的item
        {
            QDir dir("scripts/" + item->text(0));
            QMimeData* mimeData = new QMimeData();
            QTextCodec* gbkCodec = QTextCodec::codecForName("GBK");
            QTextCodec* utfCodec = QTextCodec::codecForName("utf-8");
            QByteArray ba = QString("file:///" + dir.absolutePath()).toLocal8Bit();
            mimeData->setData("text/uri-list",utfCodec->fromUnicode( gbkCodec->toUnicode(ba)));
            cb->setMimeData(mimeData);
        }
        else if( item->parent() == addedScriptItem)
        {
            if( HXChain::getInstance()->currenChain() == 1)
            {
                // 如果是测试链
                QDir dir("addedScripts/test/" + item->text(0));
                QMimeData* mimeData = new QMimeData();
                QTextCodec* gbkCodec = QTextCodec::codecForName("GBK");
                QTextCodec* utfCodec = QTextCodec::codecForName("utf-8");
                QByteArray ba = QString("file:///" + dir.absolutePath()).toLocal8Bit();
                mimeData->setData("text/uri-list",utfCodec->fromUnicode( gbkCodec->toUnicode(ba)));
                cb->setMimeData(mimeData);
            }
            else if( HXChain::getInstance()->currenChain() == 2)
            {
                // 如果是正式链
                QDir dir("addedScripts/formal/" + item->text(0));
                QMimeData* mimeData = new QMimeData();
                QTextCodec* gbkCodec = QTextCodec::codecForName("GBK");
                QTextCodec* utfCodec = QTextCodec::codecForName("utf-8");
                QByteArray ba = QString("file:///" + dir.absolutePath()).toLocal8Bit();
                mimeData->setData("text/uri-list",utfCodec->fromUnicode( gbkCodec->toUnicode(ba)));
                cb->setMimeData(mimeData);
            }
        }

    }


}

void FileListWidget::paste()
{
    if( ui->tabWidget->currentIndex() == 0)
    {
        QTreeWidgetItem* item = ui->fileTreeWidget->currentItem();
        if( item == contractItem || item->parent() == contractItem)
        {
            QClipboard* cb = QApplication::clipboard();

            const QMimeData* mimeData = cb->mimeData();
            if( mimeData->hasUrls())
            {
                QList<QUrl> urls = mimeData->urls();

                foreach (QUrl url, urls)
                {
                    if( url.isLocalFile())
                    {
                        QString path = url.path();
                        QString fileName = url.fileName();
                        if( path.endsWith(".glua"))
                        {
                            QFileInfo info("contracts/" + fileName);
                            if( info.exists())
                            {
                                // 如果已存在 警告
                                QMessageBox::warning(NULL, "", info.absoluteFilePath() + QString::fromLocal8Bit(" 已存在!"), QMessageBox::Ok) ;

                            }
                            else
                            {

                                if( path.startsWith("/"))  path = path.mid(1);
                                if( QFile::copy(path, info.absoluteFilePath()) )
                                {
                                    qDebug() << "paste contract succeed: " << info.absoluteFilePath();
                                }
                                else
                                {
                                    qDebug() << "paste contract fail: " << info.absoluteFilePath();
                                }
                            }

                        }
                    }
                }
                getContractsList();
            }

        }

    }
    else if( ui->tabWidget->currentIndex() == 1)
    {
        QTreeWidgetItem* item = ui->scriptsTreeWidget->currentItem();
        if( item == scriptItem || item->parent() == scriptItem)
        {
            QClipboard* cb = QApplication::clipboard();

            const QMimeData* mimeData = cb->mimeData();
            if( mimeData->hasUrls())
            {
                QList<QUrl> urls = mimeData->urls();

                foreach (QUrl url, urls)
                {
                    if( url.isLocalFile())
                    {
                        QString path = url.path();
                        QString fileName = url.fileName();
                        if( path.endsWith(".glua"))
                        {
                            QFileInfo info("scripts/" + fileName);
                            if( info.exists())
                            {
                                // 如果已存在 警告
                                QMessageBox::warning(NULL, "", info.absoluteFilePath() + QString::fromLocal8Bit(" 已存在!"), QMessageBox::Ok) ;

                            }
                            else
                            {

                                if( path.startsWith("/"))  path = path.mid(1);
                                if( QFile::copy(path, info.absoluteFilePath()) )
                                {
                                    qDebug() << "paste script succeed: " << info.absoluteFilePath();
                                }
                                else
                                {
                                    qDebug() << "paste script fail: " << info.absoluteFilePath();
                                }
                            }

                        }
                    }
                }
                getScriptsList();
            }
        }

    }

}




void FileListWidget::deleteFile()
{
    if( QMessageBox::Yes == QMessageBox::information(NULL, "", QString::fromLocal8Bit("确定删除该文件?"), QMessageBox::Yes | QMessageBox::No))
    {
        if( ui->tabWidget->currentIndex() == 0)
        {
            QTreeWidgetItem* currentItem = ui->fileTreeWidget->currentItem();
            if( currentItem->parent() == contractItem)
            {
                QDir dir("contracts");
                if( HXChain::getInstance()->fileRecordMap.value(dir.absoluteFilePath(currentItem->text(0))).newBuilt )
                {
                    // 如果是新建文件
                    emit fileDeleted(dir.absoluteFilePath(currentItem->text(0)));
                    HXChain::getInstance()->fileRecordMap.remove(dir.absoluteFilePath(currentItem->text(0)));
                    getContractsList();

                    return;
                }

                if( dir.remove(currentItem->text(0)))
                {
                    emit fileDeleted(dir.absoluteFilePath(currentItem->text(0)));
                    HXChain::getInstance()->fileRecordMap.remove(dir.absoluteFilePath(currentItem->text(0)));
                    getContractsList();
                    qDebug() << "Delete contract file succeed: " << dir.absoluteFilePath(currentItem->text(0)) ;

                    // 如果删除的是gpc文件 把ldf也删除
                    if( currentItem->text(0).endsWith(".gpc"))
                    {
                        QString ldfName = currentItem->text(0).left(currentItem->text(0).count() - 4) + ".lua.ldf";
                        qDebug() << "lllllllllllll " << ldfName;
                        dir.remove(ldfName);
                    }
                }
                else
                {
                    qDebug() << "Delete contract file failed: " << dir.absoluteFilePath(currentItem->text(0)) ;
                }
            }
        }
        else if( ui->tabWidget->currentIndex() == 1)
        {
            QTreeWidgetItem* currentItem = ui->scriptsTreeWidget->currentItem();
            if( currentItem->parent() == scriptItem)
            {
                QDir dir("scripts");
                if( HXChain::getInstance()->fileRecordMap.value(dir.absoluteFilePath(currentItem->text(0))).newBuilt )
                {
                    // 如果是新建文件
                    emit fileDeleted(dir.absoluteFilePath(currentItem->text(0)));
                    HXChain::getInstance()->fileRecordMap.remove(dir.absoluteFilePath(currentItem->text(0)));
                    getScriptsList();

                    return;
                }

                if( dir.remove(currentItem->text(0)))
                {
                    emit fileDeleted(dir.absoluteFilePath(currentItem->text(0)));
                    HXChain::getInstance()->fileRecordMap.remove(dir.absoluteFilePath(currentItem->text(0)));
                    getScriptsList();
                    qDebug() << "Delete script file succeed: " << dir.absoluteFilePath(currentItem->text(0)) ;

                    // 如果删除的是.script文件 把ldf也删除
                    if( currentItem->text(0).endsWith(".script"))
                    {
                        QString ldfName = currentItem->text(0).left(currentItem->text(0).count() - 7) + ".lua.ldf";
                        qDebug() << "lllllllllllll " << ldfName;
                        dir.remove(ldfName);
                    }
                }
                else
                {
                    qDebug() << "Delete script file failed: " << dir.absoluteFilePath(currentItem->text(0)) ;
                }
            }
        }

    }
}
