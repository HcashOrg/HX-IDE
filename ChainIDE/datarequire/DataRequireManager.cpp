#include "DataRequireManager.h"

#include <mutex>
#include <QTimer>
#include "RequireBase.h"
#include "websocketRequire.h"
#include "tcpsocketRequire.h"
#include "httpRequire.h"

static const QString SPLITFLAG = "****";
static const int MAXLOOPNUMBER = 2000;

class DataRequireManager::DataPrivate
{
public:
    DataPrivate(const QString &ip,const QString & port)
        :isBusy(false)
        ,requireTimer(new QTimer())
        ,requireBase(nullptr)
        ,connectPort(port)
        ,connectIP(ip)
        ,loopNumber(0)
    {

    }
    ~DataPrivate()
    {
        if(requireTimer)
        {
            delete requireTimer;
            requireTimer = nullptr;
        }
        if(requireBase)
        {
            delete requireBase;
            requireBase = nullptr;
        }
    }

public:
    QStringList pendingRpcs;//请求内容
    std::mutex dataMutex;//数据锁
    QTimer *requireTimer;//请求时间控制器，自动循环
    bool isBusy;//当前是否有未返回请求

    RequireBase *requireBase;//请求主体
    QString connectPort;
    QString connectIP;
    int loopNumber;//循环次数，超出一定次数就跳过该请求

    //多余信息--http使用
    QByteArray header;
    QByteArray values;
};

DataRequireManager::DataRequireManager(const QString &ip,const QString & connectPort,QObject *parent)
    : QObject(parent)
    ,_p(new DataPrivate(ip,connectPort))
{
    InitManager();
}

DataRequireManager::~DataRequireManager()
{
    qDebug()<<QString("delete datarequiremanager with ip:%1 port:%2").arg(_p->connectIP).arg(_p->connectPort);
    delete _p;
    _p = nullptr;
}

void DataRequireManager::requirePosted(const QString &_rpcId, const QString & _rpcCmd)
{
    std::lock_guard<std::mutex> loc1(_p->dataMutex);
    _p->pendingRpcs.append(_rpcId + SPLITFLAG + _rpcCmd);
}

void DataRequireManager::receiveResponse(const QString &message)
{
    QString id = _p->pendingRpcs.empty()?"":_p->pendingRpcs.at(0).split(SPLITFLAG).at(0);

    {
        std::lock_guard<std::mutex> loc2(_p->dataMutex);
        _p->isBusy = false;
        _p->loopNumber = 0;
        if(!_p->pendingRpcs.empty())
        {//将第一个数据移除，发送收到回复的消息
            _p->pendingRpcs.removeFirst();
        }
    }
    if(!id.isEmpty())
    {
        emit requireResponse(id,message);
    }
}

void DataRequireManager::requireClear()
{
    std::lock_guard<std::mutex> loc(_p->dataMutex);
    _p->isBusy = false;
    _p->pendingRpcs.clear();
}

void DataRequireManager::setAdditional(const QByteArray &headerName, const QByteArray &value)
{
    _p->header = headerName;
    _p->values = value;
}

bool DataRequireManager::isConnected() const
{
    if(_p->requireBase)
    {
        return _p->requireBase->isConnected();
    }
    return false;
}

void DataRequireManager::startManager(ConnectType connecttype)
{
    if(connecttype == WEBSOCKET)
    {
        _p->requireBase = new websocketRequire(_p->connectIP,_p->connectPort);
    }
    else if(connecttype == TCP)
    {
        _p->requireBase = new tcpsocketRequire(_p->connectIP,_p->connectPort);
    }
    else if(connecttype == HTTP)
    {
        _p->requireBase = new httpRequire(_p->connectIP,_p->connectPort);
    }
    else if(connecttype == HTTPWITHUSER)
    {
        _p->requireBase = new httpRequire(_p->connectIP,_p->connectPort);
        dynamic_cast<httpRequire*>(_p->requireBase)->setRawHeader(_p->header,_p->values);
    }

    connect(_p->requireBase,&RequireBase::receiveData,this,&DataRequireManager::receiveResponse);
    connect(_p->requireBase,&RequireBase::connectFinish,this,&DataRequireManager::connectFinish);

    qDebug()<<"start connect datarequire "<<_p->connectIP<<_p->connectPort;
    _p->requireBase->startConnect();
    _p->requireTimer->start(30);
}

void DataRequireManager::processRequire()
{
    if(_p->isBusy)
    {
        std::lock_guard<std::mutex> loc(_p->dataMutex);
        ++_p->loopNumber;
        if(_p->loopNumber >= MAXLOOPNUMBER)
        {
            //请求超时，清楚该请求，继续下一个
            if(!_p->pendingRpcs.empty())
            {
                qDebug()<<"require outtime "<<_p->pendingRpcs.front();
            }
            _p->loopNumber = 0;
            _p->pendingRpcs.removeFirst();
            _p->isBusy = false;
        }
    }

    if(_p->isBusy || _p->pendingRpcs.empty()) return;

    _p->isBusy = true;
    QStringList rpc = _p->pendingRpcs.at(0).split(SPLITFLAG);
    _p->requireBase->postData(rpc.at(1));
}

void DataRequireManager::InitManager()
{
    connect(_p->requireTimer,&QTimer::timeout,this,&DataRequireManager::processRequire);
}
