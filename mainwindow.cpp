#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "hxchain.h"
#include "rpcthread.h"
#include "normallogin.h"
#include "filelistwidget.h"
#include "interfacewidget.h"
#include "contentwidget.h"
#include "outputwidget.h"
#include "selectpathwidget.h"
#include "firstlogin.h"
#include "waitingforsync.h"
#include "dialog/accountlistdialog.h"
#include "dialog/consoledialog.h"
#include "dialog/registercontractdialog.h"
#include "dialog/transferdialog.h"
#include "dialog/upgradedialog.h"
#include "dialog/calldialog.h"
#include "dialog/withdrawdialog.h"
#include "dialog/addscriptdialog.h"
#include "dialog/binddialog.h"
#include "dialog/removescriptdialog.h"
#include "dialog/transfertoaccountdialog.h"
#include "aceeditor.h"
#include "ExeManager.h"
#include "compile/CompileManager.h"

#include <QSplitter>
#include <QDebug>
#include <QMessageBox>
#include <QFileDialog>
#include <QDesktopServices>
#include <QFile>
#include <QTextStream>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    formalWalletOpened(false),
    testImportDelegatesStarted(false),
    timerForFormalLaunch(NULL),
    formalExeLaunched(false),
    timerForAutoRefresh(NULL),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowTitle( QString::fromLocal8Bit("测试链-IDE"));

    connect(HXChain::getInstance(),SIGNAL(jsonDataUpdated(QString)),this,SLOT(jsonDataUpdated(QString)));

    hide();

    if( !HXChain::getInstance()->configFile->contains("/settings/hxchainPath") )
    {
        showSelectPathWidget();
    }
    else
    {
        QString path = HXChain::getInstance()->configFile->value("/settings/hxchainPath").toString();
        QDir dir(path);
        QDir dir2(path + "/wallets");
        QDir dir3(path + "/chain");

        // TODO 没有正式链 临时注释掉
//        if( !dir.exists() || ( !dir2.exists() && !dir3.exists())  )  // 如果数据文件被删除了
//        {
//            showSelectPathWidget();
//        }
//        else
//        {
            HXChain::getInstance()->appDataPath = HXChain::getInstance()->configFile->value("/settings/hxchainPath").toString();
            startHXChain();
//        }
    }


    ui->menuBar->move(50,0);

    ui->changeToFormalChainAction->setEnabled(false);
    ui->changeToTestChainAction->setEnabled(false);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::jsonDataUpdated(QString id)
{
    if( id == "id_open")
    {
        if( HXChain::getInstance()->jsonDataValue(id) == "\"result\":null")
        {
            HXChain::getInstance()->postRPC(  "id_unlock_testchain", toJsonFormat("unlock", QJsonArray() << "99999999" << "12345678" ), 1);

        }
        else
        {
            HXChain::getInstance()->postRPC(  "id_create_testchain", toJsonFormat("create", QJsonArray() << "wallet" << "12345678" ), 1);

        }

        //        waitingForSync->close();
        return;
    }

    if( id == "id_unlock_testchain")
    {
        QString result = HXChain::getInstance()->jsonDataValue( id);

        if( result == "\"result\":null")
        {
            alreadyLogin();
            waitingForSync->close();
        }

        return;
    }

    if( id == "id_create_testchain")
    {
        QString result = HXChain::getInstance()->jsonDataValue( id);

        if( result == "\"result\":null")
        {
            alreadyLogin();
            waitingForSync->close();
        }

        return;
    }

    if( id == "id_open_changetoformal")
    {
        if( HXChain::getInstance()->jsonDataValue(id) == "\"result\":null")
        {
            NormalLogin* normalLogin2 = new NormalLogin(2);
            normalLogin2->setWindowTitle( QString::fromLocal8Bit("IDE"));
            normalLogin2->setWindowModality(Qt::ApplicationModal);
            normalLogin2->setAttribute(Qt::WA_DeleteOnClose);
            connect( normalLogin2,SIGNAL(login()), this, SLOT(formalWalletLogin()));

            normalLogin2->show();
        }
        else
        {
            FirstLogin* firstLogin2 = new FirstLogin(2);
            firstLogin2->setWindowTitle( QString::fromLocal8Bit("IDE"));
            firstLogin2->setWindowModality(Qt::ApplicationModal);
            firstLogin2->setAttribute(Qt::WA_DeleteOnClose);
            connect( firstLogin2,SIGNAL(login()), this, SLOT(formalWalletLogin()));

            firstLogin2->show();
        }

        return;
    }


    if( id == "id_wallet_list_my_addresses")
    {
        QString result = HXChain::getInstance()->jsonDataValue(id);

        if( result.startsWith("\"error\":"))  return;

        HXChain::getInstance()->accountInfoMap.clear();
        if( result == "\"result\":[]")
        {
            // 如果还没有账号
            //            on_accountListAction_triggered();
            if( HXChain::getInstance()->currenChain() == 1 && testImportDelegatesStarted == false)
            {
                importTestDelegateAccounts();
            }
            return;
        }

        QStringList accountInfoList = result.split("},{");

        foreach (QString str, accountInfoList)
        {
            int pos = str.indexOf("\"name\":\"") + 8;
            if( pos == 7)  break;   // 如果还没有账号

            QString accountName = str.mid(pos, str.indexOf("\",", pos) - pos);
            AccountInfo info;
            info.name = accountName;

            if( str.contains("\"delegate_info\":"))
            {
                info.isDelegate = true;
            }
            else
            {
                info.isDelegate = false;
            }

            int pos2 = str.indexOf( "\"owner_address\":\"") + 17;
            QString addr = str.mid( pos2, str.indexOf( "\"", pos2) - pos2);
            info.address = addr;

            int pos3 = str.indexOf( "\"registration_date\":") + 21;
            QString registerTime = str.mid( pos3, str.indexOf( "\"", pos3) - pos3);
            if( registerTime != "1970-01-01T00:00:00")
            {
                info.registerTime = registerTime;
            }
            else
            {
                info.registerTime = "NO";
            }

            int pos4 = str.indexOf( "\"owner_key\":\"") + 13;
            QString ownerKey = str.mid( pos4, str.indexOf( "\"", pos4) - pos4);
            info.ownerKey = ownerKey;


            HXChain::getInstance()->accountInfoMap.insert( accountName, info);
            HXChain::getInstance()->ownerKeyAccountNameMap.insert(ownerKey,accountName);
        }

        if( HXChain::getInstance()->currentAccount.isEmpty())
        {
            // 默认是 map中第一个账户
//            HXChain::getInstance()->currentAccount = HXChain::getInstance()->accountInfoMap.keys().at(0);
             HXChain::getInstance()->setCurrentAccount(HXChain::getInstance()->accountInfoMap.keys().at(0));
        }
        else if( !HXChain::getInstance()->accountInfoMap.keys().contains(HXChain::getInstance()->currentAccount))
        {
            // 如果当前账户被自动重命名localxxx 或被删除 当前账户换为map中第一个账户
//            HXChain::getInstance()->currentAccount = HXChain::getInstance()->accountInfoMap.keys().at(0);
            HXChain::getInstance()->setCurrentAccount(HXChain::getInstance()->accountInfoMap.keys().at(0));

            currentAccountChanged(HXChain::getInstance()->currentAccount);
        }

        HXChain::getInstance()->postRPC(  "id_wallet_account_balance",toJsonFormat( "wallet_account_balance", QJsonArray()));

        if( HXChain::getInstance()->currenChain() == 1)
        {
            if( !HXChain::getInstance()->accountInfoMap.keys().contains("test9") && testImportDelegatesStarted == false)
            {
                // 如果是测试链 如果还没有test98 账户 执行脚本导入代理账户
                importTestDelegateAccounts();
            }
        }


        return;
    }


    if( id == "id_wallet_account_balance" )
    {
        QString result = HXChain::getInstance()->jsonDataValue(id);

        QStringList accountList = HXChain::getInstance()->accountInfoMap.keys();
        foreach (QString name, accountList)
        {

            int p = result.indexOf( "[\"" + name + "\",");
            if( p != -1)  // 如果balance中存在
            {
                int pos = p + 8 + name.size();
                QString str = result.mid( pos, result.indexOf( "]", pos) - pos);
                str.remove("\"");

                double amount = str.toDouble() / 100000;

                AccountInfo info = HXChain::getInstance()->accountInfoMap.value(name);
                info.balance = QString::number(amount, 'f', 5) + " ALP";
                HXChain::getInstance()->accountInfoMap.insert(name, info);
            }
            else
            {
                AccountInfo info = HXChain::getInstance()->accountInfoMap.value(name);
                info.balance = "0.00000 ALP";
                HXChain::getInstance()->accountInfoMap.insert(name, info);
            }
        }

        emit accountInfoUpdated();

        return;
    }

    if( id == "id_wallet_get_contracts")
    {
        QString result = HXChain::getInstance()->jsonDataValue(id);

        if( result == "\"result\":[]")    return;

        result = result.mid(9);
        result.remove("[");
        result.remove("]");
        result.remove("\"");
        QStringList contracts = result.split(",");

        HXChain::getInstance()->myContracts.clear();
        foreach (QString contract, contracts)
        {
            HXChain::getInstance()->myContracts.append(contract);
            HXChain::getInstance()->postRPC(  "id_get_contract_info_" + contract, toJsonFormat("get_contract_info", QJsonArray() << contract  ));
        }

        return;
    }

    if( id.startsWith( "id_get_contract_info_") )
    {
        QString result = HXChain::getInstance()->jsonDataValue(id);

        if( result.startsWith("\"error\":"))
        {
            //            QMessageBox::critical(NULL, "", QString::fromLocal8Bit("合约不存在!"), QMessageBox::Ok);
            return;
        }

        QString address;

        ContractInfo info;

        int pos = result.indexOf("\"id\":\"") + 6;
        info.address = result.mid(pos, result.indexOf("\"", pos) - pos);

        if( id == "id_get_contract_info_fileListWidget")
        {
            address = info.address;
        }
        else
        {
            address = id.mid(21);
        }

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

        HXChain::getInstance()->contractInfoMap.insert(address, info);

        emit contractInfoUpdated(address);


        QStringList keys = HXChain::getInstance()->specialOperationMap.keys();
        if( keys.contains(info.address))
        {
            if( info.level == "forever" )
            {
                foreach (QString key, keys)
                {
                    QString operation = HXChain::getInstance()->specialOperationMap.value(key);
                    if( operation == "upgrading")
                    {
                        HXChain::getInstance()->specialOperationMap.remove(key);
                    }
                }
            }
        }



        return;
    }

    if( id.startsWith("id_compile_contract_") )
    {
        QString result = HXChain::getInstance()->jsonDataValue(id);

        if( result.startsWith("\"result\":\""))
        {
            int pos = result.indexOf("\"result\":\"") + 10;
            QString gpcPath = result.mid(pos, result.indexOf("\"",pos) - pos);


            QFileInfo info(gpcPath);
            //            QStringList filesPath = HXChain::getInstance()->fileRecordMap.keys();
            //            if( !filesPath.contains( info.absoluteFilePath()) )
            //            {
            //                FileRecord frcd;
            //                frcd.type = FileRecord::CONTRACT_FILE;
            //                frcd.path = info.absoluteFilePath();
            //                frcd.newBuilt = false;
            //                HXChain::getInstance()->fileRecordMap.insert( info.absoluteFilePath(), frcd);

            //                fileListWidget->addFile(0,0,info.fileName());
            //            }

            fileListWidget->getContractsList();

            if( contentWidget->pathAceEditorMap.contains(info.absoluteFilePath()))
            {
                contentWidget->closeFile(info.absoluteFilePath());
            }
            this->showFile(info.absoluteFilePath());
            fileListWidget->treeWidgetSetCurrentItem(0,0, info.fileName());
        }

        return;
    }

    if( id.startsWith("id_compile_script_") )
    {
        QString result = HXChain::getInstance()->jsonDataValue(id);

        if( result.startsWith("\"result\":\""))
        {
            int pos = result.indexOf("\"result\":\"") + 10;
            QString scriptPath = result.mid(pos, result.indexOf("\"",pos) - pos);


            QFileInfo info(scriptPath);
            //            QStringList filesPath = HXChain::getInstance()->fileRecordMap.keys();
            //            if( !filesPath.contains( info.absoluteFilePath()) )
            //            {
            //                FileRecord frcd;
            //                frcd.type = FileRecord::SCRIPT_FILE;
            //                frcd.path = info.absoluteFilePath();
            //                frcd.newBuilt = false;
            //                HXChain::getInstance()->fileRecordMap.insert( info.absoluteFilePath(), frcd);

            //                fileListWidget->addFile(1,0,info.fileName());
            //            }

            fileListWidget->getScriptsList();

            if( contentWidget->pathAceEditorMap.contains(info.absoluteFilePath()))
            {
                contentWidget->closeFile(info.absoluteFilePath());
            }
            this->showFile(info.absoluteFilePath());
            fileListWidget->treeWidgetSetCurrentItem(1,0,info.fileName());
        }

        return;
    }

    if( id == "id_sandbox_open")
    {
        QString result = HXChain::getInstance()->jsonDataValue(id);

        HXChain::getInstance()->isInSandBox = true;
        checkSandboxActionEnable(true);

        if( HXChain::getInstance()->currenChain() == 1)
        {
            setWindowTitle( QString::fromLocal8Bit("测试链（沙盒模式）-IDE"));
        }
        else
        {
            setWindowTitle( QString::fromLocal8Bit("正式链（沙盒模式）-IDE"));
        }

        return;
    }

    if( id == "id_sandbox_close")
    {
        QString result = HXChain::getInstance()->jsonDataValue(id);

        HXChain::getInstance()->isInSandBox = false;
        checkSandboxActionEnable(false);

        if( HXChain::getInstance()->currenChain() == 1)
        {
            setWindowTitle( QString::fromLocal8Bit("测试链-IDE"));
        }
        else
        {
            setWindowTitle( QString::fromLocal8Bit("正式链-IDE"));
        }

        HXChain::getInstance()->clearSandboxOperation();
        contentWidget->closeSandboxFile();
        //        fileListWidget->getRegisteredContracts();
        fileListWidget->getAddedScripts();
        getAccountInfo();
        getAllAccountsContract();

        return;
    }

    if( id.startsWith("id_load_contract_to_file_") )
    {
        QString result = HXChain::getInstance()->jsonDataValue(id);

        if( result.startsWith("\"result\":"))
        {
            QMessageBox::information(NULL, "", QString::fromLocal8Bit("导出成功!"), QMessageBox::Ok);

            QString filePath = id.mid(25);
            QFileInfo info(filePath);
            QDir dir("contracts");

            if( info.absolutePath() != dir.absolutePath())
            {
                // 如果生成的字节码文件不在 contracts文件夹下
                return;
            }

            fileListWidget->getContractsList();

            if( contentWidget->pathAceEditorMap.contains(filePath))
            {
                contentWidget->closeFile(filePath);
            }
            showFile(filePath);
            fileListWidget->treeWidgetSetCurrentItem(0,0, info.fileName());

        }
        else
        {
            if( result.mid(0,8) == "\"error\":")
            {
                int pos = result.indexOf("\"message\":\"") + 11;
                QString errorMessage = result.mid(pos, result.indexOf("\"", pos) - pos);

                QMessageBox::critical(NULL, "", "导出失败 : " + errorMessage, QMessageBox::Ok);
            }
        }


        return;
    }

    if( id.startsWith("id_wallet_transfer_to_contract_"))
    {
        getAccountInfo();
        return;
    }

    if( id == "id_call_contract")
    {
        getAccountInfo();
        return;
    }

    if( id == "id_list_scripts")
    {
        QString result = HXChain::getInstance()->jsonDataValue(id);

        if( result == "\"result\":[]")    return;

        if( result.startsWith("\"result\":"))
        {
            QStringList scriptList = result.split("},{");

            foreach (QString script, scriptList)
            {
                ScriptInfo info;

                int pos = script.indexOf("\"id\":\"") + 6;
                info.scriptId = script.mid(pos, script.indexOf("\"", pos) - pos);

                pos = script.indexOf("\"description\":\"") + 15;
                info.description = script.mid(pos, script.indexOf("\"", pos) - pos);

                HXChain::getInstance()->scriptInfoMap.insert(info.scriptId,info);
            }

        }

        return;
    }
}

