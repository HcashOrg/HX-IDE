#include "csharpcompile_unix.h"

#include <QStringList>
#include <QFileInfo>
#include <QDir>
#include <QCoreApplication>
#include <QDebug>

#include "IDEUtil.h"
#include "DataDefine.h"

class csharpCompile_unix::DataPrivate
{
public:
    DataPrivate()
    {

    }
public:
};

csharpCompile_unix::csharpCompile_unix(QObject *parent)
    :BaseCompile(parent)
    ,_p(new DataPrivate())
{

}

csharpCompile_unix::~csharpCompile_unix()
{
    delete _p;
    _p = nullptr;
}

void csharpCompile_unix::initConfig(const QString &sourceFilePath)
{
    setTempDir( QCoreApplication::applicationDirPath()+QDir::separator()+
                DataDefine::CSHARP_COMPILE_TEMP_DIR + QDir::separator() + QFileInfo(sourceFilePath).baseName()
               );

    setSourceDir( IDEUtil::getNextDir(QCoreApplication::applicationDirPath()+QDir::separator()+DataDefine::CSHARP_DIR,
                                        sourceFilePath)
                 );

    //清空临时目录
    IDEUtil::deleteDir(getTempDir());

    readyBuild();
}

void csharpCompile_unix::startCompileFile(const QString &sourceFilePath)
{
    initConfig(sourceFilePath);

    //设置控制台路径为当前路径
    getCompileProcess()->setWorkingDirectory(getTempDir());

    emit CompileOutput(QString("start Compile %1").arg(getSourceDir()));
    generateProject();
}

void csharpCompile_unix::finishCompile(int exitcode, QProcess::ExitStatus exitStatus)
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
        emit CompileOutput(QString("generate build.project finish."));
        generateDllFile();
        break;
    case BaseCompile::StageTwo:
        emit CompileOutput(QString("generate build.dll finish."));
        generateUVMFile();
        break;
    case BaseCompile::StageThree:
        emit CompileOutput(QString("generate build.uvms finish."));
        break;

    default:
        break;
    }

}

void csharpCompile_unix::onReadStandardOutput()
{
    emit CompileOutput(QString::fromLocal8Bit(getCompileProcess()->readAll()));
}

void csharpCompile_unix::onReadStandardError()
{
    emit CompileOutput(QString::fromLocal8Bit(getCompileProcess()->readAllStandardError()));
}

void csharpCompile_unix::generateProject()
{
    setCompileStage(BaseCompile::StageOne);

    //先将源码拷贝到临时目录
    QStringList fileList;
    IDEUtil::GetAllFile(getSourceDir(),fileList,QStringList()<<DataDefine::CSHARP_SUFFIX);
    foreach (QString path, fileList) {
        QFile::copy(path,getTempDir()+"/"+QFileInfo(path).baseName()+"."+QFileInfo(path).completeSuffix());
    }
    //dotnet运行生成build目录
    QStringList params;
    params<<"new"<<"classlib"<<"-n"<<"build";
    qDebug()<<DataDefine::CSHARP_COMPILER_EXE_PATH<<params;
    getCompileProcess()->start(DataDefine::CSHARP_COMPILER_EXE_PATH,params);

}

void csharpCompile_unix::generateDllFile()
{
    setCompileStage(BaseCompile::StageTwo);
    //替换已有的build.csproj,删除class1.cs
    QString buildProj = getTempDir()+"/build/build.csproj";
    if(QFileInfo(buildProj).exists())
    {
        QFile::remove(getTempDir()+"/build/Class1.cs");
        QFile::remove(buildProj);
        QFile::copy(QCoreApplication::applicationDirPath()+"/"+DataDefine::CSHARP_PROJECT_TEMPLATE_PATH,buildProj);
    }
    else
    {
        emit CompileOutput(QString("cannot find file %1").arg("build.csproj"));
        emit errorCompileFile(getSourceDir());
        return;
    }
    //复制uvmlib到build目录，复制源码到build目录
    QFile::copy(QCoreApplication::applicationDirPath()+"/"+DataDefine::CSHARP_UVMCORE_LIB_PATH,getTempDir()+"/build/uvmcorelib.dll");

    //先将源码拷贝到build目录
    QStringList fileList;
    IDEUtil::GetAllFile(getSourceDir(),fileList,QStringList()<<DataDefine::CSHARP_SUFFIX);
    foreach (QString path, fileList) {
        QFile::copy(path,getTempDir()+"/build/"+QFileInfo(path).baseName()+"."+QFileInfo(path).completeSuffix());
    }
    //开始编译生成build.dll
    getCompileProcess()->setWorkingDirectory(getTempDir()+"/build");
    getCompileProcess()->start(DataDefine::CSHARP_COMPILER_EXE_PATH,QStringList()<<"build");
}

void csharpCompile_unix::generateUVMFile()
{
    setCompileStage(BaseCompile::StageThree);
    //找到build.dll
    QString buildDll = "";
    QStringList fileList;
    IDEUtil::GetAllFile(getTempDir()+"/build",fileList,QStringList()<<"dll");
    foreach (QString path, fileList) {
        if(QFileInfo(path).baseName()+"."+QFileInfo(path).completeSuffix() == "build.dll")
        {
            buildDll = path;
            break;
        }
    }
    if(buildDll.isEmpty())
    {
        emit CompileOutput(QString("cannot find file %1").arg("build.dll"));
        emit errorCompileFile(getSourceDir());
        return;
    }

    //编译build.dll，生成build.uvms

    QStringList params;
    params<<QCoreApplication::applicationDirPath()+"/"+DataDefine::CSHARP_GSHARPCORE_DLL_PATH
          <<"-c"<<buildDll;
    qDebug()<<DataDefine::CSHARP_COMPILER_EXE_PATH<<params;
    getCompileProcess()->start(DataDefine::CSHARP_COMPILER_EXE_PATH,params);

}

void csharpCompile_unix::generateContractFile()
{

}

