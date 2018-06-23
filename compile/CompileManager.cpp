#include "CompileManager.h"

#include "gluaCompile.h"
#include "javaCompile.h"
#include "csharpCompile.h"
#include "DataDefine.h"

class CompileManager::DataPrivate
{
public:
    DataPrivate()
//        :gluaCompiler(new gluaCompile())
//        ,javaCompiler(new javaCompile())
    {

    }
    ~DataPrivate()
    {
//        delete gluaCompiler;
//        gluaCompiler = nullptr;
//        delete javaCompiler;
//        javaCompiler = nullptr;
    }

public:
//    gluaCompile *gluaCompiler;
//    javaCompile *javaCompiler;
};

CompileManager::CompileManager(QObject *parent) : QObject(parent)
  ,_p(new DataPrivate())
{
    InitManager();
}

CompileManager::~CompileManager()
{
    delete _p;
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
//    connect(_p->gluaCompiler,&BaseCompile::finishCompileFile,this,&CompileManager::finishCompile);
//    connect(_p->gluaCompiler,&BaseCompile::CompileOutput,this,&CompileManager::CompileOutput);

//    connect(_p->javaCompiler,&BaseCompile::finishCompileFile,this,&CompileManager::finishCompile);
//    connect(_p->javaCompiler,&BaseCompile::CompileOutput,this,&CompileManager::CompileOutput);
}