void MainWindow::alreadyLogin()
{
    this->showMaximized();

    horizontalSplitter = new QSplitter(Qt::Horizontal, this);

    this->setCentralWidget( horizontalSplitter );

    fileListWidget = new FileListWidget( horizontalSplitter);

    verticalSplitter2 = new QSplitter(Qt::Vertical, horizontalSplitter);
    contentWidget = new ContentWidget( verticalSplitter2);
    outputWidget = new OutputWidget( verticalSplitter2);
    connect( HXChain::getInstance()->currentProcess(1), SIGNAL(readyReadStandardOutput()), outputWidget, SLOT(testHasOutputToRead()));
    connect( HXChain::getInstance()->currentProcess(2), SIGNAL(readyReadStandardOutput()), outputWidget, SLOT(formalHasOutputToRead()));
    //链接编译槽
    connect(HXChain::getInstance()->compileManager,&CompileManager::CompileOutput,std::bind(&OutputWidget::appendText,outputWidget,0,std::placeholders::_1));

    QList<int> widgetSize1;
    widgetSize1 << 220 << 780;
    horizontalSplitter->setSizes(widgetSize1);

    QList<int> widgetSize3;
    widgetSize3 << 450 << 250;
    verticalSplitter2->setSizes(widgetSize3);

    //设置风格
    SetIDETheme(DataDefine::Black_Theme);

    horizontalSplitter->setHandleWidth(10);
    //    verticalSplitter1->setHandleWidth(10);
    verticalSplitter2->setHandleWidth(10);

    connect( fileListWidget, SIGNAL(showFile(QString)), this, SLOT(showFile(QString)));
    connect( fileListWidget, SIGNAL(showContract(QString)), this, SLOT(showContract(QString)));
    connect( fileListWidget, SIGNAL(newContractFile()), this, SLOT(on_newContractAction_triggered()));
    connect( fileListWidget, SIGNAL(newScriptFile()), this, SLOT(on_newScriptAction_triggered()));
    connect( fileListWidget, SIGNAL(fileDeleted(QString)), contentWidget, SLOT(onFileDeleted(QString)));
    connect( fileListWidget, SIGNAL(importTriggered()), this, SLOT(on_importAction_triggered()));
    connect( fileListWidget, SIGNAL(exportTriggered()), this, SLOT(on_exportAction_triggered()));
    connect( fileListWidget, SIGNAL(compileTriggered()), this, SLOT(on_compileAction_triggered()));
    connect( fileListWidget, SIGNAL(registerContractTriggered()), this, SLOT(on_registerAction_triggered()));
    connect( fileListWidget, SIGNAL(callTriggered()), this, SLOT(on_callAction_triggered()));
    connect( fileListWidget, SIGNAL(upgradeTriggered()), this, SLOT(on_upgradeAction_triggered()));
    connect( fileListWidget, SIGNAL(withdrawTriggered()), this, SLOT(on_withdrawAction_triggered()));
    connect( fileListWidget, SIGNAL(transferTriggered()), this, SLOT(on_transferAction_triggered()));
    connect( fileListWidget, SIGNAL(addScriptTriggered()), this, SLOT(on_addScriptAction_triggered()));
    connect( fileListWidget, SIGNAL(removeTriggered()), this, SLOT(on_removeScriptAction_triggered()));
    connect( fileListWidget, SIGNAL(bindTriggered()), this, SLOT(on_bindAction_triggered()));
    connect( this, SIGNAL(contractInfoUpdated(QString)), fileListWidget, SLOT(contractInfoUpdated(QString)));
    connect( contentWidget, SIGNAL(newFileSaved(QString,QString)), fileListWidget, SLOT(newFileSaved(QString,QString)));
    connect( contentWidget, SIGNAL(textChanged()), this, SLOT(contentWidgetTextChanged()));
    connect( contentWidget, SIGNAL(fileSelected(QString)), this, SLOT(fileSelected(QString)));
    connect( this, SIGNAL(contractInfoUpdated(QString)), contentWidget, SLOT(contractInfoUpdated(QString)));

    connect(HXChain::getInstance()->compileManager,&CompileManager::finishCompile,this,&MainWindow::CompileFinishSlot);

    HXChain::getInstance()->postRPC(  "id_delegate_set_network_min_connection_count", toJsonFormat("delegate_set_network_min_connection_count", QJsonArray() << "0"));

    actionSetIcon();

    getAccountInfo();
    getAllAccountsContract();
    getAllScripts();

    startTimerForAutoRefresh();

    fileSelected("");

    //checkChangeChainActionEnable(1);
    checkSandboxActionEnable(false);
    ui->newScriptAction->setEnabled(false);
}

