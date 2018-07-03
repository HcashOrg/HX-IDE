#include "kotlinCompile.h"

#include <QStringList>
#include <QFileInfo>
#include <QDir>
#include <QCoreApplication>
#include <QDebug>

#include "IDEUtil.h"
#include "DataDefine.h"


class kotlinCompile::DataPrivate
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

kotlinCompile::kotlinCompile(QObject *parent)
    : BaseCompile(parent)
    ,_p(new DataPrivate())
{

}

kotlinCompile::~kotlinCompile()
{
    delete _p;
    _p = nullptr;
}

void kotlinCompile::startCompileFile(const QString &sourceFilePath)
{
    _p->tempDir = QCoreApplication::applicationDirPath()+QDir::separator()+
                  DataDefine::KOTLIN_COMPILE_TEMP_DIR + QDir::separator() + QFileInfo(sourceFilePath).baseName();

    _p->sourceDir = IDEUtil::getNextDir(QCoreApplication::applicationDirPath()+QDir::separator()+DataDefine::KOTLIN_DIR,
                                        sourceFilePath);//QFileInfo(sourceFilePath).absoluteDir().absolutePath();

    _p->dstFilePath = _p->sourceDir+"/"+QFileInfo(_p->sourceDir).fileName();

    //设置控制台路径为当前路径
    getCompileProcess()->setWorkingDirectory(_p->tempDir);

    IDEUtil::deleteDir(_p->tempDir);
    QDir dir(_p->tempDir);
    if(!dir.exists())
    {
        dir.mkpath(dir.path());
    }
    emit CompileOutput(QString("start Compile %1").arg(_p->sourceDir));
    generateClassFile();

}

void kotlinCompile::finishCompile(int exitcode, QProcess::ExitStatus exitStatus)
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
            //删除之前的文件
            QFile::remove(_p->dstFilePath+".gpc");
            QFile::remove(_p->dstFilePath+".meta.json");

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
        QString targetPath = _p->sourceDir+"/"+QFileInfo(_p->sourceDir).fileName();
        QFile::remove(targetPath+".gpc");
        QFile::remove(targetPath+".meta.json");
        //删除临时目录
        IDEUtil::deleteDir(_p->tempDir);
    }

}

void kotlinCompile::onReadStandardOutput()
{
    emit CompileOutput(QString::fromLocal8Bit(getCompileProcess()->readAll()));
}

void kotlinCompile::onReadStandardError()
{
    emit CompileOutput(QString::fromLocal8Bit(getCompileProcess()->readAllStandardError()));

}

void kotlinCompile::generateClassFile()
{
    _p->currentState = 0;

    QStringList fileList;
    IDEUtil::GetAllFile(_p->sourceDir,fileList,QStringList()<<DataDefine::KOTLIN_SUFFIX);
    //调用命令行编译
    QStringList params;
    params<<"-cp"<<QCoreApplication::applicationDirPath()+QDir::separator()+DataDefine::JAVA_CORE_PATH
          <<fileList<<"-d"<<_p->tempDir;

    qDebug()<<"kotlin-compile "<<params;

    getCompileProcess()->start(QCoreApplication::applicationDirPath()+QDir::separator()+DataDefine::KOTLIN_COMPILE_PATH,params);
}

void kotlinCompile::generateAssFile()
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

void kotlinCompile::generateOutFile()
{
    _p->currentState = 2;
    //将result.ass编译为.out文件
    getCompileProcess()->start(QCoreApplication::applicationDirPath()+QDir::separator()+DataDefine::JAVA_UVM_ASS_PATH,QStringList()<<_p->tempDir+"/result.ass");
}

void kotlinCompile::generateContractFile()
{
    _p->currentState = 3;
    //将.out .meta.json文件编译为gpc文件
    getCompileProcess()->start(QCoreApplication::applicationDirPath()+QDir::separator()+DataDefine::JAVA_PACKAGE_GPC_PATH,
                               QStringList()<<_p->tempDir+"/result.out"<<_p->tempDir+"/result.meta.json");

}
