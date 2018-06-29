#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <QSettings>
#include <QSplitter>
#include <QCoreApplication>
#include <QMessageBox>

#include "ChainIDE.h"
#include "DataManager.h"
#include "selectpathwidget.h"
#include "waitingforsync.h"
#include "outputwidget.h"
#include "compile/CompileManager.h"
#include "backstage/BackStageBase.h"
#include "filewidget/FileWidget.h"
#include "contentwidget/ContextWidget.h"


#include "popwidget/NewFileDialog.h"
#include "popwidget/consoledialog.h"

#include "popwidget/AccountWidget.h"
#include "popwidget/registercontractdialog.h"
#include "popwidget/TransferWidget.h"
#include "popwidget/CallContractWidget.h"


class MainWindow::DataPrivate
{
public:
    DataPrivate()
        :waitingForSync(nullptr)
    {

    }
public:
    WaitingForSync* waitingForSync;
};

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    _p(new DataPrivate())
{
    ui->setupUi(this);
    InitWidget();
}

MainWindow::~MainWindow()
{
    delete _p;
    delete ChainIDE::getInstance();
    delete ui;
}

void MainWindow::InitWidget()
{
//    startWidget();
    hide();
    if( ChainIDE::getInstance()->getConfigAppDataPath().isEmpty() )
    {
        showSelectPathWidget();
    }
    else
    {
        startChain();
    }
}

void MainWindow::showSelectPathWidget()
{
    SelectPathWidget* selectPathWidget = new SelectPathWidget;
    selectPathWidget->setWindowTitle( QString::fromLocal8Bit("IDE"));
    selectPathWidget->setAttribute(Qt::WA_DeleteOnClose);
    connect( selectPathWidget,&SelectPathWidget::enter,this,&MainWindow::startChain);

    selectPathWidget->show();
}

void MainWindow::startChain()
{
    showWaitingForSyncWidget();

    //启动client ， node
    connect(ChainIDE::getInstance()->testManager(),&BackStageBase::exeStarted,this,&MainWindow::exeStartedSlots);
    connect(ChainIDE::getInstance()->formalManager(),&BackStageBase::exeStarted,this,&MainWindow::exeStartedSlots);
    ChainIDE::getInstance()->testManager()->startExe();
    //ChainIDE::getInstance()->formalManager()->startExe();

}

void MainWindow::startWidget()
{
    showMaximized();
    //标题
    //set
    //设置界面背景色
    setAutoFillBackground(true);
    QPalette palette;
    palette.setColor(QPalette::Window, DataDefine::Black_Theme == ChainIDE::getInstance()->getCurrentTheme() ?
                                       DataDefine::DARK_CLACK_BACKGROUND : DataDefine::WHITE_BACKGROUND);
    setPalette(palette);

    //初始化样式表
    ChainIDE::getInstance()->refreshStyleSheet();

    //隐藏部分不需要的按钮
    ui->savaAsAction->setVisible(false);
    ui->enterSandboxAction->setVisible(false);
    ui->exitSandboxAction->setVisible(false);
    ui->withdrawAction->setVisible(false);
    ui->transferAction->setVisible(false);
    ui->importAction->setVisible(false);
    ui->exportAction->setVisible(false);

    //设置界面比例
    ui->splitter_hori->setSizes(QList<int>()<<0.1*ui->centralWidget->width()<<0.9*ui->centralWidget->width());
    ui->splitter_ver->setSizes(QList<int>()<<0.67*ui->centralWidget->height()<<0.33*ui->centralWidget->height());

    //链接编译槽
    connect(ChainIDE::getInstance()->getCompileManager(),&CompileManager::CompileOutput,std::bind(&OutputWidget::receiveCompileMessage,ui->outputWidget,std::placeholders::_1,ChainIDE::getInstance()->getCurrentChainType()));
    connect(ui->fileWidget,&FileWidget::fileClicked,ui->contentWidget,&ContextWidget::showFile);

    connect(ui->contentWidget,&ContextWidget::fileSelected,ui->fileWidget,&FileWidget::SelectFile);
    connect(ui->contentWidget,&ContextWidget::contentStateChange,this,&MainWindow::ModifyActionState);

    ModifyActionState();

    //已注册合约
    //connect(ChainIDE::getInstance()->getDataManager(),&DataManager::queryContractFinish,ui->contractWidget,&ContractWidget::RefreshTree);
    connect(ui->tabWidget,&QTabWidget::currentChanged,this,&MainWindow::on_tabWidget_currentChanged);
    //QTimer::singleShot(3000,[](){ChainIDE::getInstance()->getDataManager()->queryContract();});

}