void MainWindow::startHXChain()
{
    showWaitingForSyncWidget();

    // 沙盒模式下退出  下次启动将不再是沙盒模式 需清除沙盒下的操作
    HXChain::getInstance()->clearSandboxOperation();

//    // 启动测试链  测试链数据路径是正式链路径下testchain文件夹
//    QStringList strList;
//    strList << "--data-dir" << ( HXChain::getInstance()->configFile->value("/settings/cchainPath").toString() + "/testchain" )
//            << "--rpcuser" << "a" << "--rpcpassword" << "b" << "--rpcport" << QString::number( RPC_PORT + 1) << "--server";
//    HXChain::getInstance()->testProcess->start("wallet-c_test.exe",strList );

//    if( HXChain::getInstance()->testProcess->waitForStarted())
//    {
//        qDebug() << "laungh wallet-c_test.exe succeeded";
//        //         qDebug()  <<     HXChain::getInstance()->read(HXChain::getInstance()->testProcess);
//        timerForTestLaunch = new QTimer(this);
//        connect(timerForTestLaunch,SIGNAL(timeout()),this,SLOT(readTestExe()));
//        timerForTestLaunch->start(1000);
//    }
//    else
//    {
//        qDebug() << "laungh wallet-c_test.exe failed";
//    }


//    // 启动正式链
//    QStringList strList2;
//    strList2 << "--data-dir" << HXChain::getInstance()->configFile->value("/settings/cchainPath").toString()
//             << "--rpcuser" << "a" << "--rpcpassword" << "b" << "--rpcport" << QString::number( RPC_PORT) << "--server";
//    HXChain::getInstance()->formalProcess->start("wallet-c.exe",strList2 );

//    if( HXChain::getInstance()->formalProcess->waitForStarted())
//    {
//        //        qDebug() << "laungh wallet-c.exe succeeded";
//        //         qDebug()  <<     HXChain::getInstance()->read(HXChain::getInstance()->formalProcess);

//        timerForFormalLaunch = new QTimer(this);
//        connect(timerForFormalLaunch,SIGNAL(timeout()),this,SLOT(readFormalExe()));
//        timerForFormalLaunch->start(1000);
//    }
//    else
//    {
//        qDebug() << "laungh wallet-c.exe failed";
//    }

    //启动client ， node
    connect(HXChain::getInstance()->testManager,&ExeManager::exeStarted,this,&MainWindow::exeStartedSlots);
    connect(HXChain::getInstance()->formalManager,&ExeManager::exeStarted,this,&MainWindow::exeStartedSlots);
    HXChain::getInstance()->testManager->startExe();
    HXChain::getInstance()->formalManager->startExe();

}

