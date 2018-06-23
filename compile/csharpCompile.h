#ifndef CSHARPCOMPILE_H
#define CSHARPCOMPILE_H

#include "BaseCompile.h"

class csharpCompile : public BaseCompile
{
public:
    explicit csharpCompile(QObject *parent = 0);
    ~csharpCompile();
public:
    void startCompileFile(const QString &sourceFilePath);
protected slots:
    virtual void finishCompile(int exitcode,QProcess::ExitStatus exitStatus);
    virtual void onReadStandardOutput();
    virtual void onReadStandardError();
private:
    void generateDllFile();
    void generateContractFile();
private:
    class DataPrivate;
    DataPrivate *_p;
};

#endif // CSHARPCOMPILE_H
