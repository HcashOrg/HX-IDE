#include "csharpCompile.h"

class csharpCompile::DataPrivate
{
public:
    DataPrivate()
    {

    }
public:
};

csharpCompile::csharpCompile(QObject *parent)
    :BaseCompile(parent)
    ,_p(new DataPrivate())
{

}

csharpCompile::~csharpCompile()
{
    delete _p;
}

void csharpCompile::startCompileFile(const QString &sourceFilePath)
{

}

void csharpCompile::finishCompile(int exitcode, QProcess::ExitStatus exitStatus)
{

}

void csharpCompile::onReadStandardOutput()
{

}

void csharpCompile::onReadStandardError()
{

}
