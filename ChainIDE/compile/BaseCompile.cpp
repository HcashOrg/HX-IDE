#include "BaseCompile.h"

#include <QFile>
#include <QDir>
#include <QDebug>
#include "DataDefine.h"

class BaseCompile::DataPrivate
{
public:
    DataPrivate()
        :compileProcess(new QProcess())
        ,compileStage(BaseCompile::StageBegin)
    {

    }
    ~DataPrivate()
    {
        delete compileProcess;
        compileProcess = nullptr;
    }

public:
    QProcess *compileProcess;

    QString tempDir;
    QString sourceDir;
    QString dstByteFilePath;
    QString dstMetaFilePath;
    QString dstOutFilePath;

    BaseCompile::CompileStage compileStage;
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

QProcess *const BaseCompile::getCompileProcess() const
{
    return _p->compileProcess;
}

void BaseCompile::setTempDir(const QString &dir)
{
    _p->tempDir = dir;
}

const QString &BaseCompile::getTempDir() const
{
    return _p->tempDir;
}

void BaseCompile::setSourceDir(const QString &dir)
{
    _p->sourceDir = dir;
}

const QString &BaseCompile::getSourceDir() const
{
    return _p->sourceDir;
}

void BaseCompile::setDstByteFilePath(const QString &path)
{
    _p->dstByteFilePath = path;
}

const QString &BaseCompile::getDstByteFilePath() const
{
    return _p->dstByteFilePath;
}

void BaseCompile::setDstMetaFilePath(const QString &path)
{
    _p->dstMetaFilePath = path;
}

void BaseCompile::setDstOutFilePath(const QString &path)
{
    _p->dstOutFilePath = path;
}

const QString &BaseCompile::getDstMetaFilePath() const
{
    return _p->dstMetaFilePath;
}

const QString &BaseCompile::getDstOutFilePath() const
{
    return _p->dstOutFilePath;
}

void BaseCompile::setCompileStage(BaseCompile::CompileStage sta)
{
    _p->compileStage = sta;
}

BaseCompile::CompileStage BaseCompile::getCompileStage() const
{
    return _p->compileStage;
}

void BaseCompile::readyBuild()
{
    QString dst = getSourceDir()+"/"+QFileInfo(getSourceDir()).fileName();
    setDstByteFilePath(dst+"."+DataDefine::CONTRACT_SUFFIX);
    setDstMetaFilePath(dst+"."+DataDefine::META_SUFFIX);
    setDstOutFilePath(dst+"."+DataDefine::BYTE_OUT_SUFFIX);

    QDir dir(getTempDir());
    if(!dir.exists())
    {
        qDebug()<<"dirmake"<<dir.path();
        dir.mkpath(dir.path());
    }

    //删除之前的文件
    QFile::remove(getDstByteFilePath());
    QFile::remove(getDstMetaFilePath());
}
