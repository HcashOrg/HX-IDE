#include "ExeManager.h"

#include <QProcess>
#include <QTimer>
#include <QDebug>
#include <QSettings>

#include "ChainIDE.h"
#include "websocketmanager.h"
#include "commondialog.h"

static const int NODE_RPC_PORT = 50320;//node端口  test    formal = test+10
static const int CLIENT_RPC_PORT = 50321;//client端口  test    formal = test+10

class ExeManager::DataPrivate
{
public:
    DataPrivate(int type)
        :nodeProc(new QProcess)
        ,clientProc(new QProcess)
        ,chaintype(type)
    {
        nodePort = NODE_RPC_PORT + 10*(type-1);
        clientPort = CLIENT_RPC_PORT + 10*(type-1);
        WSMnager = new WebSocketManager(clientPort);
        dataPath = 1 == type ? "/testDataPath" : "/formalPath";
    }
    ~DataPrivate()
    {
        delete nodeProc;
        nodeProc = nullptr;
        delete clientProc;
        clientProc = nullptr;

    }
public:
    int chaintype;
    int nodePort;
    int clientPort;
    QString dataPath;
    QProcess* nodeProc;
    QProcess* clientProc;
    WebSocketManager *WSMnager;
    QTimer    timerForStartExe;
    QTimer websocketCheckTimer;
};

ExeManager::ExeManager(int type,QObject *parent) : QObject(parent)
        ,_p(new DataPrivate(type))
{

}

ExeManager::~ExeManager()
{
    delete _p;
}

void ExeManager::startExe()
{
    connect(_p->nodeProc,SIGNAL(stateChanged(QProcess::ProcessState)),this,SLOT(onNodeExeStateChanged()));

    QStringList strList;
    strList << "--data-dir=" + ChainIDE::getInstance()->getConfigAppDataPath().replace("\\","/")+_p->dataPath
            << QString("--rpc-endpoint=127.0.0.1:%1").arg(_p->nodePort);

    _p->nodeProc->start("lnk_node.exe",strList);
    qDebug() << "start lnk_node.exe " << strList;

}

bool ExeManager::exeRunning()
{
    return _p->clientProc->state() == QProcess::Running && _p->nodeProc->state() == QProcess::Running;
}

QProcess *ExeManager::getProcess() const
{
    return _p->clientProc;
}

void ExeManager::onNodeExeStateChanged()
{
    qDebug() << "node exe state " << _p->nodeProc->state();

    if(_p->nodeProc->state() == QProcess::Starting)
    {
        qDebug() << QString("%1 is starting").arg("lnk_node.exe");
    }
    else if(_p->nodeProc->state() == QProcess::Running)
    {
        qDebug() << QString("%1 is running").arg("lnk_node.exe");
        connect(&_p->timerForStartExe,SIGNAL(timeout()),this,SLOT(checkNodeExeIsReady()));
        _p->timerForStartExe.start(1000);
    }
    else if(_p->nodeProc->state() == QProcess::NotRunning)
    {
        CommonDialog commonDialog(CommonDialog::OkOnly);
        commonDialog.setText(tr("Fail to launch %1 !").arg("lnk_node.exe"));
        commonDialog.pop();
    }
}

void ExeManager::checkNodeExeIsReady()
{
    QString str = _p->nodeProc->readAllStandardError();
    qDebug() << "node exe standardError: " << str ;
    if(str.contains("Chain ID is"))
    {
        _p->timerForStartExe.stop();
        QTimer::singleShot(1000,this,SLOT(delayedLaunchClient()));
    }
}

void ExeManager::delayedLaunchClient()
{
    connect(_p->clientProc,SIGNAL(stateChanged(QProcess::ProcessState)),this,SLOT(onClientExeStateChanged()));

    QStringList strList;
    strList << "--wallet-file=" + ChainIDE::getInstance()->getConfigAppDataPath().replace("\\","/") +_p->dataPath+ "/wallet.json"
            << QString("--server-rpc-endpoint=ws://127.0.0.1:%1").arg(_p->nodePort)
            << QString("--rpc-endpoint=127.0.0.1:%1").arg(_p->clientPort);

    _p->clientProc->start("lnk_client.exe",strList);
}

void ExeManager::onClientExeStateChanged()
{
    if(_p->clientProc->state() == QProcess::Starting)
    {
        qDebug() << QString("%1 is starting").arg("lnk_client.exe");
    }
    else if(_p->clientProc->state() == QProcess::Running)
    {
        qDebug() << QString("%1 is running").arg("lnk_client.exe");

        initWebSocketManager();

        connect(&_p->websocketCheckTimer,SIGNAL(timeout()),this,SLOT(checkWebsocketConnected()));
        _p->websocketCheckTimer.start(1000);

    }
    else if(_p->clientProc->state() == QProcess::NotRunning)
    {
        qDebug() << "client not running" + _p->clientProc->errorString();
        CommonDialog commonDialog(CommonDialog::OkOnly);
        commonDialog.setText(tr("Fail to launch %1 !").arg("lnk_client.exe"));
        commonDialog.pop();
    }
}

void ExeManager::initWebSocketManager()
{
    _p->WSMnager->start();
    _p->WSMnager->moveToThread(_p->WSMnager);
}

void ExeManager::checkWebsocketConnected()
{
    if(_p->WSMnager == nullptr)   return;
    if(_p->WSMnager->isConnected)
    {
        _p->websocketCheckTimer.stop();
        emit exeStarted();
    }
}

void ExeManager::rpcPostedSlot(QString id, QString cmd)
{
    _p->WSMnager->processRPCs(id,cmd);
}
