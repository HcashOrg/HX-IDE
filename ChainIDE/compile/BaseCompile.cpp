#include "BaseCompile.h"

class BaseCompile::DataPrivate
{
public:
    DataPrivate()
        :compileProcess(new QProcess())
    {

    }
    ~DataPrivate()
    {
        delete compileProcess;
        compileProcess = nullptr;
    }

public:
    QProcess *compileProcess;
};

BaseCompile::BaseCompile(QObject *parent) : QObject(parent)
  ,_p(new DataPrivate())
{
    connect(_p->compileProcess,static_cast<void (QProcess::*)(int , QProcess::ExitStatus)>(&QProcess::finished),
            this,&BaseCompile::finishCompile);

    connect(_p->compileProcess,&QProcess::readyReadStandardOutput,this,&BaseCompile::onReadStandardOutput);
    connect(_p->compileProcess,&QProcess::readyReadStandardError,this,&BaseCompile::onReadStandardError);
}

BaseCompile::~BaseCompile()
{
    delete _p;
}

QProcess *BaseCompile::getCompileProcess() const
{
    return _p->compileProcess;
}
