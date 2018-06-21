#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <QSettings>
#include <QSplitter>
#include <QCoreApplication>
#include <QMessageBox>

#include "ChainIDE.h"
#include "selectpathwidget.h"
#include "waitingforsync.h"
#include "ExeManager.h"
#include "filewidget/FileWidget.h"
#include "outputwidget.h"
#include "contentwidget/contentwidget.h"
#include "compile/CompileManager.h"
#include "NewFileDialog.h"
#include "consoledialog.h"

class MainWindow::DataPrivate
{
public:
    DataPrivate()
        :waitingForSync(nullptr)
        ,fileWidget(nullptr)
        ,contentWidget(nullptr)
        ,outputWidget(nullptr)
    {

    }
public:
    WaitingForSync* waitingForSync;

    FileWidget *fileWidget;

    ContentWidget *contentWidget;
    OutputWidget *outputWidget;
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
    delete ui;
}

void MainWindow::InitWidget()
{
    startWidget();
//    hide();
//    if( ChainIDE::getInstance()->getConfigAppDataPath().isEmpty() )
//    {
//        showSelectPathWidget();
//    }
//    else
//    {
//        startChain();
//    }
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
    connect(ChainIDE::getInstance()->testManager(),&ExeManager::exeStarted,this,&MainWindow::exeStartedSlots);
    connect(ChainIDE::getInstance()->formalManager(),&ExeManager::exeStarted,this,&MainWindow::exeStartedSlots);
    ChainIDE::getInstance()->testManager()->startExe();
    //HXChain::getInstance()->formalManager->startExe();

}

void MainWindow::startWidget()
{
    setAutoFillBackground(true);
    QPalette palette;
    palette.setColor(QPalette::Window, DataDefine::Black_Theme == ChainIDE::getInstance()->getCurrentTheme() ?
                                       DataDefine::DARK_CLACK_BACKGROUND : DataDefine::WHITE_BACKGROUND);
    setPalette(palette);

    ChainIDE::getInstance()->refreshStyleSheet();

    //真正地初始化界面
    ui->savaAsAction->setVisible(false);

    //初始化图标
    actionSetIcon();

    QSplitter *horizontalSplitter = new QSplitter(Qt::Horizontal, this);
    this->setCentralWidget( horizontalSplitter );
    _p->fileWidget = new FileWidget( horizontalSplitter);

    QSplitter* verticalSplitter = new QSplitter(Qt::Vertical, horizontalSplitter);
    _p->contentWidget = new ContentWidget( verticalSplitter);
    _p->outputWidget = new OutputWidget( verticalSplitter);

    horizontalSplitter->setStretchFactor(0,1);
    horizontalSplitter->setStretchFactor(1,9);
    verticalSplitter->setStretchFactor(0,2);
    verticalSplitter->setStretchFactor(1,1);

    //链接编译槽
    connect(ChainIDE::getInstance()->getCompileManager(),&CompileManager::CompileOutput,std::bind(&OutputWidget::receiveCompileMessage,_p->outputWidget,std::placeholders::_1,ChainIDE::getInstance()->getCurrentChainType()));
    connect(_p->fileWidget,&FileWidget::fileClicked,_p->contentWidget,&ContentWidget::showFile);
    connect(_p->contentWidget,&ContentWidget::fileSelected,_p->fileWidget,&FileWidget::SelectFile);
    connect(_p->contentWidget,&ContentWidget::contentStateChange,this,&MainWindow::ModifyActionState);


    ModifyActionState();
    showMaximized();
}

