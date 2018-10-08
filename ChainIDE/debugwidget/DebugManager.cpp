#include "DebugManager.h"

#include <mutex>
#include <algorithm>

#include <QCoreApplication>
#include <QProcess>
#include <QFileInfo>
#include <QDebug>
#include <QTimer>

#include "DataDefine.h"
#include "DebugUtil.h"

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
    std::vector<int> commentLines;
    std::mutex breakMutex;

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
        emit debugOutput(_p->filePath+" isn't a file or exists");
        emit debugError();
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
    CancelBreakPoint();
    ResetDebugger();
    _p->uvmProcess->write("continue\n");
    _p->uvmProcess->close();
    emit debugFinish();
}

void DebugManager::getVariantInfo()
{
    setDebuggerState(DebugDataStruct::QueryInfo);
    _p->uvmProcess->write("info locals\n");
}

void DebugManager::fetchBreakPointsFinish(const std::vector<int> &data)
{
    ModifyBreakPoint(data);
    //获取到文件的断点信息
    if(getDebuggerState() == DebugDataStruct::StartDebug)
    {
        //计算当前文件的注释行
        DebugUtil::getCommentLine(_p->filePath,_p->commentLines);

        ModifyBreakPoint(data);

        emit debugStarted();

    }
    else if(getDebuggerState() == DebugDataStruct::StepDebug)
    {
        _p->uvmProcess->write("step\n");
    }
    else if(getDebuggerState() == DebugDataStruct::ContinueDebug)
    {
        int breakLine = getNextBreakPoint(GetCurrentBreakLine(),data);
        if(0 < breakLine)
        {
            SetBreakPoint(_p->filePath,breakLine);
        }
        _p->uvmProcess->write("continue\n");
    }
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

const QString &DebugManager::getCurrentDebugFile() const
{
    return _p->filePath;
}

void DebugManager::OnProcessStateChanged()
{
    if(_p->uvmProcess->state() == QProcess::Starting)
    {

    }
    else if(_p->uvmProcess->state() == QProcess::Running)
    {
        //设置调试器状态
        setDebuggerState(DebugDataStruct::StartDebug);

        //获取当前文件所有断点
        fetchBreakPoints(_p->filePath);

    }
    else if(_p->uvmProcess->state() == QProcess::NotRunning)
    {
        qDebug()<<"debugger not running";
        ResetDebugger();
        emit debugFinish();
    }
}

void DebugManager::readyReadStandardOutputSlot()
{
    QString outPut = _p->uvmProcess->readAllStandardOutput();
    if(getDebuggerState() == DebugDataStruct::StartDebug)
    {

    }
    else if(getDebuggerState() == DebugDataStruct::StepDebug)
    {
        getVariantInfo();
    }
    else if(getDebuggerState() == DebugDataStruct::ContinueDebug)
    {
        getVariantInfo();
    }
    else if(getDebuggerState() == DebugDataStruct::QueryInfo)
    {
        ParseQueryInfo(outPut);
    }
    ParseBreakPoint(outPut);

    emit debugOutput(outPut);
}

void DebugManager::readyReadStandardErrorSlot()
{
    emit debugOutput(_p->uvmProcess->readAllStandardError());
    emit debugError();
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
    SetCurrentBreakLine(-1);
}

void DebugManager::ParseQueryInfo(const QString &info)
{
    BaseItemDataPtr root = std::make_shared<BaseItemData>();

    QStringList data = info.split("\r\n");
    QRegExp rx("(.*)=(.*)");

    foreach (QString eachdata, data) {

        if(rx.indexIn(eachdata) < 0 || rx.cap(1).isEmpty() || rx.cap(2).isEmpty()) continue;
        //开始构造显示内容
        BaseItemDataPtr pa = std::make_shared<BaseItemData>(rx.cap(1),rx.cap(2),"unknow",root);
        root->appendChild(pa);
    }

    emit showVariant(root);
}

void DebugManager::ParseBreakPoint(const QString &info)
{
    QString data = info.simplified();
    QRegExp rx("hit breakpoint at (.*):(\\d+)",Qt::CaseInsensitive);
    rx.indexIn(data);
    if(rx.indexIn(data) < 0 || rx.cap(1).isEmpty() || rx.cap(2).isEmpty()) return;
    SetCurrentBreakLine(rx.cap(2).toInt());
    emit debugBreakAt(_p->filePath,rx.cap(2).toInt());
}

void DebugManager::SetBreakPoint(const QString &file, int lineNumber)
{
    setDebuggerState(DebugDataStruct::SetBreakPoint);
    _p->uvmProcess->write(QString("break ? %1\n").arg(QString::number(lineNumber)).toStdString().c_str());
    _p->uvmProcess->waitForReadyRead();
}

void DebugManager::CancelBreakPoint()
{
    setDebuggerState(DebugDataStruct::DeleteBreakPoint);
    _p->uvmProcess->write("delete\n");
    _p->uvmProcess->waitForReadyRead();
}

void DebugManager::SetCurrentBreakLine(int li)
{
    std::lock_guard<std::mutex> loc(_p->breakMutex);
    _p->currentBreakLine = li;
}

int DebugManager::GetCurrentBreakLine() const
{
    std::lock_guard<std::mutex> loc(_p->breakMutex);
    return _p->currentBreakLine;
}

int DebugManager::getNextBreakPoint(int currentBreak, const std::vector<int> &lineVec)
{
    auto it = std::find_if(lineVec.begin(),lineVec.end(),[currentBreak,this](int li){
        return (li>currentBreak && this->_p->commentLines.end() == std::find(this->_p->commentLines.begin(),this->_p->commentLines.end(),li));
    });
    if(lineVec.end()!= it)
    {
        return *it;
    }
    return -1;
}

void DebugManager::ModifyBreakPoint(const std::vector<int> &data)
{
    //调整断点情况
    std::vector<int> temp = data;
    std::for_each(temp.begin(),temp.end(),[this](int li){
        if(this->_p->commentLines.end() != std::find(this->_p->commentLines.begin(),this->_p->commentLines.end(),li))
        {
            //如果断点在注释行中，就删除，并且顺移到下一个非注释行
            emit removeBreakPoint(this->_p->filePath,li);
            int line = li;
            while(this->_p->commentLines.end() != std::find(this->_p->commentLines.begin(),this->_p->commentLines.end(),line))
            {
                ++line;
            }
            emit addBreakPoint(this->_p->filePath,line);
        }
    });
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
