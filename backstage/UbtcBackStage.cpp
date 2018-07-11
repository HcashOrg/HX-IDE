#include "UbtcBackStage.h"

#include <QTimer>
#include <QDebug>
#include <QSettings>
#include <QCoreApplication>
#include <QDir>
#include <thread>
#include <memory>

#include "DataDefine.h"
#include "ChainIDE.h"
#include "popwidget/commondialog.h"


#include "datarequire/DataRequireManager.h"

static const int NODE_RPC_PORT = 5555;//node端口  test    formal = test+10
static const int CLIENT_RPC_PORT = 5556;//client端口  test    formal = test+10

static const QString RPC_USER = "a";
static const QString RPC_PASSWORD = "b";

class UbtcBackStage::DataPrivate
{
public:
    DataPrivate(int type)
        :nodeProc(new QProcess)
        ,clientProc(new QProcess)
        ,chaintype(type)
    {
        nodePort = NODE_RPC_PORT + 10*(type-1);
        clientPort = CLIENT_RPC_PORT + 10*(type-1);
        dataPath = 1 == type ? "/testub" : "/formalub";//teUbctPastth
        dataRequire = new DataRequireManager("127.0.0.1",QString::number(clientPort));

    }
    ~DataPrivate()
    {
        delete clientProc;
        clientProc = nullptr;
        delete nodeProc;
        nodeProc = nullptr;
        delete dataRequire;
        dataRequire = nullptr;
    }
public:
    int chaintype;
    int nodePort;
    int clientPort;
    QString dataPath;
    QProcess* nodeProc;
    QProcess* clientProc;
    QTimer    timerForStartExe;
    QString currentCMD;
    DataRequireManager *dataRequire;
};

UbtcBackStage::UbtcBackStage(int type,QObject *parent)
    : BackStageBase(parent)
    ,_p(new DataPrivate(type))
{
}

UbtcBackStage::~UbtcBackStage()
{
    delete _p;
}

void UbtcBackStage::startExe()
{
    connect(_p->nodeProc,&QProcess::stateChanged,this,&UbtcBackStage::onNodeExeStateChanged);

    //先确保目录存在
    QString dataPath = ChainIDE::getInstance()->getConfigAppDataPath().replace("\\","/")+_p->dataPath;
    QDir dir(dataPath);
    if(!dir.exists())
    {
        dir.mkpath(dir.path());
    }

    QStringList strList;
    strList << "-datadir=" + dataPath
            << "-testnet"<<"-staking"<<"-port="+QString::number(_p->nodePort)<<"-rpcport="+QString::number(_p->clientPort)
            << "-rpcuser="+RPC_USER<<"-rpcpassword="+RPC_PASSWORD<<"-txindex"<<"-server";
    qDebug() << "start ubcd.exe " << strList;
    _p->nodeProc->start(QCoreApplication::applicationDirPath()+QDir::separator()+DataDefine::UBCD_NODE_EXE,strList);
}

bool UbtcBackStage::exeRunning()
{
    return _p->nodeProc->state() == QProcess::Running;
}

QProcess *UbtcBackStage::getProcess() const
{
    return _p->clientProc;
}

void UbtcBackStage::ReadyClose()
{
    disconnect(_p->nodeProc,&QProcess::stateChanged,this,&UbtcBackStage::onNodeExeStateChanged);
    QStringList strList;
    strList << "-rpcport="+QString::number(_p->clientPort)
            << "-rpcuser="+RPC_USER<<"-rpcpassword="+RPC_PASSWORD<<"stop";
    _p->clientProc->execute(QCoreApplication::applicationDirPath()+QDir::separator()+DataDefine::UBCD_CLIENT_EXE,strList);

    _p->clientProc->waitForFinished(30000);
    qDebug()<<"stop";
    _p->clientProc->close();
    _p->nodeProc->close();
}

void UbtcBackStage::rpcPostedSlot(const QString &cmd, const QString & param)
{
    _p->dataRequire->requirePosted(cmd,param);
}

void UbtcBackStage::onNodeExeStateChanged()
{
    if(_p->nodeProc->state() == QProcess::Starting)
    {
        qDebug() << QString("%1 is starting").arg("ubcd.exe");
    }
    else if(_p->nodeProc->state() == QProcess::Running)
    {
        qDebug() << QString("%1 is running").arg("ubcd.exe");

        initSocketManager();
        //emit exeStarted();
    }
    else if(_p->nodeProc->state() == QProcess::NotRunning)
    {
        qDebug()<<_p->nodeProc->errorString();
        CommonDialog commonDialog(CommonDialog::OkOnly);
        commonDialog.setText(tr("Fail to launch %1 !").arg("ubcd.exe"));
        commonDialog.pop();
    }
}

void UbtcBackStage::initSocketManager()
{
    connect(_p->dataRequire,&DataRequireManager::requireResponse,this,&BackStageBase::rpcReceived);
    connect(_p->dataRequire,&DataRequireManager::connectFinish,this,&BackStageBase::exeStarted);

    _p->dataRequire->startManager(DataRequireManager::HTTP);
}
