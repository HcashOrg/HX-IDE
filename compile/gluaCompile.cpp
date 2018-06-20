#include "gluaCompile.h"

#include <QProcess>
#include <QStringList>
#include <QFileInfo>
#include <QDir>
class gluaCompile::DataPrivate
{
public:
    DataPrivate()
        :compileProcess(new QProcess)
    {

    }
    ~DataPrivate()
    {
        delete compileProcess;
        compileProcess = nullptr;
    }

public:
    QProcess *compileProcess;

    QString dstFilePath;
};

gluaCompile::gluaCompile(QObject *parent)
    :BaseCompile(parent)
    ,_p(new DataPrivate())
{
    connect(_p->compileProcess,static_cast<void (QProcess::*)(int , QProcess::ExitStatus)>(&QProcess::finished),
            this,&gluaCompile::finishCompile);

    connect(_p->compileProcess,&QProcess::readyReadStandardOutput,this,&gluaCompile::readyReadStandardOutput);
}

gluaCompile::~gluaCompile()
{
    delete _p;
    _p = nullptr;
}

void gluaCompile::startCompileFile(const QString &sourceFilePath)
{
    //当前文件路径名
    _p->dstFilePath = sourceFilePath;
    _p->dstFilePath.append(".gpc");


    emit CompileOutput(QString("start compile %1").arg(sourceFilePath));

    QStringList params;
    params<<"-g"<<sourceFilePath;
    _p->compileProcess->start("compile/glua_compiler.exe",params);

}

void gluaCompile::finishCompile(int exitcode, QProcess::ExitStatus exitStatus)
{
    if(exitStatus == QProcess::NormalExit)
    {
        emit CompileOutput(QString("compile finish,open %1").arg(_p->dstFilePath));
        emit finishCompileFile(_p->dstFilePath);
    }
    else
    {
        emit CompileOutput(QString("compile error"));
    }
}

void gluaCompile::readyReadStandardOutput()
{
    emit CompileOutput(QString::fromLocal8Bit(_p->compileProcess->readAllStandardOutput()));
}
