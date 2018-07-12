#include "csharpCompile.h"

#include <QStringList>
#include <QFileInfo>
#include <QDir>
#include <QCoreApplication>
#include <QDebug>

#include "IDEUtil.h"
#include "DataDefine.h"

class csharpCompile::DataPrivate
{
public:
    DataPrivate()
        :currentState(0)
    {

    }
public:
    QString dstFilePath;
    QString tempDir;
    QString sourceDir;

    int currentState;
};

csharpCompile::csharpCompile(QObject *parent)
    :BaseCompile(parent)
    ,_p(new DataPrivate())
{

}

csharpCompile::~csharpCompile()
{
    delete _p;
    _p = nullptr;
}

void csharpCompile::startCompileFile(const QString &sourceFilePath)
{
    _p->tempDir = QCoreApplication::applicationDirPath()+QDir::separator()+
                  DataDefine::CSHARP_COMPILE_TEMP_DIR + QDir::separator() + QFileInfo(sourceFilePath).baseName();
    _p->sourceDir = IDEUtil::getNextDir(QCoreApplication::applicationDirPath()+QDir::separator()+DataDefine::CSHARP_DIR,
                                        sourceFilePath);;

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
    generateDllFile();
}

void csharpCompile::finishCompile(int exitcode, QProcess::ExitStatus exitStatus)
{
    if(exitStatus == QProcess::NormalExit)
    {
        if(0 == _p->currentState)
        {//生成gpc文件
            emit CompileOutput(QString(".dll file generate finish."));
            generateContractFile();
        }
        else if(1 == _p->currentState)
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

void csharpCompile::onReadStandardOutput()
{
    emit CompileOutput(QString::fromLocal8Bit(getCompileProcess()->readAll()));
}

void csharpCompile::onReadStandardError()
{
    emit CompileOutput(QString::fromLocal8Bit(getCompileProcess()->readAllStandardError()));
}

void csharpCompile::generateDllFile()
{
    _p->currentState = 0;

    QStringList fileList;
    IDEUtil::GetAllFile(_p->sourceDir,fileList,QStringList()<<DataDefine::CSHARP_SUFFIX);
    //将这些文件编译成dll文件
    QStringList params;
    params<<"/target:library"<<"-reference:"+QCoreApplication::applicationDirPath()+QDir::separator()+DataDefine::CSHARP_JSON_DLL_PATH
            +","+QCoreApplication::applicationDirPath()+QDir::separator()+DataDefine::CSHARP_CORE_DLL_PATH+""
          <<fileList<<"-out:"+_p->tempDir+QDir::separator()+"result.dll"
          <<"-debug"<<"-pdb:"+_p->tempDir+QDir::separator()+"result.pdb";

    qDebug()<<"c#-compile  "<<params;

    //获取C#环境变量
    QString cscVal = QProcessEnvironment::systemEnvironment().value(DataDefine::CSHARP_COMPILER_EXE_ENV);
    if(!cscVal.isEmpty())
    {
        emit CompileOutput("start compiler path:"+cscVal+"/csc.exe\n");
        getCompileProcess()->start(cscVal+"/csc.exe",params);
    }
    else
    {
        emit CompileOutput("Error :can't find CSC environment,please set csc.exe to CSC!");
    }
}

void csharpCompile::generateContractFile()
{
    _p->currentState = 1;
    //将result编译成.pgc文件，需要先将工作目录指定为uvm_ass.exe所在目录
    getCompileProcess()->setWorkingDirectory(QCoreApplication::applicationDirPath()+QDir::separator()+DataDefine::CSHARP_COMPILE_DIR);
    QStringList params;
    params<<"--gpc"<<_p->tempDir+QDir::separator()+"result.dll";

    getCompileProcess()->start(DataDefine::CSHARP_COMPILE_PATH,params);

}
