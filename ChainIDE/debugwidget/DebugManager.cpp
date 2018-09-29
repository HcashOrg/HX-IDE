#include "DebugManager.h"

#include <mutex>

#include <QCoreApplication>
#include <QProcess>
#include <QFileInfo>
#include <QDebug>
#include <algorithm>
#include <QTimer>

#include "DataDefine.h"

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
    QString outFilePath;
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
    qDebug()<<"delete debugMmanager";
    delete _p;
    _p = nullptr;
}

void DebugManager::setOutFile(const QString &outFile)
{
    _p->outFilePath = outFile;
}

void DebugManager::startDebug(const QString &filePath,const QString &api,const QStringList &param)
{
    _p->filePath = filePath;
    if(!QFileInfo(_p->filePath).isFile() || !QFileInfo(_p->filePath).exists())
    {
        emit debugError(_p->filePath+" isn't a file or exists");
        return ;
    }
    //获取.out字节码

    //启动单步调试器
    QStringList params;
    params<<"-luvmdebug"<<_p->outFilePath/*<<api<<param*/;

    qDebug()<<"start debug"<<QCoreApplication::applicationDirPath()+"/"+DataDefine::DEBUGGER_UVM_PATH<<params;
    _p->uvmProcess->start(QCoreApplication::applicationDirPath()+"/"+DataDefine::DEBUGGER_UVM_PATH,params);

}

void DebugManager::debugNextStep()
{
    setDebuggerState(DebugDataStruct::StepDebug);
    fetchBreakPoints(_p->filePath);
}

void DebugManager::debugContinue()
{

    setDebuggerState(DebugDataStruct::ContinueDebug);
    fetchBreakPoints(_p->filePath);
}

void DebugManager::stopDebug()
{
    _p->uvmProcess->write("continue\n");
    _p->uvmProcess->close();
    ResetDebugger();
    emit debugFinish();
}

void DebugManager::getVariantInfo()
{
    _p->uvmProcess->write("info locals\n");
}

void DebugManager::fetchBreakPointsFinish(const std::vector<int> &data)
{
    //获取到文件的断点信息
    if(getDebuggerState() == DebugDataStruct::StartDebug)
    {
        //启动单步调试器

    }
    else if(getDebuggerState() == DebugDataStruct::StepDebug)
    {
        qDebug()<<"write single step";
        _p->uvmProcess->write("step\n");
        getVariantInfo();
    }
    else if(getDebuggerState() == DebugDataStruct::ContinueDebug)
    {
        qDebug()<<"write continue";
        _p->uvmProcess->write("breakpoint ?:19\n");
        getVariantInfo();
    }
    //std::for_each(data.begin(),data.end(),[](int in){qDebug()<<in;});
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

void DebugManager::ReadyClose()
{
    if(_p->uvmProcess->state() == QProcess::Running)
    {
        _p->uvmProcess->write("continue\n");
        _p->uvmProcess->waitForReadyRead();
    }
    _p->uvmProcess->close();
}

void DebugManager::OnProcessStateChanged()
{
    if(_p->uvmProcess->state() == QProcess::Starting)
    {

    }
    else if(_p->uvmProcess->state() == QProcess::Running)
    {
        //获取当前文件所有断点
        fetchBreakPoints(_p->filePath);
        //设置调试器状态
        setDebuggerState(DebugDataStruct::StartDebug);
        emit debugStarted();
        testData();
    }
    else if(_p->uvmProcess->state() == QProcess::NotRunning)
    {
        qDebug()<<"not run";
        ResetDebugger();
        emit debugFinish();
    }
}

void DebugManager::readyReadStandardOutputSlot()
{
    if(getDebuggerState() == DebugDataStruct::StartDebug)
    {

    }
    else if(getDebuggerState() == DebugDataStruct::StepDebug)
    {

    }
    else if(getDebuggerState() == DebugDataStruct::ContinueDebug)
    {

    }
    else if(getDebuggerState() == DebugDataStruct::FinishDebug)
    {

    }
    emit debugOutput(_p->uvmProcess->readAllStandardOutput());
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

void DebugManager::testData()
{
    BaseItemDataPtr root = std::make_shared<BaseItemData>();

    BaseItemDataPtr pa = std::make_shared<BaseItemData>("测试","2","vector",root);
    root->appendChild(pa);
    BaseItemDataPtr pa1 = std::make_shared<BaseItemData>("a","13","int",pa);
    pa->appendChild(pa1);
    BaseItemDataPtr pa2 = std::make_shared<BaseItemData>("b","14","int",pa);
    pa->appendChild(pa2);

    BaseItemDataPtr pa3 = std::make_shared<BaseItemData>("item1","0x45241254","pair<string,int>",root);
    root->appendChild(pa3);
    BaseItemDataPtr pa6 = std::make_shared<BaseItemData>("item2","0x45249664","pair<string,string>",root);
    root->appendChild(pa6);


    BaseItemDataPtr pa4 = std::make_shared<BaseItemData>("test","0","string",pa3);
    pa3->appendChild(pa4);
    BaseItemDataPtr pa5 = std::make_shared<BaseItemData>("test1","4","int",pa3);
    pa3->appendChild(pa5);

    BaseItemDataPtr pa7 = std::make_shared<BaseItemData>("no_tt","ims","string",pa6);
    pa6->appendChild(pa7);
    BaseItemDataPtr pa8 = std::make_shared<BaseItemData>("ok_yy","ok that","string",pa6);
    pa6->appendChild(pa8);

    emit showVariant(root);

}
