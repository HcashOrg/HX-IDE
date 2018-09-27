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
    {

    }
    ~DataPrivate()
    {
    }

public:
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

void kotlinCompile::initConfig(const QString &sourceFilePath)
{
    setTempDir( QCoreApplication::applicationDirPath()+QDir::separator()+
                DataDefine::KOTLIN_COMPILE_TEMP_DIR + QDir::separator() + QFileInfo(sourceFilePath).baseName()
               );

    setSourceDir( IDEUtil::getNextDir(QCoreApplication::applicationDirPath()+QDir::separator()+DataDefine::KOTLIN_DIR,
                                        sourceFilePath)
                 );


    //清空临时目录
    IDEUtil::deleteDir(getTempDir());

    readyBuild();

}

void kotlinCompile::startCompileFile(const QString &sourceFilePath)
{
    initConfig(sourceFilePath);

    //设置控制台路径为当前路径
    getCompileProcess()->setWorkingDirectory(getTempDir());

    emit CompileOutput(QString("start Compile %1").arg(getSourceDir()));
    generateClassFile();

}

void kotlinCompile::finishCompile(int exitcode, QProcess::ExitStatus exitStatus)
{
    if(QProcess::NormalExit != exitStatus)
    {
        //删除之前的文件
        QFile::remove(getDstByteFilePath());
        QFile::remove(getDstMetaFilePath());
        QFile::remove(getDstOutFilePath());

        //删除临时目录
        IDEUtil::deleteDir(getTempDir());

        emit CompileOutput(QString("compile error:stage %1").arg(getCompileStage()));
        emit errorCompileFile(getSourceDir());
        return;
    }
    switch (getCompileStage()) {
    case BaseCompile::StageOne:
        //进行第二步，将.class文件编译成.ass文件
        emit CompileOutput(QString("class file generate finish:%1").arg(getTempDir()));
        emit CompileOutput(QString("start generate .ass file"));
        generateAssFile();
        break;
    case BaseCompile::StageTwo:
        //进行第三步，将.ass文件编译成.out和.meta.json文件
        emit CompileOutput(QString(".ass file generate finish."));
        emit CompileOutput(QString("start generate .out .meta.json file"));
        generateOutFile();
        break;
    case BaseCompile::StageThree:
        //生成gpc文件
        emit CompileOutput(QString(".out file generate finish."));
        emit CompileOutput(QString("start generate .gpc file"));
        generateContractFile();
        break;
    case BaseCompile::StageFour:
        //复制gpc meta.json文件到源目录
        QFile::copy(getTempDir()+"/result.gpc",getDstByteFilePath());
        QFile::copy(getTempDir()+"/result.meta.json",getDstMetaFilePath());
        QFile::copy(getTempDir()+"/result.out",getDstOutFilePath());

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
    setCompileStage(BaseCompile::StageOne);

    QStringList fileList;
    IDEUtil::GetAllFile(getSourceDir(),fileList,QStringList()<<DataDefine::KOTLIN_SUFFIX);
    //调用命令行编译
    QStringList params;
    params<<"-cp"<<QCoreApplication::applicationDirPath()+QDir::separator()+DataDefine::JAVA_CORE_PATH
          <<fileList<<"-d"<<getTempDir();

    qDebug()<<"kotlin-compile-generate-.class: "<<DataDefine::KOTLIN_COMPILE_PATH<<params;

    getCompileProcess()->start(QCoreApplication::applicationDirPath()+QDir::separator()+DataDefine::KOTLIN_COMPILE_PATH,params);
}

void kotlinCompile::generateAssFile()
{
    setCompileStage(BaseCompile::StageTwo);

    QString firstStr = ""+getTempDir()+";"+QCoreApplication::applicationDirPath()+QDir::separator()+DataDefine::JAVA_COMPILE_PATH;
    QStringList fileList;
    IDEUtil::GetAllFile(getTempDir(),fileList);
    //去掉后缀
    for(int i = 0;i < fileList.count();++i)
    {
        fileList[i].remove(QRegExp("."+QFileInfo(fileList[i]).suffix()+"$"));
    }
    //调用命令行编译
    QStringList params;
    params<<"-cp"<<firstStr<<"gjavac.MainKt"<<"-o"<<getTempDir()<<fileList;

    qDebug()<<"kotlin-compile-generate-.ass: java"<<params;

    getCompileProcess()->start("java",params);
}

void kotlinCompile::generateOutFile()
{
    setCompileStage(BaseCompile::StageThree);
    //将result.ass编译为.out文件
    qDebug()<<"kotlin-compile-generate-.out+.meta.json:"<<DataDefine::JAVA_UVM_ASS_PATH<<getTempDir()+"/result.ass";
    getCompileProcess()->start(QCoreApplication::applicationDirPath()+QDir::separator()+DataDefine::JAVA_UVM_ASS_PATH,QStringList()<<getTempDir()+"/result.ass");
}

void kotlinCompile::generateContractFile()
{
    setCompileStage(BaseCompile::StageFour);
    //将.out .meta.json文件编译为gpc文件
    qDebug()<<"kotlin-compile-generate-.gpc: "<<DataDefine::JAVA_PACKAGE_GPC_PATH<<getTempDir()+"/result.out"<<getTempDir()+"/result.meta.json";
    getCompileProcess()->start(QCoreApplication::applicationDirPath()+QDir::separator()+DataDefine::JAVA_PACKAGE_GPC_PATH,
                               QStringList()<<getTempDir()+"/result.out"<<getTempDir()+"/result.meta.json");

}

