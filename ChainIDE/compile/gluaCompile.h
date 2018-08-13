#ifndef GLUACOMPILE_H
#define GLUACOMPILE_H

#include "BaseCompile.h"
//glua编译器
class gluaCompile : public BaseCompile
{
    Q_OBJECT
public:
    explicit gluaCompile(QObject *parent = 0);
    virtual ~gluaCompile();
public:
    void startCompileFile(const QString &sourceFilePath);
protected slots:
    virtual void finishCompile(int exitcode,QProcess::ExitStatus exitStatus);
    virtual void onReadStandardOutput();
    virtual void onReadStandardError();
private:
    void initConfig(const QString &sourceFilePath);
private:
    class DataPrivate;
    DataPrivate *_p;
};

#endif // GLUACOMPILE_H