void MainWindow::startFormalHXChain()
{

}

void MainWindow::contractRegistered(QString path)
{
    QFileInfo info(path);
    fileListWidget->addFile(0,1, info.fileName());

    getAccountInfo();
}

void MainWindow::scriptAdded(QString path)
{
    getAllScripts();
    QMessageBox::information(NULL, "", "Script added : " + path, QMessageBox::Ok);

    QFileInfo info(path);
    fileListWidget->addFile(1,1, info.fileName());
}

void MainWindow::fileSelected(QString path)
{
    if( path.isEmpty())
    {
        fileListWidget->setSelectionEmpty();
    }

    QFileInfo info(path);
    if( HXChain::getInstance()->isInContracts(path))
    {
        fileListWidget->treeWidgetSetCurrentItem(0,0,info.fileName());
    }
    else if( HXChain::getInstance()->isInScripts(path))
    {
        fileListWidget->treeWidgetSetCurrentItem(1,0,info.fileName());
    }
    else if( HXChain::getInstance()->isContractFileRegistered(path))
    {
        fileListWidget->treeWidgetSetCurrentItem(0,1,info.fileName());
    }
    else if( HXChain::getInstance()->isScriptAdded(path))
    {
        fileListWidget->treeWidgetSetCurrentItem(1,1,info.fileName());
    }

    if( path.startsWith("CON") && !path.contains("."))
    {
        QStringList textList = fileListWidget->treeWidgetItemGetAllChildrenText(fileListWidget->registeredItem);
        if( textList.contains(path))
        {
            // 如果在我的合约里有该合约address  则filelist选中我的合约里面的
            fileListWidget->treeWidgetSetCurrentItem(0,1,path);
        }
        else
        {
            // 否则选中合约库里面的
            fileListWidget->treeWidgetSetCurrentItem(0,2,path);
        }

        fileListWidget->interfaceWidget->setContract(path);

        ui->exportAction->setEnabled(true);
        ui->importAction->setEnabled(true);
        ui->addScriptAction->setEnabled(false);
        ui->removeScriptAction->setEnabled(false);
        ui->bindAction->setEnabled(false);
        ui->compileAction->setEnabled(false);
        ui->registerAction->setEnabled(false);
        ui->upgradeAction->setEnabled(false);
        ui->withdrawAction->setEnabled(false);
        ui->closeAction->setEnabled(true);

        ui->saveAction->setEnabled( contentWidget->currentFileUnsaved());
        ui->saveAllAction->setEnabled( contentWidget->hasFileUnsaved());

        return;
    }
    else
    {
        checkActionEnable(path);
    }

    QString address =  HXChain::getInstance()->configGetContractAddress(path);
    if(address.startsWith("CON") )
    {
        fileListWidget->interfaceWidget->setContract(address);
    }
    else
    {
        // 如果是脚本
        address = HXChain::getInstance()->configGetScriptId(path);
        if( address.startsWith("SCR"))
        {
            fileListWidget->interfaceWidget->setScript(address);
        }
        else
        {
            fileListWidget->interfaceWidget->setContract("");
        }
    }

}

void MainWindow::showSelectPathWidget()
{
    SelectPathWidget* selectPathWidget = new SelectPathWidget;
    selectPathWidget->setWindowTitle( QString::fromLocal8Bit("IDE"));
    selectPathWidget->setAttribute(Qt::WA_DeleteOnClose);
    connect( selectPathWidget,SIGNAL(enter()),this,SLOT(startHXChain()));

    selectPathWidget->show();
}

void MainWindow::showWaitingForSyncWidget()
{
    waitingForSync = new WaitingForSync;
    waitingForSync->setWindowTitle( QString::fromLocal8Bit("IDE"));
    waitingForSync->setAttribute(Qt::WA_DeleteOnClose);

    waitingForSync->show();
}

void MainWindow::importTestDelegateAccounts()
{
    QFileInfo info("test_init_delegate_keys.json");
    if( info.exists())
    {
        testImportDelegatesStarted = true;
        HXChain::getInstance()->write( "execute_script \"" + info.absoluteFilePath() + "\"\n");
        outputWidget->appendText(1, QString::fromLocal8Bit("测试链初始化中，正在导入99个初始代理账户..."));
        //        HXChain::getInstance()->postRPC( toJsonFormat( "id_execute_script", "execute_script", QStringList() << info.absoluteFilePath()));
    }

}

