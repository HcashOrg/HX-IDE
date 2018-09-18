#include "LinkBackStage.h"

#include <QProcess>
#include <QTimer>
#include <QDebug>
#include <QCoreApplication>
#include <QDir>

#include "DataDefine.h"
#include "IDEUtil.h"
#include "popwidget/commondialog.h"

#include "datarequire/DataRequireManager.h"

static const int NODE_RPC_PORT = 60320;//node端口  test    formal = test+10
static const int CLIENT_RPC_PORT = 60321;//client端口  test    formal = test+10

class LinkBackStage::DataPrivate
{
public:
    DataPrivate(int type)
        :nodeProc(new QProcess)
        ,clientProc(new QProcess)
        ,chaintype(type)
    {
        nodePort = NODE_RPC_PORT + 10*(type-1);
        clientPort = CLIENT_RPC_PORT + 10*(type-1);

        dataPath = 1 == type ? "/testhx" : "/formalhx";

        dataRequire = new DataRequireManager("127.0.0.1",QString::number(clientPort));
    }
    ~DataPrivate()
    {
        clientProc->close();
        nodeProc->close();
        delete dataRequire;
        dataRequire = nullptr;
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
    QTimer    timerForStartExe;
    DataRequireManager *dataRequire;
};

LinkBackStage::LinkBackStage(int type,QObject *parent)
        : BackStageBase(parent)
        ,_p(new DataPrivate(type))
{

}

LinkBackStage::~LinkBackStage()
{
    qDebug()<<"delete "<<_p->chaintype<<" hxstage";
    delete _p;
}

void LinkBackStage::startExe(const QString &appDataPath)
{
    //设置数据存储路径
    QString str = appDataPath;
    str.replace("\\","/");
    _p->dataPath =str + _p->dataPath;

    connect(_p->nodeProc,&QProcess::stateChanged,this,&LinkBackStage::onNodeExeStateChanged);
    QStringList strList;
    strList << "--data-dir=" +_p->dataPath
            << QString("--rpc-endpoint=127.0.0.1:%1").arg(_p->nodePort);

    qDebug() << "start hx_node " << strList;
    _p->nodeProc->start(QCoreApplication::applicationDirPath()+QDir::separator()+DataDefine::LINK_NODE_EXE,strList);

}

bool LinkBackStage::exeRunning()
{
    return _p->clientProc->state() == QProcess::Running && _p->nodeProc->state() == QProcess::Running && _p->dataRequire->isConnected();
}

QProcess *LinkBackStage::getProcess() const
{
    return _p->clientProc;
}

void LinkBackStage::ReadyClose()
{
    if(_p->clientProc->state() == QProcess::Running)
    {
        disconnect(_p->clientProc,&QProcess::stateChanged,this,&LinkBackStage::onClientExeStateChanged);
        disconnect(_p->nodeProc,&QProcess::stateChanged,this,&LinkBackStage::onNodeExeStateChanged);
        //先lock

        QSharedPointer<QEventLoop> loop = QSharedPointer<QEventLoop>(new QEventLoop());

        connect(_p->dataRequire,&DataRequireManager::requireResponse,[&loop,this](const QString &_rpcId,const QString &message){
            if(_rpcId == "id-lock-onCloseIDE")
            {
                rpcPostedSlot( "id-witness_node_stop",IDEUtil::toJsonFormat( "witness_node_stop", QJsonArray()));
            }
            else if(_rpcId == "id-witness_node_stop")
            {
                if(loop && loop->isRunning())
                {
                    _p->clientProc->waitForFinished();
                    _p->nodeProc->waitForFinished();
                    qDebug()<<"close hxstage "<<_p->chaintype<<" finish";
                    loop->quit();
                }
            }
        });
        rpcPostedSlot("id-lock-onCloseIDE",IDEUtil::toJsonFormat( "lock", QJsonArray()));

        loop->exec();
        //IDEUtil::msecSleep(5000);
    }
    emit exeClosed();
}

void LinkBackStage::onNodeExeStateChanged()
{
    if(_p->nodeProc->state() == QProcess::Starting)
    {
        //qDebug() << QString("%1 is starting").arg("hx_node");
    }
    else if(_p->nodeProc->state() == QProcess::Running)
    {
        qDebug() << QString("hx_node %1 is running").arg(_p->chaintype);
        connect(&_p->timerForStartExe,&QTimer::timeout,this,&LinkBackStage::checkNodeExeIsReady);
        _p->timerForStartExe.start(100);
    }
    else if(_p->nodeProc->state() == QProcess::NotRunning)
    {
        qDebug()<<QString("hx_node %1 is notrunning :%2").arg(_p->chaintype).arg(_p->nodeProc->errorString());
        CommonDialog commonDialog(CommonDialog::OkOnly);
        commonDialog.setText(tr("Fail to launch hx_node !"));
        commonDialog.pop();
    }
}

void LinkBackStage::checkNodeExeIsReady()
{
    QString str = _p->nodeProc->readAllStandardError();
    if(!str.isEmpty())
    {
        qDebug() << "node exe standardError: " << str ;
        emit AdditionalOutputMessage(str);
    }
    if(str.contains("Chain ID is"))
    {
        _p->timerForStartExe.stop();
        QTimer::singleShot(100,this,&LinkBackStage::delayedLaunchClient);
    }
}

void LinkBackStage::delayedLaunchClient()
{
    connect(_p->clientProc,&QProcess::stateChanged,this,&LinkBackStage::onClientExeStateChanged);

    QStringList strList;
    strList << "--wallet-file=" + _p->dataPath + "/wallet.json"
            << QString("--server-rpc-endpoint=ws://127.0.0.1:%1").arg(_p->nodePort)
            << QString("--rpc-endpoint=127.0.0.1:%1").arg(_p->clientPort);

    _p->clientProc->start(QCoreApplication::applicationDirPath()+QDir::separator()+DataDefine::LINK_CLIENT_EXE,strList);
}

void LinkBackStage::onClientExeStateChanged()
{
    if(_p->clientProc->state() == QProcess::Starting)
    {
        //qDebug() << QString("%1 is starting").arg("hx_client.exe");
    }
    else if(_p->clientProc->state() == QProcess::Running)
    {
        qDebug() << QString("hx_client %1 is running").arg(_p->chaintype);

        initSocketManager();
    }
    else if(_p->clientProc->state() == QProcess::NotRunning)
    {
        qDebug() << QString("hx_client %1 not running :%2").arg(_p->chaintype).arg(_p->clientProc->errorString());
        CommonDialog commonDialog(CommonDialog::OkOnly);
        commonDialog.setText(tr("Fail to launch %1 !").arg("hx_client"));
        commonDialog.pop();
    }
}

void LinkBackStage::initSocketManager()
{
    connect(_p->dataRequire,&DataRequireManager::requireResponse,this,&LinkBackStage::rpcReceivedSlot);
    connect(_p->dataRequire,&DataRequireManager::connectFinish,this,&BackStageBase::exeStarted);

    _p->dataRequire->startManager(DataRequireManager::WEBSOCKET);
}

void LinkBackStage::rpcPostedSlot(const QString & id, const QString & cmd)
{
    _p->dataRequire->requirePosted(id,cmd);
}

void LinkBackStage::rpcReceivedSlot(const QString &id, const QString &message)
{
    QString result = message.mid( QString("{\"id\":32800,\"jsonrpc\":\"2.0\",").size());
    result = result.left( result.size() - 1);
    result.prepend("{");
    result.append("}");
    emit rpcReceived(id,result);
}
