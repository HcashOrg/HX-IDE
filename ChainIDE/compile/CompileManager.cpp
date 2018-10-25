#include "CompileManager.h"

#include <QDebug>
#include <QSettings>
#include <QRegExpValidator>
#include <QCoreApplication>
#ifdef WIN32
#include "gluaCompile.h"
#include "javaCompile.h"
#include "csharpCompile.h"
#include "kotlinCompile.h"
#else
#include "csharpCompile_unix.h"
#endif
#include "DataDefine.h"

class CompileManager::DataPrivate
{
public:
    DataPrivate()
    {

    }
    ~DataPrivate()
    {
    }
};

CompileManager::CompileManager(QObject *parent) : QObject(parent)
  ,_p(new DataPrivate())
{
    InitManager();
}

CompileManager::~CompileManager()
{
    qDebug()<<"delete compileManager";
    delete _p;
    _p = nullptr;
}

void CompileManager::startCompile(const QString &filePath)
{
    BaseCompile *compiler = nullptr;
    if(filePath.endsWith("."+DataDefine::GLUA_SUFFIX))
    {//调用glua编译器
#ifdef WIN32
        compiler = new gluaCompile(this);
#else
#endif
    }
    else if(filePath.endsWith("."+DataDefine::JAVA_SUFFIX))
    {//调用java编译器
        if(checkJavaEnvironment())
        {
#ifdef WIN32
            compiler = new javaCompile(this);
#else
#endif
        }
        else
        {
            qDebug()<<"no java";
            emit CompileOutput(tr("Please make sure you've installed jdk8 and commond 'java -version' is useful."));
            emit CompileOutput(tr("Here is the download website: https://docs.oracle.com/javase/8/docs/technotes/guides/install/install_overview.html"));
        }
    }
    else if(filePath.endsWith("."+DataDefine::CSHARP_SUFFIX))
    {
        if(checkCsharpEnvironment())
        {
#ifdef WIN32
            compiler = new csharpCompile(this);
#else
            compiler = new csharpCompile_unix(this);
#endif
        }
        else
        {
            emit CompileOutput(tr("Please make sure you've installed .net framework 4.6 or newer version."));
            emit CompileOutput(tr("Here is the download website: https://www.microsoft.com/en-us/download/details.aspx?id=48130"));
        }
    }
    else if(filePath.endsWith("."+DataDefine::KOTLIN_SUFFIX))
    {
        if(checkJavaEnvironment())
        {
#ifdef WIN32
            compiler = new kotlinCompile(this);
#else
#endif
        }
        else
        {
            qDebug()<<"no java";
            emit CompileOutput(tr("Please make sure you've installed jdk8 and commond 'java -version' is useful."));
            emit CompileOutput(tr("Here is the download website: https://docs.oracle.com/javase/8/docs/technotes/guides/install/install_overview.html"));
        }
    }

    if(compiler)
    {
        connect(compiler,&BaseCompile::finishCompileFile,this,&CompileManager::finishCompile);
        connect(compiler,&BaseCompile::errorCompileFile,this,&CompileManager::errorCompile);
        connect(compiler,&BaseCompile::CompileOutput,this,&CompileManager::CompileOutput);

        compiler->startCompileFile(filePath);
    }
    else
    {
        emit errorCompile(filePath);
    }
}

void CompileManager::InitManager()
{
}

bool CompileManager::checkJavaEnvironment()
{
    QProcess *pro = new QProcess();
    pro->start("java",QStringList()<<"-version");
    pro->waitForFinished();
    QString str(pro->readAllStandardError());
    qDebug()<<"java check version"<<str;
    return str.contains("\"1.8.");
}

bool CompileManager::checkCsharpEnvironment()
{
#ifdef WIN32
    //windows下查找注册表有没有.netframework 4.5以上
    QSettings set("HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\NET Framework Setup\\NDP",QSettings::NativeFormat);

    QRegExp versionReg("^[0-4]\.[0-5]\.");
    QStringList li = set.allKeys();
    foreach (QString key, li) {
        if(key.endsWith("/Version"))
        {
            if(set.value(key).toString().indexOf(versionReg)==-1)
            {
                //说明存在4.5以上的版本
                qDebug()<<"check .net framework version "<<key<<set.value(key).toString();
                return true;
            }
        }
    }
#else
    //mac系统下直接尝试.net
    QProcess dot;
    dot.start("/usr/local/share/dotnet/dotnet",QStringList()<<"--version");
    dot.waitForReadyRead();
    QString out = dot.readAllStandardOutput();
    QRegExp versionReg("^[0-9]\.[0-9]\.");
    if(-1 != out.indexOf(versionReg))
    {
        return true;
    }
#endif
    return false;
}