void MainWindow::checkActionEnable(QString path)
{
    contentWidgetTextChanged();     // 能不能撤销/恢复

    ui->exportAction->setEnabled(false);
    ui->importAction->setEnabled(true);

    ui->saveAction->setEnabled( contentWidget->currentFileUnsaved());
    ui->saveAllAction->setEnabled( contentWidget->hasFileUnsaved());


    if( path.isEmpty())
    {
        ui->closeAction->setEnabled(false);

        ui->addScriptAction->setEnabled(false);
        ui->removeScriptAction->setEnabled(false);
        ui->bindAction->setEnabled(false);
        ui->compileAction->setEnabled(false);
        ui->registerAction->setEnabled(false);
        ui->upgradeAction->setEnabled(false);
        ui->withdrawAction->setEnabled(false);
    }
    else
    {
        ui->closeAction->setEnabled(true);
    }

    //    qDebug() << HXChain::getInstance()->isInContracts(path);
    //    qDebug() << HXChain::getInstance()->isInScripts(path);
    //    qDebug() << HXChain::getInstance()->isContractFileRegistered(path) << HXChain::getInstance()->isContractFileUpgraded(path);
    //    qDebug() << HXChain::getInstance()->isScriptAdded(path);
    if( HXChain::getInstance()->isInContracts(path))
    {
        ui->addScriptAction->setEnabled(false);
        ui->removeScriptAction->setEnabled(false);
        ui->bindAction->setEnabled(false);
        ui->upgradeAction->setEnabled(false);
        ui->withdrawAction->setEnabled(false);

        if( path.endsWith(".gpc"))
        {
            ui->compileAction->setEnabled(false);
            ui->registerAction->setEnabled(true);
        }
        else
        {
            ui->compileAction->setEnabled(true);
            ui->registerAction->setEnabled(false);
        }
    }
    else if( HXChain::getInstance()->isInScripts(path))
    {
        ui->upgradeAction->setEnabled(false);
        ui->withdrawAction->setEnabled(false);
        ui->removeScriptAction->setEnabled(false);
        ui->bindAction->setEnabled(false);
        ui->registerAction->setEnabled(false);

        if( path.endsWith(".script"))
        {
            ui->compileAction->setEnabled(false);
            ui->addScriptAction->setEnabled(true);
        }
        else if( path.endsWith(".glua"))
        {
            ui->compileAction->setEnabled(true);
            ui->addScriptAction->setEnabled(false);
        }
    }
    else if( HXChain::getInstance()->isContractFileRegistered(path))
    {
        ui->addScriptAction->setEnabled(false);
        ui->removeScriptAction->setEnabled(false);
        ui->bindAction->setEnabled(false);
        ui->compileAction->setEnabled(false);
        ui->registerAction->setEnabled(false);

        if( HXChain::getInstance()->isContractFileUpgraded(path))
        {
            ui->upgradeAction->setEnabled(false);
            ui->withdrawAction->setEnabled(false);
        }
        else
        {
            ui->upgradeAction->setEnabled(true);
            ui->withdrawAction->setEnabled(true);
        }
    }
    else if( HXChain::getInstance()->isScriptAdded(path))
    {
        ui->addScriptAction->setEnabled(false);
        ui->removeScriptAction->setEnabled(true);
        ui->bindAction->setEnabled(true);
        ui->compileAction->setEnabled(false);
        ui->registerAction->setEnabled(false);
        ui->upgradeAction->setEnabled(false);
        ui->withdrawAction->setEnabled(false);
    }
}

void MainWindow::checkChangeChainActionEnable(int chainType)
{
    if( chainType == 1)
    {
        ui->changeToFormalChainAction->setEnabled(true);
        ui->changeToTestChainAction->setEnabled(false);
    }
    else
    {
        ui->changeToFormalChainAction->setEnabled(false);
        ui->changeToTestChainAction->setEnabled(true);
    }
}

void MainWindow::checkSandboxActionEnable(bool isInSandbox)
{
    if( isInSandbox)
    {
        ui->enterSandboxAction->setEnabled(false);
        ui->exitSandboxAction->setEnabled(true);
        ui->transferToAccountAction->setEnabled(false);
    }
    else
    {
        ui->enterSandboxAction->setEnabled(true);
        ui->exitSandboxAction->setEnabled(false);
        ui->transferToAccountAction->setEnabled(true);
    }
}

void MainWindow::specialOperationsWhenAutoRefresh()
{
    QStringList keys = HXChain::getInstance()->specialOperationMap.keys();
    foreach (QString key, keys)
    {
        QString operation = HXChain::getInstance()->specialOperationMap.value(key);
        if( operation == "upgrading")
        {
            HXChain::getInstance()->postRPC(  "id_get_contract_info_" + key, toJsonFormat("get_contract_info", QJsonArray() << key  ));
        }
    }
}

void MainWindow::actionSetIcon()
{
    ui->newContractAction->setIcon(QIcon(":/pic2/newContractIcon.png"));
    ui->importAction->setIcon(QIcon(":/pic2/importIcon.png"));
    ui->exportAction->setIcon(QIcon(":/pic2/exportIcon.png"));
    ui->saveAction->setIcon(QIcon(":/pic2/saveIcon.png"));
    ui->consoleAction->setIcon(QIcon(":pic2/consoleIcon.png"));
    ui->accountListAction->setIcon(QIcon(":pic2/accountListIcon.png"));
    ui->transferToAccountAction->setIcon(QIcon(":pic2/transferToAccountIcon.png"));
    ui->helpAction->setIcon(QIcon(":pic2/helpIcon.png"));
    ui->saveAllAction->setIcon(QIcon(":pic2/saveAllIcon.png"));
    ui->undoAction->setIcon(QIcon(":/pic2/undoIcon.png"));
    ui->redoAction->setIcon(QIcon(":/pic2/redoIcon.png"));
    ui->compileAction->setIcon(QIcon(":/pic2/compileIcon.png"));
    ui->registerAction->setIcon(QIcon(":/pic2/registerContractIcon.png"));
    ui->callAction->setIcon(QIcon(":/pic2/callContractIcon.png"));
    ui->upgradeAction->setIcon(QIcon(":/pic2/upgradeContractIcon.png"));
    ui->withdrawAction->setIcon(QIcon(":/pic2/withdrawContractIcon.png"));
    ui->bindAction->setIcon(QIcon(":/pic2/bindScriptIcon.png"));
    ui->enterSandboxAction->setIcon(QIcon(":/pic2/enterSandboxIcon.png"));
    ui->changeToFormalChainAction->setIcon(QIcon(":/pic2/changeToFormalChainIcon.png"));
    ui->addScriptAction->setIcon(QIcon(":/pic2/addScriptIcon.png"));
    ui->removeScriptAction->setIcon(QIcon(":/pic2/removeScriptIcon.png"));
    ui->transferAction->setIcon(QIcon(":/pic2/transferToContractIcon.png"));
    ui->newScriptAction->setIcon(QIcon(":/pic2/newScriptIcon.png"));
    ui->exitSandboxAction->setIcon(QIcon(":/pic2/exitSandboxIcon.png"));
    ui->changeToTestChainAction->setIcon(QIcon(":/pic2/changeToTestChainIcon.png"));

}

void MainWindow::closeEvent(QCloseEvent* e)
{
    if(contentWidget->closeAll())
    {
        QMainWindow::closeEvent(e);
    }
    else
    {
        e->ignore();
    }
}

void MainWindow::exeStartedSlots()
{
    if(HXChain::getInstance()->testManager->exeRunning() && HXChain::getInstance()->formalManager->exeRunning())
    {
       disconnect(HXChain::getInstance()->testManager,&ExeManager::exeStarted,this,&MainWindow::exeStartedSlots);
       disconnect(HXChain::getInstance()->formalManager,&ExeManager::exeStarted,this,&MainWindow::exeStartedSlots);
       if(waitingForSync)
       {
           waitingForSync->close();
       }
       alreadyLogin();
    }
}

void MainWindow::getAccountInfo()
{
    HXChain::getInstance()->postRPC(  "id_wallet_list_my_addresses", toJsonFormat("wallet_list_my_addresses", QJsonArray()));
}

void MainWindow::getAllAccountsContract()
{
    HXChain::getInstance()->postRPC(  "id_wallet_get_contracts", toJsonFormat("wallet_get_contracts", QJsonArray()));
}

void MainWindow::getAllScripts()
{
    HXChain::getInstance()->postRPC(  "id_list_scripts", toJsonFormat("list_scripts", QJsonArray()));

}

