#include "javaCompile.h"

#include <QStringList>
#include <QFileInfo>
#include <QDir>
#include <QCoreApplication>
#include <QDebug>

#include "IDEUtil.h"

static const QString JavaCompileDir = "javaTemp";

class javaCompile::DataPrivate
{
public:
    DataPrivate()
        :currentState(0)
    {

    }
    ~DataPrivate()
    {
    }

public:
    QString dstFilePath;
    QString tempDir;
    QString sourceDir;

    int currentState;
};

javaCompile::javaCompile(QObject *parent)
    :BaseCompile(parent)
    ,_p(new DataPrivate())
{

}

javaCompile::~javaCompile()
{
    delete _p;
    _p = nullptr;
}

void javaCompile::startCompileFile(const QString &sourceFilePath)
{
    _p->tempDir = QCoreApplication::applicationDirPath()+QDir::separator()+JavaCompileDir + QDir::separator() + QFileInfo(sourceFilePath).baseName();
    _p->sourceDir = QFileInfo(sourceFilePath).absoluteDir().absolutePath();

    emit CompileOutput(QString("start Compile %1").arg(_p->sourceDir));
    generateClassFile();
}

void javaCompile::finishCompile(int exitcode, QProcess::ExitStatus exitStatus)
{
    if(exitStatus == QProcess::NormalExit)
    {
        if(0 == _p->currentState)
        {//进行第二步，将.class文件编译成.ass文件
            emit CompileOutput(QString("class file generate finish:").arg(_p->tempDir));
            emit CompileOutput(QString("start generate .ass file"));
            generateAssFile();
        }
        else if(1 == _p->currentState)
        {//进行第三步，将.ass文件编译成.out和.meta.json文件
            generateOutFile();
        }
        else if(2 == _p->currentState)
        {//生成gpc文件
            emit CompileOutput(QString("compile finish,open %1").arg(_p->dstFilePath));
            emit finishCompileFile(_p->dstFilePath);
            generateContractFile();
        }
    }
    else
    {
        emit CompileOutput(QString("compile error:stage %1").arg(_p->currentState));
    }
}

void javaCompile::onReadStandardOutput()
{
    emit CompileOutput(QString::fromLocal8Bit(getCompileProcess()->readAll()));
}

void javaCompile::onReadStandardError()
{
    emit CompileOutput(QString::fromLocal8Bit(getCompileProcess()->readAllStandardError()));
}

void javaCompile::generateClassFile()
{
    _p->currentState = 0;

    QStringList fileList;
    IDEUtil::GetAllFile(_p->sourceDir,fileList);
    //调用命令行编译
    QStringList params;
    params<<"-classpath"<<QCoreApplication::applicationDirPath()+"/compile/gjavac-core-1.0.1-dev.jar"<<"-d"<<_p->tempDir<<"-source"<<"8"<<"-encoding"<<"utf-8"<<fileList;

    foreach (QString is, fileList) {
        qDebug()<<is;
    }
    foreach (QString is, params) {
        qDebug()<<is;
    }
    qDebug()<<_p->sourceDir;

    _p->currentState = 0;
    getCompileProcess()->start("javac",params);
}

void javaCompile::generateAssFile()
{
    _p->currentState = 1;

    QString firstStr = "\""+_p->tempDir+";"+QCoreApplication::applicationDirPath()+"/compile/gjavac-compiler-1.0.1-dev-jar-with-dependencies.jar"+"\"";
    QStringList fileList;
    IDEUtil::GetAllFile(_p->tempDir,fileList);
    //去掉后缀
    for(int i = 0;i < fileList.count();++i)
    {
        fileList[i].remove(QRegExp("."+QFileInfo(fileList[i]).suffix()+"$"));
    }
    //调用命令行编译
    QStringList params;
    params<<"-cp"<<firstStr<<"gjavac.MainKt"<<fileList;

    foreach (QString is, params) {
        qDebug()<<is;
    }

    _p->currentState = 1;
    getCompileProcess()->start("java",params);
}

void javaCompile::generateOutFile()
{

}

void javaCompile::generateContractFile()
{

}
