#include "gluaCompile.h"

#include <QStringList>
#include <QFileInfo>
#include <QDir>
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
    params<<"-g"<<sourceFilePath;
    getCompileProcess()->start("compile/glua_compiler.exe",params);

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