void MainWindow::showFile(QString path)
{
    contentWidget->showFile(path);
    QString address =  HXChain::getInstance()->configGetContractAddress(path);

    if(address.startsWith("CON") )
    {
        fileListWidget->interfaceWidget->setContract(address);
    }
    else
    {
        // 如果是脚本
        address = HXChain::getInstance()->configGetScriptId(path);
        if( address.startsWith("SCR"))
        {
            fileListWidget->interfaceWidget->setScript(address);
        }
        else
        {
            fileListWidget->interfaceWidget->setContract("");
        }
    }

    checkActionEnable(path);
}

void MainWindow::showContract(QString address)
{
    fileListWidget->interfaceWidget->setContract(address);
    contentWidget->showContract(address);

    contentWidgetTextChanged();

    ui->exportAction->setEnabled(true);
    ui->importAction->setEnabled(true);
    ui->addScriptAction->setEnabled(false);
    ui->removeScriptAction->setEnabled(false);
    ui->bindAction->setEnabled(false);
    ui->compileAction->setEnabled(false);
    ui->registerAction->setEnabled(false);
    ui->upgradeAction->setEnabled(false);
    ui->withdrawAction->setEnabled(false);
    ui->closeAction->setEnabled(true);

    ui->saveAction->setEnabled( contentWidget->currentFileUnsaved());
    ui->saveAllAction->setEnabled( contentWidget->hasFileUnsaved());
}

void MainWindow::on_newContractAction_triggered()
{
    // 新建合约文件
    // 新建的文件命名为 "new X.glua"  序号依次排
    int num = 1;
    QDir dir("contracts");
    do
    {
        QString newFileName = "new " + QString::number(num) + ".glua";
        if(!HXChain::getInstance()->fileRecordMap.contains(dir.absoluteFilePath(newFileName)))
        {
            FileRecord frcd;
            frcd.type = FileRecord::CONTRACT_FILE;
            frcd.path = dir.absoluteFilePath(newFileName);
            frcd.newBuilt = true;
            qDebug() << "frcd.path" << frcd.path;
            HXChain::getInstance()->fileRecordMap.insert(dir.absoluteFilePath(newFileName), frcd);
            qDebug() << "dir.absoluteFilePath" << dir.absoluteFilePath(newFileName);
            contentWidget->showFile(dir.absoluteFilePath(newFileName));
            fileListWidget->addFile(0,0,newFileName);
            fileListWidget->setCurrentIndex(0);
            break;
        }
        num++;
    }while(1);
}

void MainWindow::on_newScriptAction_triggered()
{
    // 新建脚本文件
    // 新建的文件命名为 "new_X.glua"  序号依次排
    int num = 1;
    QString newFileName = "new_" + QString::number(num) + ".glua";

    do
    {
        QDir dir( "scripts/" + newFileName);
        if( !HXChain::getInstance()->fileRecordMap.contains(dir.absolutePath()) )  break;
        num++;
        newFileName = "new_" + QString::number(num) + ".glua";
    }while(1);

    // 新建的合约在保存时候才实际创建
    QDir dir2("scripts");
    FileRecord frcd;
    frcd.type = FileRecord::SCRIPT_FILE;
    frcd.path = dir2.absoluteFilePath(newFileName);
    frcd.newBuilt = true;
    HXChain::getInstance()->fileRecordMap.insert(dir2.absoluteFilePath(newFileName), frcd);

    contentWidget->showFile(dir2.absoluteFilePath(newFileName));
    fileListWidget->addFile(1,0,newFileName);
    fileListWidget->setCurrentIndex(1);
}

void MainWindow::on_saveAction_triggered()
{
    contentWidget->saveFile();
    ui->saveAction->setEnabled(false);
}

//void MainWindow::on_accountAction_triggered()
//{
//    AccountDialog accountDialog;
//    connect( &accountDialog,SIGNAL(currentAccountChanged(QString)),this,SLOT(currentAccountChanged(QString)));
//    accountDialog.exec();
//}

void MainWindow::on_accountListAction_triggered()
{
    AccountListDialog accountListDialog;
    connect( &accountListDialog, SIGNAL(newAccount()), this, SLOT(getAccountInfo()));
    connect( &accountListDialog,SIGNAL(currentAccountChanged(QString)),this,SLOT(currentAccountChanged(QString)));
    connect( this, SIGNAL(accountInfoUpdated()), &accountListDialog, SLOT(updateAccountList()));
    accountListDialog.exec();
}

void MainWindow::on_consoleAction_triggered()
{
    ConsoleDialog consoleDialog;
    connect( outputWidget, SIGNAL(outputRead(QString)), &consoleDialog, SLOT(outputRead(QString)));
    consoleDialog.exec();
}

void MainWindow::on_compileAction_triggered()
{
    contentWidget->saveFile();

    if( HXChain::getInstance()->fileRecordMap.value(contentWidget->currentFilePath).type == FileRecord::CONTRACT_FILE )
    {
//        if(contentWidget->currentFilePath.endsWith(".glua"))
//        {
//            HXChain::getInstance()->postRPC( "id_compile_contract_" + contentWidget->currentFilePath, toJsonFormat("compile_contract", QJsonArray() << contentWidget->currentFilePath));
//        }
        HXChain::getInstance()->compileManager->startCompile(contentWidget->currentFilePath);
    }
    else if( HXChain::getInstance()->fileRecordMap.value(contentWidget->currentFilePath).type == FileRecord::SCRIPT_FILE )
    {
        if(contentWidget->currentFilePath.endsWith(".glua"))
        {
            HXChain::getInstance()->postRPC(  "id_compile_script_" + contentWidget->currentFilePath, toJsonFormat("compile_script", QJsonArray() << contentWidget->currentFilePath));
        }

    }



}

void MainWindow::on_registerAction_triggered()
{
    if( contentWidget->currentFilePath.isEmpty() || !contentWidget->currentFilePath.endsWith(".gpc"))  return;

    RegisterContractDialog registerContractDialog(contentWidget->currentFilePath);
    connect( &registerContractDialog, SIGNAL(contractRegistered(QString)), this, SLOT(contractRegistered(QString)));
    registerContractDialog.exec();
}

void MainWindow::on_upgradeAction_triggered()
{
    QString address;

    if( contentWidget->currentFilePath.isEmpty())   return;
    if( contentWidget->currentFilePath.startsWith("CON") && !contentWidget->currentFilePath.contains("."))
    {
        // 如果当前选择的是链上合约
        address = contentWidget->currentFilePath;
    }
    else
    {
        address = HXChain::getInstance()->configGetContractAddress(contentWidget->currentFilePath);
    }

    UpgradeDialog upgradeDialog( address);
    upgradeDialog.exec();

    autoRefresh();
}

void MainWindow::on_transferAction_triggered()
{
    QString address;
    if( contentWidget->currentFilePath.startsWith("CON") && !contentWidget->currentFilePath.contains("."))
    {
        // 如果当前选择的是链上合约
        address = contentWidget->currentFilePath;
    }
    else
    {
        address = HXChain::getInstance()->configGetContractAddress(contentWidget->currentFilePath);
    }

    TransferDialog transferDialog( address);
    connect(this, SIGNAL(accountInfoUpdated()), &transferDialog, SLOT(updateAccountBalance()));
    connect(this, SIGNAL(contractInfoUpdated(QString)), &transferDialog, SLOT(contractInfoUpdated(QString)));
    transferDialog.exec();
}

void MainWindow::on_undoAction_triggered()
{
    contentWidget->undo();
}

void MainWindow::on_redoAction_triggered()
{
    contentWidget->redo();
}