void MainWindow::closeEvent(QCloseEvent *event)
{
    ChainIDE::getInstance()->testManager()->ReadyClose();
    ChainIDE::getInstance()->formalManager()->ReadyClose();
    QWidget::closeEvent(event);
}

void MainWindow::exeStartedSlots()
{
    if(ChainIDE::getInstance()->testManager()->exeRunning() /*&& ChainIDE::getInstance()->formalManager()->exeRunning()*/)
    {
       disconnect(ChainIDE::getInstance()->testManager(),&BackStageBase::exeStarted,this,&MainWindow::exeStartedSlots);
       disconnect(ChainIDE::getInstance()->formalManager(),&BackStageBase::exeStarted,this,&MainWindow::exeStartedSlots);

       ChainIDE::getInstance()->getDataManager()->InitManager();

       if(_p->waitingForSync)
       {
           _p->waitingForSync->close();
       }
       startWidget();
    }
}

void MainWindow::showWaitingForSyncWidget()
{
    _p->waitingForSync = new WaitingForSync;
    _p->waitingForSync->setWindowTitle( QString::fromLocal8Bit("IDE"));
    _p->waitingForSync->setAttribute(Qt::WA_DeleteOnClose);

    _p->waitingForSync->show();
}

void MainWindow::on_newContractAction_glua_triggered()
{//新建glua合约，只能放在glua文件夹下
    NewFileDialog dia(QCoreApplication::applicationDirPath()+"/"+DataDefine::GLUA_DIR,QStringList()<<"."+DataDefine::GLUA_SUFFIX);
    dia.exec();
    NewFileCreated(dia.getNewFilePath());
}

void MainWindow::on_newContractAction_csharp_triggered()
{//新建csharp合约，放在csharp目录下
    NewFileDialog dia(QCoreApplication::applicationDirPath()+"/"+DataDefine::CSHARP_DIR,QStringList()<<"."+DataDefine::CSHARP_SUFFIX);
    dia.exec();
    NewFileCreated(dia.getNewFilePath());
}

void MainWindow::on_newContractAtion_kotlin_triggered()
{//新建kotlin合约，只能放在kotlin文件夹下
    NewFileDialog dia(QCoreApplication::applicationDirPath()+"/"+DataDefine::KOTLIN_DIR,QStringList()<<"."+DataDefine::KOTLIN_SUFFIX);
    dia.exec();
    NewFileCreated(dia.getNewFilePath());

}

void MainWindow::on_newContractAction_java_triggered()
{//新建java合约，放在java目录下
    NewFileDialog dia(QCoreApplication::applicationDirPath()+"/"+DataDefine::JAVA_DIR,QStringList()<<"."+DataDefine::JAVA_SUFFIX);
    dia.exec();
    NewFileCreated(dia.getNewFilePath());
}

void MainWindow::on_saveAction_triggered()
{
    ui->contentWidget->saveFile();
}

void MainWindow::on_savaAsAction_triggered()
{

}

void MainWindow::on_saveAllAction_triggered()
{
    ui->contentWidget->saveAll();
}

void MainWindow::on_closeAction_triggered()
{
    ui->contentWidget->closeFile(ui->contentWidget->getCurrentFilePath());
}

void MainWindow::on_closeAllAction_triggered()
{
    ui->contentWidget->closeAll();
}

void MainWindow::on_tabWidget_currentChanged(int index)
{
    qDebug()<<"sdfsf";
    if(index == 1)
    {
        ui->contractWidget->RefreshTree();
        //ChainIDE::getInstance()->getDataManager()->queryContract();
    }
}

void MainWindow::on_exitAction_triggered()
{
    if(ui->contentWidget->closeAll())
    {
        close();
    }
}

void MainWindow::on_undoAction_triggered()
{
    ui->contentWidget->undo();
}

void MainWindow::on_redoAction_triggered()
{
    ui->contentWidget->redo();
}

void MainWindow::on_importAction_triggered()
{

}

