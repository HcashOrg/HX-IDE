#include "DebugManager.h"

#include <mutex>

#include <QProcess>
#include <QFileInfo>
#include <QDebug>
#include <algorithm>
#include <QTimer>

class DebugManager::DataPrivate
{
public:
    DataPrivate()
        :currentBreakLine(-1)
        ,uvmProcess(new QProcess)
        ,debuggerState(DebugDataStruct::Available)
    {

    }
    ~DataPrivate()
    {
        delete uvmProcess;
        uvmProcess = nullptr;
    }
public:
    QString filePath;
    int currentBreakLine;

    QProcess *uvmProcess;
    DebugDataStruct::DebuggerState debuggerState;

    std::mutex dataMutex;
};

DebugManager::DebugManager(QObject *parent)
    : QObject(parent)
    ,_p(new DataPrivate())
{
    InitDebugger();
}

DebugManager::~DebugManager()
{
    delete _p;
    _p = nullptr;
}

void DebugManager::startDebug(const QString &filePath)
{
    _p->filePath = filePath;
    if(!QFileInfo(_p->filePath).isFile() || !QFileInfo(_p->filePath).exists())
    {
        emit debugError(_p->filePath+" isn't a file or exists");
        return ;
    }

    //获取当前文件所有断点
    fetchBreakPoints(_p->filePath);
    //设置调试器状态
    setDebuggerState(DebugDataStruct::StartDebug);
}

void DebugManager::debugNextStep()
{

}

void DebugManager::debugContinue()
{

}

void DebugManager::stopDebug()
{

}

void DebugManager::fetchBreakPointsFinish(const std::vector<int> &data)
{

    std::for_each(data.begin(),data.end(),[](int in){qDebug()<<in;});
}

DebugDataStruct::DebuggerState DebugManager::getDebuggerState() const
{
    std::lock_guard<std::mutex> loc(_p->dataMutex);
    return _p->debuggerState;
}

void DebugManager::setDebuggerState(DebugDataStruct::DebuggerState state)
{
    std::lock_guard<std::mutex> loc(_p->dataMutex);
    _p->debuggerState = state;
}

void DebugManager::OnProcessStateChanged()
{
    if(_p->uvmProcess->state() == QProcess::Starting)
    {

    }
    else if(_p->uvmProcess->state() == QProcess::Running)
    {

    }
    else if(_p->uvmProcess->state() == QProcess::NotRunning)
    {

    }
}

void DebugManager::readyReadStandardOutputSlot()
{





    emit debugError(_p->uvmProcess->readAllStandardOutput());
}

void DebugManager::readyReadStandardErrorSlot()
{
    emit debugError(_p->uvmProcess->readAllStandardError());
}

void DebugManager::InitDebugger()
{
    connect(_p->uvmProcess,&QProcess::stateChanged,this,&DebugManager::OnProcessStateChanged);
    connect(_p->uvmProcess,&QProcess::readyReadStandardOutput,this,&DebugManager::readyReadStandardOutputSlot);
    connect(_p->uvmProcess,&QProcess::readyReadStandardError,this,&DebugManager::readyReadStandardErrorSlot);
}

void DebugManager::ResetDebugger()
{
    setDebuggerState(DebugDataStruct::Available);

}
