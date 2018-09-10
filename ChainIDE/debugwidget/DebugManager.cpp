#include "DebugManager.h"

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
    DebugDataStruct::DebugerState debuggerState;
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
    //获取

    fetchBreakPoints(_p->filePath);
}

void DebugManager::fetchBreakPointsFinish(const std::vector<int> &data)
{
    std::for_each(data.begin(),data.end(),[](int in){qDebug()<<in;});
}

DebugDataStruct::DebugerState DebugManager::getDebuggerState() const
{
    return _p->debuggerState;
}

void DebugManager::setDebuggerState(DebugDataStruct::DebugerState state)
{
    _p->debuggerState = state;
}

void DebugManager::OnProcessStateChanged()
{
    if(_p->uvmProcess->state() == QProcess::Running)
    {

    }
    else if(_p->uvmProcess->state() == QProcess::Running)
    {

    }
}

void DebugManager::InitDebugger()
{
    connect(_p->uvmProcess,&QProcess::stateChanged,this,&DebugManager::OnProcessStateChanged);
}