void MainWindow::on_exportAction_triggered()
{

}

void MainWindow::on_registerAction_triggered()
{
    RegisterContractDialog dia;
    dia.exec();
}

void MainWindow::on_transferAction_triggered()
{

}

void MainWindow::on_callAction_triggered()
{//调用合约
    CallContractWidget callWidget;
    callWidget.exec();
}

void MainWindow::on_upgradeAction_triggered()
{

}

void MainWindow::on_withdrawAction_triggered()
{

}

void MainWindow::on_changeChainAction_triggered()
{
    ChainIDE::getInstance()->setCurrentChainType(ChainIDE::getInstance()->getCurrentChainType() == 1?2:1);
    ModifyActionState();
}

void MainWindow::on_compileAction_triggered()
{//编译
    //先触发保存判断
    if( ui->contentWidget->currentFileUnsaved())
    {
        QMessageBox::StandardButton choice = QMessageBox::information(NULL, "", ui->contentWidget->getCurrentFilePath() + " " + tr("文件已修改，是否保存?"), QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
        if( QMessageBox::Yes == choice)
        {
            ui->contentWidget->saveFile();
        }
        else
        {
            return;
        }
    }


    ChainIDE::getInstance()->getCompileManager()->startCompile(ui->contentWidget->getCurrentFilePath());//当前打开的文件

}

void MainWindow::on_debugAction_triggered()
{

}

void MainWindow::on_stopAction_triggered()
{

}

void MainWindow::on_stepAction_triggered()
{

}

void MainWindow::on_enterSandboxAction_triggered()
{
    ChainIDE::getInstance()->setSandboxMode(true);
    ModifyActionState();
}

void MainWindow::on_exitSandboxAction_triggered()
{
    ChainIDE::getInstance()->setSandboxMode(false);
    ModifyActionState();
}

void MainWindow::on_accountListAction_triggered()
{
    AccountWidget accountWidget;
    accountWidget.exec();
}

void MainWindow::on_consoleAction_triggered()
{
    ConsoleDialog consoleDialog;
    consoleDialog.exec();
}

void MainWindow::on_transferToAccountAction_triggered()
{//转账
    TransferWidget transferWidget;
    transferWidget.exec();
}

void MainWindow::on_helpAction_triggered()
{

}

void MainWindow::on_aboutAction_triggered()
{

}

void MainWindow::ModifyActionState()
{
    ui->changeChainAction->setIcon(ChainIDE::getInstance()->getCurrentChainType()==1?
                                   QIcon(":/pic/changeToFormal_enable.png"):QIcon(":/pic/changeToTest_enable.png"));
    ui->changeChainAction->setText(ChainIDE::getInstance()->getCurrentChainType()==1?tr("切换到正式链"):tr("切换到测试链"));

    ui->undoAction->setEnabled(ui->contentWidget->isUndoAvailable());
    ui->redoAction->setEnabled(ui->contentWidget->isRedoAvailable());
    ui->saveAction->setEnabled(ui->contentWidget->currentFileUnsaved());
    ui->saveAllAction->setEnabled(ui->contentWidget->hasFileUnsaved());

    ui->enterSandboxAction->setEnabled(!ChainIDE::getInstance()->isSandBoxMode());
    ui->exitSandboxAction->setEnabled(ChainIDE::getInstance()->isSandBoxMode());

    if(ui->fileWidget->getCurrentFile().endsWith(DataDefine::GLUA_SUFFIX)||
       ui->fileWidget->getCurrentFile().endsWith(DataDefine::JAVA_SUFFIX)||
       ui->fileWidget->getCurrentFile().endsWith(DataDefine::CSHARP_SUFFIX)||
       ui->fileWidget->getCurrentFile().endsWith(DataDefine::KOTLIN_SUFFIX))
    {
        ui->compileAction->setEnabled(true);
    }
    else
    {
        ui->compileAction->setEnabled(false);
    }

    ui->savaAsAction->setEnabled(ui->fileWidget->getCurrentFile().isEmpty());
}

void MainWindow::NewFileCreated(const QString &filePath)
{//新建了文件后，文件树刷新，点击文件树，
    if(filePath.isEmpty() || !QFileInfo(filePath).isFile()) return;
    ui->fileWidget->SelectFile(filePath);
    ui->contentWidget->showFile(filePath);
}