void MainWindow::on_callAction_triggered()
{
    QString address;
    if( contentWidget->currentFilePath.startsWith("CON") && !contentWidget->currentFilePath.contains("."))
    {
        // 如果当前选择的是链上合约
        address = contentWidget->currentFilePath;
    }
    else
    {
        address = HXChain::getInstance()->configGetContractAddress(contentWidget->currentFilePath);
    }

    CallDialog callDialog(address);
    connect(this, SIGNAL(contractInfoUpdated(QString)), &callDialog, SLOT(contractInfoUpdated(QString)));
    callDialog.exec();
}

void MainWindow::on_withdrawAction_triggered()
{
    QString address;
    if( contentWidget->currentFilePath.startsWith("CON") && !contentWidget->currentFilePath.contains("."))
    {
        // 如果当前选择的是链上合约
        address = contentWidget->currentFilePath;
    }
    else
    {
        address = HXChain::getInstance()->configGetContractAddress(contentWidget->currentFilePath);
    }

    // 只有未升级的合约可以销毁
    if( HXChain::getInstance()->contractInfoMap.value(address).level != "temp")  return;

    WithdrawDialog withdrawDialog(address);
    connect( &withdrawDialog, SIGNAL(contractDestroyed(QString)), this, SLOT(contractDestroyed(QString)));
    withdrawDialog.exec();

}

void MainWindow::contractDestroyed(QString address)
{
    HXChain::getInstance()->contractInfoMap.remove(address);

    // contentWidget关闭已经打开的
    if( contentWidget->pathAceEditorMap.contains(contentWidget->currentFilePath))
    {
        contentWidget->closeFile(contentWidget->currentFilePath);
    }

    // 删除config文件中的记录
    QString key = HXChain::getInstance()->configRemoveContractAddress( address);

    if( key.isEmpty())
    {
        qDebug() << "no contract in config file: " << address;

        fileListWidget->getRegisteredContracts();
        getAccountInfo();
        return;
    }

    QString path = HXChain::getInstance()->restorePathFormat(key);
    qDebug() << "remove file " + path + " : " << QFile::remove(path);
    fileListWidget->getRegisteredContracts();

    getAccountInfo();
}

void MainWindow::scriptRemoved(QString scriptId)
{
    // contentWidget关闭已经打开的
    if( contentWidget->pathAceEditorMap.contains(contentWidget->currentFilePath))
    {
        contentWidget->closeFile(contentWidget->currentFilePath);
    }

    fileListWidget->getAddedScripts();
}

void MainWindow::formalWalletLogin()
{
    HXChain::getInstance()->setCurrentChain(2);
    checkChangeChainActionEnable(2);

    connect( HXChain::getInstance()->formalProcess, SIGNAL(readyReadStandardOutput()), outputWidget, SLOT(formalHasOutputToRead()));


    setWindowTitle( QString::fromLocal8Bit("正式链-IDE"));

    HXChain::getInstance()->currentAccount = HXChain::getInstance()->configFile->value("/settings/formalCurrentAccount").toString();
    HXChain::getInstance()->myContracts.clear();
    HXChain::getInstance()->scriptInfoMap.clear();

    QTimer::singleShot(200,fileListWidget,SLOT(getRegisteredContracts()));  // 第一次获取前等一段时间
    fileListWidget->getForeverContracts();
    fileListWidget->getAddedScripts();
    fileListWidget->interfaceWidget->setContract("");
    outputWidget->changeToFormal();
    //    contentWidget->hideAllEditor();
    contentWidget->onChainChanged();

    getAccountInfo();
    getAllAccountsContract();
    getAllScripts();

    formalWalletOpened = true;
}

void MainWindow::contentWidgetTextChanged()
{
    bool undoAvailable = contentWidget->isUndoAvailable();
    bool redoAvailable = contentWidget->isRedoAvailable();
    ui->undoAction->setEnabled(undoAvailable);
    ui->redoAction->setEnabled(redoAvailable);

    bool currentFileUnsaved = contentWidget->currentFileUnsaved();
    bool hasFileUnsaved = contentWidget->hasFileUnsaved();
    ui->saveAction->setEnabled(currentFileUnsaved);
    ui->saveAllAction->setEnabled(hasFileUnsaved);

}

void MainWindow::on_addScriptAction_triggered()
{
    if( contentWidget->currentFilePath.isEmpty() || !contentWidget->currentFilePath.endsWith(".script"))  return;

    AddScriptDialog addScriptDialog(contentWidget->currentFilePath);
    connect( &addScriptDialog, SIGNAL(scriptAdded(QString)), this, SLOT(scriptAdded(QString)));
    addScriptDialog.exec();

}

void MainWindow::on_removeScriptAction_triggered()
{
    QString scriptId = HXChain::getInstance()->configGetScriptId(contentWidget->currentFilePath);

    if( scriptId.isEmpty())  return;

    RemoveScriptDialog removeScriptDialog(scriptId);
    connect( &removeScriptDialog, SIGNAL(scriptRemoved(QString)), this, SLOT(scriptRemoved(QString)));
    removeScriptDialog.exec();
}

void MainWindow::on_bindAction_triggered()
{
    QString scriptId = HXChain::getInstance()->configGetScriptId(contentWidget->currentFilePath);
    if( scriptId.isEmpty())  return;

    BindDialog bindDialog(scriptId,"","");
    bindDialog.exec();

}

void MainWindow::on_enterSandboxAction_triggered()
{   
    if( !HXChain::getInstance()->isInSandBox)
    {
        HXChain::getInstance()->postRPC( "id_sandbox_open", toJsonFormat("sandbox_open", QJsonArray() << ""));
    }
}

void MainWindow::on_exitSandboxAction_triggered()
{
    if( HXChain::getInstance()->isInSandBox)
    {
        HXChain::getInstance()->postRPC( "id_sandbox_close", toJsonFormat("sandbox_close", QJsonArray() << ""));
    }
}

void MainWindow::on_importAction_triggered()
{
    QString filePath = QFileDialog::getOpenFileName(this, QString::fromLocal8Bit("选择合约字节码文件"), "", "(*.gpc)");

    if( !filePath.isEmpty())
    {
        QFileInfo info(filePath);
        QDir dir("contracts");
        if( info.absolutePath() == dir.absolutePath())
        {
            QMessageBox::warning(NULL, "", QString::fromLocal8Bit("合约已在本地!"), QMessageBox::Ok);
            return;
        }

        if( QFile::exists("contracts/" + info.fileName()) )
        {
            // 如果contracts 下存在同名文件
            QMessageBox::warning(NULL, "", info.fileName() + QString::fromLocal8Bit("文件已存在!"), QMessageBox::Ok) ;
            return;

        }

        if( QFile::copy(filePath,"contracts/" + info.fileName()) )
        {
            fileListWidget->addFile(0,0,info.fileName());
            fileListWidget->treeWidgetSetCurrentItem(0,0,info.fileName());
            QMessageBox::information(NULL, "", QString::fromLocal8Bit("导入成功!"), QMessageBox::Ok);

        }
        else
        {
            QMessageBox::warning(NULL, "", QString::fromLocal8Bit("导入失败!"), QMessageBox::Ok);
        }

    }
}

