#include "javaCompile.h"

#include <QStringList>
#include <QFileInfo>
#include <QDir>
#include <QCoreApplication>
#include <QDebug>

#include "IDEUtil.h"
#include "DataDefine.h"


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

void javaCompile::initConfig(const QString &sourceFilePath)
{
    _p->tempDir = QCoreApplication::applicationDirPath()+QDir::separator()+
                  DataDefine::JAVA_COMPILE_TEMP_DIR + QDir::separator() + QFileInfo(sourceFilePath).baseName();

    _p->sourceDir = IDEUtil::getNextDir(QCoreApplication::applicationDirPath()+QDir::separator()+DataDefine::JAVA_DIR,
                                        sourceFilePath);;

    _p->dstFilePath = _p->sourceDir+"/"+QFileInfo(_p->sourceDir).fileName();

    IDEUtil::deleteDir(_p->tempDir);
    QDir dir(_p->tempDir);
    if(!dir.exists())
    {
        dir.mkpath(dir.path());
    }

    //删除之前的文件
    QFile::remove(_p->dstFilePath+".gpc");
    QFile::remove(_p->dstFilePath+".meta.json");


}

void javaCompile::startCompileFile(const QString &sourceFilePath)
{
    initConfig(sourceFilePath);

    //设置控制台路径为当前路径
    getCompileProcess()->setWorkingDirectory(_p->tempDir);

    emit CompileOutput(QString("start Compile %1").arg(_p->sourceDir));
    generateClassFile();
}

void javaCompile::finishCompile(int exitcode, QProcess::ExitStatus exitStatus)
{
    if(exitStatus == QProcess::NormalExit)
    {
        if(0 == _p->currentState)
        {//进行第二步，将.class文件编译成.ass文件
            emit CompileOutput(QString("class file generate finish:%1").arg(_p->tempDir));
            emit CompileOutput(QString("start generate .ass file"));
            generateAssFile();
        }
        else if(1 == _p->currentState)
        {//进行第三步，将.ass文件编译成.out和.meta.json文件
            emit CompileOutput(QString(".ass file generate finish."));
            emit CompileOutput(QString("start generate .out .meta.json file"));

            generateOutFile();
        }
        else if(2 == _p->currentState)
        {//生成gpc文件
            emit CompileOutput(QString(".out file generate finish."));
            emit CompileOutput(QString("start generate .gpc file"));

            generateContractFile();
        }
        else if(3 == _p->currentState)
        {
            //复制gpc meta.json文件到源目录
            QFile::copy(_p->tempDir+"/result.gpc",_p->dstFilePath+".gpc");
            QFile::copy(_p->tempDir+"/result.meta.json",_p->dstFilePath+".meta.json");

            //删除临时目录
            IDEUtil::deleteDir(_p->tempDir);

            if(QFile(_p->dstFilePath+".gpc").exists())
            {
                emit CompileOutput(QString("compile finish,see %1").arg(_p->dstFilePath));
                emit finishCompileFile(_p->sourceDir);
            }
        }
    }
    else
    {
        emit CompileOutput(QString("compile error:stage %1").arg(_p->currentState));

        //删除之前的文件
        QFile::remove(_p->dstFilePath+".gpc");
        QFile::remove(_p->dstFilePath+".meta.json");

        //删除临时目录
        IDEUtil::deleteDir(_p->tempDir);
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
    IDEUtil::GetAllFile(_p->sourceDir,fileList,QStringList()<<DataDefine::JAVA_SUFFIX);
    //调用命令行编译
    QStringList params;
    params<<"-classpath"<<QCoreApplication::applicationDirPath()+QDir::separator()+DataDefine::JAVA_CORE_PATH
          <<"-d"<<_p->tempDir<<"-encoding"<<"utf-8"<<fileList;

    qDebug()<<"java-compile "<<params;

    getCompileProcess()->start("javac",params);
}

void javaCompile::generateAssFile()
{
    _p->currentState = 1;

    QString firstStr = ""+_p->tempDir+";"+QCoreApplication::applicationDirPath()+QDir::separator()+DataDefine::JAVA_COMPILE_PATH;
    QStringList fileList;
    IDEUtil::GetAllFile(_p->tempDir,fileList);
    //去掉后缀
    for(int i = 0;i < fileList.count();++i)
    {
        fileList[i].remove(QRegExp("."+QFileInfo(fileList[i]).suffix()+"$"));
    }
    //调用命令行编译
    QStringList params;
    params<<"-cp"<<firstStr<<"gjavac.MainKt"<<"-o"<<_p->tempDir<<fileList;

    foreach (QString is, params) {
        qDebug()<<is;
    }

    getCompileProcess()->start("java",params);
}

void javaCompile::generateOutFile()
{
    _p->currentState = 2;
    //将result.ass编译为.out文件
    getCompileProcess()->start(QCoreApplication::applicationDirPath()+QDir::separator()+DataDefine::JAVA_UVM_ASS_PATH,QStringList()<<_p->tempDir+"/result.ass");
}

void javaCompile::generateContractFile()
{
    _p->currentState = 3;
    //将.out .meta.json文件编译为gpc文件
    getCompileProcess()->start(QCoreApplication::applicationDirPath()+QDir::separator()+DataDefine::JAVA_PACKAGE_GPC_PATH,
                               QStringList()<<_p->tempDir+"/result.out"<<_p->tempDir+"/result.meta.json");

}

