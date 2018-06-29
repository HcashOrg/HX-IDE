#include "gluaCompile.h"

#include <QStringList>
#include <QFileInfo>
#include <QDir>
#include <QCoreApplication>
#include <QDebug>

#include "DataDefine.h"
#include "IDEUtil.h"

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
    QString sourceFile;
    QString dstFilePath;
    QString tempDir;
    QString sourceDir;
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
    _p->sourceFile = sourceFilePath;
    _p->tempDir = QCoreApplication::applicationDirPath()+QDir::separator()+
                  DataDefine::GLUA_COMPILE_TEMP_DIR + QDir::separator() + QFileInfo(sourceFilePath).baseName();
    _p->sourceDir = QFileInfo(sourceFilePath).absoluteDir().absolutePath();

    IDEUtil::deleteDir(_p->tempDir);
    QDir dir(_p->tempDir);
    if(!dir.exists())
    {
        qDebug()<<"dirmake"<<dir.path();
        dir.mkpath(dir.path());
    }

    _p->dstFilePath = _p->sourceDir+"/"+QFileInfo(_p->sourceDir).fileName();

    emit CompileOutput(QString("start compile %1").arg(_p->sourceDir));

    QStringList params;
    params<<"-o"<<_p->tempDir<<"-g"<<_p->sourceFile;
    qDebug()<<"glua-compile"<<params;
    getCompileProcess()->start(QCoreApplication::applicationDirPath()+QDir::separator()+DataDefine::GLUA_COMPILE_PATH,params);

}

void gluaCompile::finishCompile(int exitcode, QProcess::ExitStatus exitStatus)
{
    if(exitStatus == QProcess::NormalExit)
    {
        //生成完毕
        //删除之前的文件
        QFile::remove(_p->dstFilePath+".gpc");
        QFile::remove(_p->dstFilePath+".meta.json");

        //复制gpc meta.json文件到源目录
        QFile::copy(_p->tempDir+"/"+QFileInfo(_p->sourceFile).fileName()+".gpc",_p->dstFilePath+".gpc");
        QFile::copy(_p->tempDir+"/"+QFileInfo(_p->sourceFile).fileName()+".meta.json",_p->dstFilePath+".meta.json");

        //删除临时目录
        IDEUtil::deleteDir(_p->tempDir);

        if(QFile(_p->dstFilePath+".gpc").exists())
        {
            emit CompileOutput(QString("compile finish,see %1").arg(_p->dstFilePath));
            emit finishCompileFile(_p->sourceDir);
        }
    }
    else
    {
        emit CompileOutput(QString("compile error"));
        //删除之前的文件
        QFile::remove(_p->dstFilePath+".gpc");
        QFile::remove(_p->dstFilePath+".meta.json");

        //删除临时目录
        IDEUtil::deleteDir(_p->tempDir);
    }
}

void gluaCompile::onReadStandardOutput()
{

}

void gluaCompile::onReadStandardError()
{
    emit CompileOutput(QString::fromLocal8Bit(getCompileProcess()->readAllStandardError()));
}

