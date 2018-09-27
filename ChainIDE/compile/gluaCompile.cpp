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

void gluaCompile::initConfig(const QString &sourceFilePath)
{
    //当前文件路径名
    _p->sourceFile = sourceFilePath;
    setTempDir( QCoreApplication::applicationDirPath()+QDir::separator()+
                DataDefine::GLUA_COMPILE_TEMP_DIR + QDir::separator() + QFileInfo(sourceFilePath).baseName()
               );
    setSourceDir(IDEUtil::getNextDir(QCoreApplication::applicationDirPath()+QDir::separator()+DataDefine::GLUA_DIR,
                                        sourceFilePath)
                 );
    //清空临时目录
    IDEUtil::deleteDir(getTempDir());
    readyBuild();
}

void gluaCompile::startCompileFile(const QString &sourceFilePath)
{
    initConfig(sourceFilePath);

    emit CompileOutput(QString("start compile %1").arg(getSourceDir()));

    //先生成.out文件，为了调试器使用
    generateOutFile();
}

void gluaCompile::finishCompile(int exitcode, QProcess::ExitStatus exitStatus)
{
    if(QProcess::NormalExit != exitStatus)
    {
        //删除之前的文件
        QFile::remove(getDstByteFilePath());
        QFile::remove(getDstMetaFilePath());
        QFile::remove(getDstOutFilePath());

        //删除临时目录
        IDEUtil::deleteDir(getTempDir());

        emit CompileOutput(QString("compile error"));
        emit errorCompileFile(getSourceDir());
        return;
    }

    //生成完毕
    switch (getCompileStage()) {
    case BaseCompile::StageOne:
        //重命名.out文件名称
        QFile(getSourceDir()+"/"+ QFileInfo(_p->sourceFile).fileName().append(".out")).rename(getDstOutFilePath());
        //进行第二步，生成合约gpc文件
        generateContractFile();
        break;
    case BaseCompile::StageTwo:
        //复制gpc meta.json文件到源目录
        QFile::copy(getTempDir()+"/"+QFileInfo(_p->sourceFile).fileName()+"."+DataDefine::CONTRACT_SUFFIX,getDstByteFilePath());
        QFile::copy(getTempDir()+"/"+QFileInfo(_p->sourceFile).fileName()+"."+DataDefine::META_SUFFIX,getDstMetaFilePath());

        //删除临时目录
        IDEUtil::deleteDir(getTempDir());

        if(QFile(getDstByteFilePath()).exists())
        {
            emit CompileOutput(QString("compile finish,see %1").arg(getDstByteFilePath()));
            emit finishCompileFile(getDstByteFilePath());
        }
        else
        {
            emit CompileOutput(QString("compile error,cann't find :%1").arg(getDstByteFilePath()));
            emit errorCompileFile(getSourceDir());
        }
        break;
    default:
        break;
    }


}

void gluaCompile::onReadStandardOutput()
{
    emit CompileOutput(QString::fromLocal8Bit(getCompileProcess()->readAll()));
}

void gluaCompile::onReadStandardError()
{
    emit CompileOutput(QString::fromLocal8Bit(getCompileProcess()->readAllStandardError()));
}

void gluaCompile::generateOutFile()
{
    setCompileStage(BaseCompile::StageOne);
    QStringList params;
    params<<_p->sourceFile;
    qDebug()<<"glua-compiler-.out: "<<DataDefine::GLUA_COMPILE_PATH<<params;
    getCompileProcess()->start(QCoreApplication::applicationDirPath()+QDir::separator()+DataDefine::GLUA_COMPILE_PATH,params);
}

void gluaCompile::generateContractFile()
{
    setCompileStage(BaseCompile::StageTwo);

    QStringList params;
    params<<"-o"<<getTempDir()<<"-g"<<_p->sourceFile;
    qDebug()<<"glua-compiler-.pgc: "<<DataDefine::GLUA_COMPILE_PATH<<params;
    getCompileProcess()->start(QCoreApplication::applicationDirPath()+QDir::separator()+DataDefine::GLUA_COMPILE_PATH,params);
}


