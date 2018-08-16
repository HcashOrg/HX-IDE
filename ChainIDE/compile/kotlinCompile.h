#ifndef KOTLINCOMPILE_H
#define KOTLINCOMPILE_H

#include "BaseCompile.h"
//kotlin编译器
class kotlinCompile : public BaseCompile
{
    Q_OBJECT
public:
    explicit kotlinCompile(QObject *parent = 0);
    virtual ~kotlinCompile();
public:
    void startCompileFile(const QString &sourceFilePath)override final;
protected slots:
    void finishCompile(int exitcode,QProcess::ExitStatus exitStatus)override final;
    void onReadStandardOutput()override final;
    void onReadStandardError()override final;
private:
    void generateClassFile();
    void generateAssFile();
    void generateOutFile();
    void generateContractFile();
private:
    void initConfig(const QString &sourceFilePath);
private:
    class DataPrivate;
    DataPrivate *_p;
};

#endif // KOTLINCOMPILE_H
