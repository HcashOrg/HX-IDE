#include "javaCompile.h"

#include <QStringList>
#include <QFileInfo>
#include <QDir>
#include <QCoreApplication>
#include <QDebug>

#include "IDEUtil.h"
#include "DataDefine.h"

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
            //删除之前的文件
            QString targetPath = _p->sourceDir+"/"+QFileInfo(_p->sourceDir).fileName();
            QFile::remove(targetPath+".gpc");
            QFile::remove(targetPath+".meta.json");

            //复制gpc meta.json文件到源目录
            QFile::copy(_p->tempDir+"/result.gpc",targetPath+".gpc");
            QFile::copy(_p->tempDir+"/result.meta.json",targetPath+".meta.json");

            //删除临时目录
            IDEUtil::deleteDir(_p->tempDir);

            _p->dstFilePath = _p->sourceDir+"/"+QFileInfo(_p->sourceDir).fileName()+".gpc";
            if(QDir(_p->dstFilePath).exists())
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
    IDEUtil::GetAllFile(_p->sourceDir,fileList,QStringList()<<"java");
    //调用命令行编译
    QStringList params;
    params<<"-classpath"<<QCoreApplication::applicationDirPath()+QDir::separator()+DataDefine::JAVA_CORE_PATH
          <<"-d"<<_p->tempDir<<"-encoding"<<"utf-8"<<fileList;

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

    _p->currentState = 1;
    getCompileProcess()->start("java",params);
}

void javaCompile::generateOutFile()
{
    _p->currentState = 2;
    //将result.ass编译为.out文件
    getCompileProcess()->start(QCoreApplication::applicationDirPath()+QDir::separator()+DataDefine::UVM_ASS_PATH,QStringList()<<_p->tempDir+"/result.ass");
}

void javaCompile::generateContractFile()
{
    _p->currentState = 3;
    //将.out .meta.json文件编译为gpc .meta.json文件
    getCompileProcess()->start(QCoreApplication::applicationDirPath()+QDir::separator()+DataDefine::PACKAGE_GPC_PATH,
                               QStringList()<<_p->tempDir+"/result.out"<<_p->tempDir+"/result.meta.json");

}
