#ifndef JAVACOMPILE_H
#define JAVACOMPILE_H

#include "BaseCompile.h"
//java编译器
class javaCompile : public BaseCompile
{
    Q_OBJECT
public:
    explicit javaCompile(QObject *parent = 0);
    virtual ~javaCompile();
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

#endif // JAVACOMPILE_H