void MainWindow::on_exportAction_triggered()
{
    if( contentWidget->currentFilePath.startsWith("CON") && !contentWidget->currentFilePath.contains("."))
    {
        // 链上合约可以导出
        QString filePath = QFileDialog::getSaveFileName(this, QString::fromLocal8Bit("导出合约"), "contracts", "(*.gpc)");

        if( !filePath.isEmpty() )
        {
            if( !filePath.endsWith(".gpc"))    filePath.append(".gpc");
            HXChain::getInstance()->postRPC( "id_load_contract_to_file_" + filePath, toJsonFormat("load_contract_to_file",
                                                         QJsonArray() << contentWidget->currentFilePath << filePath));
        }
    }

}

void MainWindow::on_changeToFormalChainAction_triggered()
{
    if( !formalExeLaunched)
    {
        QMessageBox::information(NULL, "", QString::fromLocal8Bit("正式链还没有准备好，请稍等..."), QMessageBox::Ok);
        return;
    }

    if( HXChain::getInstance()->currenChain() == 1)
    {
        if( QMessageBox::Yes == QMessageBox::information(NULL, "", QString::fromLocal8Bit("是否切换至正式链?"), QMessageBox::Yes | QMessageBox::No))
        {
            // 切换链前退出沙盒
            HXChain::getInstance()->postRPC( "id_sandbox_close", toJsonFormat("sandbox_close", QJsonArray() << ""));
            HXChain::getInstance()->isInSandBox = false;

            if( !formalWalletOpened)
            {
                HXChain::getInstance()->postRPC(  "id_open_changetoformal", toJsonFormat("open", QJsonArray() << "wallet"), 2);
            }
            else
            {
                HXChain::getInstance()->setCurrentChain(2);
                checkChangeChainActionEnable(2);

                setWindowTitle( QString::fromLocal8Bit("正式链-IDE"));

                HXChain::getInstance()->currentAccount = HXChain::getInstance()->configFile->value("/settings/formalCurrentAccount").toString();
                HXChain::getInstance()->myContracts.clear();
                HXChain::getInstance()->scriptInfoMap.clear();

                QTimer::singleShot(200,fileListWidget,SLOT(getRegisteredContracts()));  // 第一次获取前等一段时间
                fileListWidget->getForeverContracts();
                fileListWidget->getAddedScripts();
                fileListWidget->interfaceWidget->setContract("");
                outputWidget->changeToFormal();
                //               contentWidget->hideAllEditor();
                contentWidget->onChainChanged();

                getAccountInfo();
                getAllAccountsContract();
                getAllScripts();
            }
        }
    }

}

void MainWindow::on_changeToTestChainAction_triggered()
{
    if( HXChain::getInstance()->currenChain() == 2)
    {
        if( QMessageBox::Yes ==  QMessageBox::information(NULL, "", QString::fromLocal8Bit("是否切换至测试链?"), QMessageBox::Yes | QMessageBox::No))
        {
            HXChain::getInstance()->postRPC( "id_sandbox_close", toJsonFormat("sandbox_close", QJsonArray() << ""));
            HXChain::getInstance()->isInSandBox = false;

            HXChain::getInstance()->setCurrentChain(1);
            checkChangeChainActionEnable(1);

            setWindowTitle( QString::fromLocal8Bit("测试链-IDE"));

            HXChain::getInstance()->currentAccount = HXChain::getInstance()->configFile->value("/settings/testCurrentAccount").toString();
            HXChain::getInstance()->myContracts.clear();
            HXChain::getInstance()->scriptInfoMap.clear();


            QTimer::singleShot(200,fileListWidget,SLOT(getRegisteredContracts()));  // 第一次获取前等一段时间
            fileListWidget->getForeverContracts();
            fileListWidget->getAddedScripts();
            fileListWidget->interfaceWidget->setContract("");
            outputWidget->changeToTest();
            //           contentWidget->hideAllEditor();
            contentWidget->onChainChanged();

            getAccountInfo();
            getAllAccountsContract();
            getAllScripts();
        }

    }
}

void MainWindow::on_transferToAccountAction_triggered()
{
    TransferToAccountDialog transferToAccountDialog;
    connect(this, SIGNAL(accountInfoUpdated()), &transferToAccountDialog, SLOT(updateAccountBalance()));
    transferToAccountDialog.exec();
}

void MainWindow::startTimerForAutoRefresh()
{
    timerForAutoRefresh = new QTimer(this);
    connect(timerForAutoRefresh,SIGNAL(timeout()),this,SLOT(autoRefresh()));
    timerForAutoRefresh->start(AUTO_REFRESH_TIME);
}

void MainWindow::autoRefresh()
{
    getAccountInfo();
    specialOperationsWhenAutoRefresh();

    fileListWidget->getForeverContracts();
}

void MainWindow::currentAccountChanged(QString account)
{
    fileListWidget->getRegisteredContracts();

    contentWidget->onAccountChanged();
}

void MainWindow::readTestExe()
{
//    QString str = HXChain::getInstance()->readAll(HXChain::getInstance()->testProcess);

//    if( str.contains(">>>"))
//    {
//        timerForTestLaunch->stop();

//        RpcThread* rpcThread = new RpcThread;
//        connect(rpcThread,SIGNAL(finished()),rpcThread,SLOT(deleteLater()));
//        rpcThread->setLogin("a","b");
//        rpcThread->setWriteData(  "id_open", toJsonFormat("open", QJsonArray() << "wallet" ));
//        rpcThread->start();
//        HXChain::getInstance()->initWorkerThreadManagerForTestChain();
//    }
}

void MainWindow::readFormalExe()
{
    QString str = HXChain::getInstance()->readAll(HXChain::getInstance()->formalProcess);

    if( str.contains(">>>"))
    {
        timerForFormalLaunch->stop();
        formalExeLaunched = true;
        HXChain::getInstance()->initWorkerThreadManagerForFormalChain();
    }
}

void MainWindow::on_saveAllAction_triggered()
{
    contentWidget->saveAll();
}

void MainWindow::on_closeAction_triggered()
{
    if( !contentWidget->currentFilePath.isEmpty())
    {
        contentWidget->closeFile(contentWidget->currentFilePath);
    }
}

void MainWindow::on_exitAction_triggered()
{
    if(contentWidget->closeAll())
    {
        close();
    }

}

void MainWindow::on_helpAction_triggered()
{
    QDesktopServices::openUrl(QUrl("http://115.28.142.164:9000/index.html"));
}

void MainWindow::CompileFinishSlot(const QString &dstFilePath)
{
    QFileInfo info(dstFilePath);

    fileListWidget->getContractsList();

    if( contentWidget->pathAceEditorMap.contains(info.absoluteFilePath()))
    {
        contentWidget->closeFile(info.absoluteFilePath());
    }
    this->showFile(info.absoluteFilePath());
    fileListWidget->treeWidgetSetCurrentItem(0,0, info.fileName());
}

void MainWindow::SetIDETheme(DataDefine::ThemeStyle theme)
{
    setAutoFillBackground(true);
    QPalette palette;

    QString path ;
    if(DataDefine::Black_Theme == theme)
    {
        path = ":/qss/black_style.qss";
        palette.setColor(QPalette::Window, QColor(30,30,30));
    }
    else if(DataDefine::White_Theme == theme)
    {
        path = ":/qss/white_style.qss";
        palette.setColor(QPalette::Window, QColor(255,255,255));
    }


    setPalette(palette);

    QFile inputFile(path);
    inputFile.open(QIODevice::ReadOnly);
    QString css = inputFile.readAll();
    inputFile.close();
    setStyleSheet( css);

}

