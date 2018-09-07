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
};

DebugManager::DebugManager(QObject *parent)
    : QObject(parent)
    ,_p(new DataPrivate())
{

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
