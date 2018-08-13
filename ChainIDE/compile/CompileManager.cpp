#include "CompileManager.h"

#include "gluaCompile.h"
#include "javaCompile.h"
#include "csharpCompile.h"
#include "kotlinCompile.h"

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
        compiler = new gluaCompile(this);
    }
    else if(filePath.endsWith("."+DataDefine::JAVA_SUFFIX))
    {//调用java编译器
        compiler = new javaCompile(this);
    }
    else if(filePath.endsWith("."+DataDefine::CSHARP_SUFFIX))
    {
        compiler = new csharpCompile(this);
    }
    else if(filePath.endsWith("."+DataDefine::KOTLIN_SUFFIX))
    {
        compiler = new kotlinCompile(this);
    }

    if(compiler)
    {
        connect(compiler,&BaseCompile::finishCompileFile,this,&CompileManager::finishCompile);
        connect(compiler,&BaseCompile::CompileOutput,this,&CompileManager::CompileOutput);

        compiler->startCompileFile(filePath);
    }
}

void CompileManager::InitManager()
{
}
