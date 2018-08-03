#include "UbtcBackStage.h"

#include <QTimer>
#include <QDebug>
#include <QSettings>
#include <QCoreApplication>
#include <QDir>
#include <thread>
#include <memory>

#include "DataDefine.h"
#include "IDEUtil.h"
#include "popwidget/commondialog.h"


#include "datarequire/DataRequireManager.h"

static const int NODE_RPC_PORT = 55555;//node端口  test    formal = test+10
static const int CLIENT_RPC_PORT = 55556;//client端口  test    formal = test+10

static const QString RPC_USER = "a";
static const QString RPC_PASSWORD = "b";

class UbtcBackStage::DataPrivate
{
public:
    DataPrivate(int type,const QString &appDataPath)
        :nodeProc(new QProcess)
        ,clientProc(new QProcess)
        ,chaintype(type)
    {
        nodePort = NODE_RPC_PORT + 10*(type-1);
        clientPort = CLIENT_RPC_PORT + 10*(type-1);

        dataPath = appDataPath + (1 == type ? "/testub" : "/formalub");

        dataRequire = new DataRequireManager("127.0.0.1",QString::number(clientPort));
    }
    ~DataPrivate()
    {
        qDebug()<<"delete ubtcstage";
        clientProc->close();
        nodeProc->close();
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

UbtcBackStage::UbtcBackStage(int type,const QString &appDataPath,QObject *parent)
    : BackStageBase(parent)
    ,_p(new DataPrivate(type,appDataPath))
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
    QString dataPath = _p->dataPath;
    QDir dir(dataPath);
    if(!dir.exists())
    {
        dir.mkpath(dir.path());
    }

    QStringList strList;
    if(1 == _p->chaintype)
    {//测试链 + testnet
        strList << "-datadir=" + dataPath
                << "-testnet"<<"-staking"<<"-port="+QString::number(_p->nodePort)<<"-rpcport="+QString::number(_p->clientPort)
                << "-rpcuser="+RPC_USER<<"-rpcpassword="+RPC_PASSWORD<<"-txindex"<<"-server";
    }
    else if(2 == _p->chaintype)
    {//正式链
        strList << "-datadir=" + dataPath
                <<"-staking"<<"-port="+QString::number(_p->nodePort)<<"-rpcport="+QString::number(_p->clientPort)
                << "-rpcuser="+RPC_USER<<"-rpcpassword="+RPC_PASSWORD<<"-txindex"<<"-server";

    }
    qDebug() << "start ubcd.exe " << strList;
    _p->nodeProc->start(QCoreApplication::applicationDirPath()+QDir::separator()+DataDefine::UBCD_NODE_EXE,strList);
}

bool UbtcBackStage::exeRunning()
{
    return _p->nodeProc->state() == QProcess::Running && _p->dataRequire->isConnected();
}

QProcess *UbtcBackStage::getProcess() const
{
    return _p->clientProc;
}

void UbtcBackStage::ReadyClose()
{
    disconnect(_p->nodeProc,&QProcess::stateChanged,this,&UbtcBackStage::onNodeExeStateChanged);
    if(_p->nodeProc->state() == QProcess::Running)
    {
        QSharedPointer<QEventLoop> loop = QSharedPointer<QEventLoop>(new QEventLoop());

        disconnect(_p->dataRequire,&DataRequireManager::requireResponse,this,&BackStageBase::rpcReceived);
        connect(_p->dataRequire,&DataRequireManager::requireResponse,[&loop,this](const QString &_rpcId,const QString &message){
            if(_rpcId == "id-stop-onCloseIDE")
            {
                if(loop && loop->isRunning())
                {
                    _p->nodeProc->waitForFinished();
                    qDebug()<<"close ub "<<_p->chaintype<<" finish";
                    loop->quit();
                }
            }
        });
        qDebug()<<"close ub "<<_p->chaintype;
        _p->dataRequire->requirePosted("id-stop-onCloseIDE",IDEUtil::toJsonFormat( "stop", QJsonArray()));

        loop->exec();
    }
    emit exeClosed();
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

    _p->dataRequire->setAdditional("Authorization","Basic YTpi");
    _p->dataRequire->startManager(DataRequireManager::HTTPWITHUSER);
}
