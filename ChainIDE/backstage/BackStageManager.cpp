#include "BackStageManager.h"

#include <QTimer>
#include "BackStageBase.h"
#include "LinkBackStage.h"
#include "UbtcBackStage.h"

class BackStageManager::DataPrivate
{
public:
    DataPrivate(DataDefine::BlockChainClass chainclass, DataDefine::ChainTypes starttype, const QString &path)
        :chainClass(chainclass)
        ,startType(starttype)
        ,closeType(starttype)
        ,dataPath(path)
        ,testBackStage(nullptr)
        ,formalBackStage(nullptr)
    {

    }
    ~DataPrivate()
    {
        if(testBackStage)
        {
            delete testBackStage;
            testBackStage = nullptr;
        }
        if(formalBackStage)
        {
            delete formalBackStage;
            formalBackStage = nullptr;
        }
    }
public:
    DataDefine::BlockChainClass chainClass;
    DataDefine::ChainTypes startType;
    DataDefine::ChainTypes closeType;
    QString dataPath;
    BackStageBase *testBackStage;
    BackStageBase *formalBackStage;
};

BackStageManager::BackStageManager(DataDefine::BlockChainClass chainClass, DataDefine::ChainTypes startType,const QString &dataPath,QObject *parent)
    : QObject(parent)
    , _p(new DataPrivate(chainClass,startType,dataPath))
{
    InitBackStage(chainClass,startType);
}

BackStageManager::~BackStageManager()
{
    qDebug()<<"delete backStageManager";
    delete _p;
    _p = nullptr;
}

void BackStageManager::postRPC(const QString &_rpcId, const QString &_rpcCmd, DataDefine::ChainType chainType)
{
    switch (chainType)
    {
    case DataDefine::TEST:
        emit rpcPosted(_rpcId,_rpcCmd);
        break;
    case DataDefine::FORMAL:
        emit rpcPostedFormal(_rpcId,_rpcCmd);
        break;
    default:
        break;
    }
}

void BackStageManager::startBackStage()
{
    //不启动
    if( DataDefine::NONE == _p->startType)
    {
        emit startBackStageFinish();
        return ;
    }

    //启动后台
    if(_p->startType & DataDefine::TEST)
    {
        if(_p->testBackStage)
        {
            connect(_p->testBackStage,&BackStageBase::exeStarted,this,&BackStageManager::exeStartedSlots);
            _p->testBackStage->startExe(_p->dataPath);
        }
    }
    if(_p->startType & DataDefine::FORMAL)
    {
        if(_p->formalBackStage)
        {
            connect(_p->formalBackStage,&BackStageBase::exeStarted,this,&BackStageManager::exeStartedSlots);
            _p->formalBackStage->startExe(_p->dataPath);
        }
    }
}

void BackStageManager::closeBackStage()
{
    if(_p->startType & DataDefine::TEST)
    {
        QTimer::singleShot(10,[this](){
            connect(_p->testBackStage,&BackStageBase::exeClosed,[this](){
                if(0 == (this->_p->closeType &= ~DataDefine::TEST))
                {
                    emit closeBackStageFinish();
                }
            });
            _p->testBackStage->ReadyClose();
        });
    }

    if(_p->startType & DataDefine::FORMAL)
    {
        QTimer::singleShot(10,[this](){
            connect(_p->formalBackStage,&BackStageBase::exeClosed,[this](){
                if(0 == (this->_p->closeType &= ~DataDefine::FORMAL))
                {
                    emit closeBackStageFinish();
                }
            });
            _p->formalBackStage->ReadyClose();
        });
    }
}

void BackStageManager::InitBackStage(DataDefine::BlockChainClass chainClass, DataDefine::ChainTypes startType)
{
    if(DataDefine::HX == chainClass)
    {
        if(startType & DataDefine::TEST)
        {
            _p->testBackStage = new LinkBackStage(1);
        }
        if(startType & DataDefine::FORMAL)
        {
            _p->formalBackStage = new LinkBackStage(2);
        }
    }
    else if(DataDefine::UB == chainClass)
    {
        if(startType & DataDefine::TEST)
        {
            _p->testBackStage = new UbtcBackStage(1);
        }
        if(startType & DataDefine::FORMAL)
        {
            _p->formalBackStage = new UbtcBackStage(2);
        }
    }

    if(_p->testBackStage)
    {
        connect(this,&BackStageManager::rpcPosted,_p->testBackStage,&BackStageBase::rpcPostedSlot);
        connect(_p->testBackStage,&BackStageBase::rpcReceived,this,&BackStageManager::jsonDataUpdated);
    }
    if(_p->formalBackStage)
    {
        connect(this,&BackStageManager::rpcPostedFormal,_p->formalBackStage,&BackStageBase::rpcPostedSlot);
        connect(_p->formalBackStage,&BackStageBase::rpcReceived,this,&BackStageManager::jsonDataUpdated);
    }
}

void BackStageManager::exeStartedSlots()
{
    bool test = false;
    bool formal = false;
    if(_p->startType & DataDefine::TEST)
    {
        if(_p->testBackStage->exeRunning())
        {
            test = true;
            disconnect(_p->testBackStage,&BackStageBase::exeStarted,this,&BackStageManager::exeStartedSlots);
        }
    }
    else
    {
        test = true;
    }

    if(!test) return;

    if(_p->startType & DataDefine::FORMAL)
    {
        if(_p->formalBackStage->exeRunning())
        {
          formal = true;
          disconnect(_p->formalBackStage,&BackStageBase::exeStarted,this,&BackStageManager::exeStartedSlots);
        }
    }
    else
    {
        formal = true;
    }

    if(!formal) return;

    emit startBackStageFinish();
}
