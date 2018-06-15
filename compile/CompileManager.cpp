#include "CompileManager.h"

#include "gluaCompile.h"
class CompileManager::DataPrivate
{
public:
    DataPrivate()
        :gluaCompiler(new gluaCompile())
    {

    }
    ~DataPrivate()
    {
        delete gluaCompiler;
        gluaCompiler = nullptr;
    }

public:
    gluaCompile *gluaCompiler;
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
    if(filePath.endsWith(".glua"))
    {//调用glua编译器
        if(_p->gluaCompiler)
        {
            _p->gluaCompiler->startCompileFile(filePath);
        }
    }
    else if(filePath.endsWith(".cs"))
    {

    }
}

void CompileManager::InitManager()
{
    connect(_p->gluaCompiler,&BaseCompile::finishCompileFile,this,&CompileManager::finishCompile);
    connect(_p->gluaCompiler,&BaseCompile::CompileOutput,this,&CompileManager::CompileOutput);
}
