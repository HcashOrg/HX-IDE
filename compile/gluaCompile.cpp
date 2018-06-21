#include "gluaCompile.h"

#include <QStringList>
#include <QFileInfo>
#include <QDir>
#include <QCoreApplication>
#include <QDebug>

#include "DataDefine.h"
#include "IDEUtil.h"

static const QString GluaCompileDir = "GluaTemp";

class gluaCompile::DataPrivate
{
public:
    DataPrivate()
    {

    }
    ~DataPrivate()
    {
    }

public:

    QString dstFilePath;
};

gluaCompile::gluaCompile(QObject *parent)
    :BaseCompile(parent)
    ,_p(new DataPrivate())
{
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
    params<<"-o"<<QFileInfo(_p->dstFilePath).absoluteDir().path()<<"-g"<<sourceFilePath;
    foreach (QString is, params) {
        qDebug()<<is;
    }
    getCompileProcess()->start(QCoreApplication::applicationDirPath()+QDir::separator()+DataDefine::GLUA_COMPILE_PATH,params);

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

void gluaCompile::onReadStandardOutput()
{

}

void gluaCompile::onReadStandardError()
{
    emit CompileOutput(QString::fromLocal8Bit(getCompileProcess()->readAllStandardError()));
}