void MainWindow::exeStartedSlots()
{
    if(ChainIDE::getInstance()->testManager()->exeRunning() /*&& HXChain::getInstance()->formalManager->exeRunning()*/)
    {
       disconnect(ChainIDE::getInstance()->testManager(),&ExeManager::exeStarted,this,&MainWindow::exeStartedSlots);
       disconnect(ChainIDE::getInstance()->formalManager(),&ExeManager::exeStarted,this,&MainWindow::exeStartedSlots);
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

void MainWindow::actionSetIcon()
{
    ui->newContractAction_glua->setIcon(QIcon(":/pic2/newContractIcon.png"));
    ui->newContractAction_csharp->setIcon(QIcon(":/pic2/newContractIcon.png"));
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
    ui->enterSandboxAction->setIcon(QIcon(":/pic2/enterSandboxIcon.png"));
    ui->changeToFormalChainAction->setIcon(QIcon(":/pic2/changeToFormalChainIcon.png"));
    ui->transferAction->setIcon(QIcon(":/pic2/transferToContractIcon.png"));
    ui->exitSandboxAction->setIcon(QIcon(":/pic2/exitSandboxIcon.png"));
    ui->changeToTestChainAction->setIcon(QIcon(":/pic2/changeToTestChainIcon.png"));

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
    _p->contentWidget->saveFile();
}

void MainWindow::on_savaAsAction_triggered()
{

}

void MainWindow::on_saveAllAction_triggered()
{
    _p->contentWidget->saveAll();
}

void MainWindow::on_compileAction_triggered()
{//编译
    //先触发保存判断
    if( _p->contentWidget->currentFileUnsaved())
    {
        QMessageBox::StandardButton choice = QMessageBox::information(NULL, "", _p->contentWidget->getCurrentFilePath() + " " + tr("文件已修改，是否保存?"), QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
        if( QMessageBox::Yes == choice)
        {
            _p->contentWidget->saveFile();
        }
        else
        {
            return;
        }
    }


    ChainIDE::getInstance()->getCompileManager()->startCompile(_p->contentWidget->getCurrentFilePath());//当前打开的文件

}

void MainWindow::on_closeAction_triggered()
{
    _p->contentWidget->close();
}

void MainWindow::on_closeAllAction_triggered()
{
    _p->contentWidget->closeFile(_p->contentWidget->getCurrentFilePath());
}

void MainWindow::on_exitAction_triggered()
{
    if(_p->contentWidget->closeAll())
    {
        close();
    }
}

void MainWindow::on_undoAction_triggered()
{
    _p->contentWidget->undo();
}

void MainWindow::on_redoAction_triggered()
{
    _p->contentWidget->redo();
}

void MainWindow::on_importAction_triggered()
{

}

void MainWindow::on_exportAction_triggered()
{

}

void MainWindow::on_registerAction_triggered()
{

}

void MainWindow::on_transferAction_triggered()
{

}

void MainWindow::on_callAction_triggered()
{

}

void MainWindow::on_upgradeAction_triggered()
{

}

void MainWindow::on_withdrawAction_triggered()
{

}

void MainWindow::on_changeToFormalChainAction_triggered()
{

}

void MainWindow::on_changeToTestChainAction_triggered()
{

}

void MainWindow::on_enterSandboxAction_triggered()
{

}

void MainWindow::on_exitSandboxAction_triggered()
{

}

void MainWindow::on_accountListAction_triggered()
{

}

void MainWindow::on_consoleAction_triggered()
{
    ConsoleDialog *consoleDialog = new ConsoleDialog();
    consoleDialog->exec();

}

void MainWindow::on_transferToAccountAction_triggered()
{

}

void MainWindow::on_helpAction_triggered()
{

}

void MainWindow::on_aboutAction_triggered()
{

}

void MainWindow::ModifyActionState()
{
    ui->undoAction->setEnabled(_p->contentWidget->isUndoAvailable());
    ui->redoAction->setEnabled(_p->contentWidget->isRedoAvailable());
    ui->saveAction->setEnabled(_p->contentWidget->currentFileUnsaved());
    ui->saveAllAction->setEnabled(_p->contentWidget->hasFileUnsaved());

    //ui->changeToFormalChainAction();
    //ui->changeToTestChainAction();
}

void MainWindow::NewFileCreated(const QString &filePath)
{//新建了文件后，文件树刷新，点击文件树，
    if(filePath.isEmpty() || !QFileInfo(filePath).isFile()) return;
    _p->fileWidget->SelectFile(filePath);
    _p->contentWidget->showFile(filePath);
}


