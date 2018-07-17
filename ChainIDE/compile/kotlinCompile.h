#ifndef KOTLINCOMPILE_H
#define KOTLINCOMPILE_H

#include "BaseCompile.h"

#include <QProcess>
class kotlinCompile : public BaseCompile
{
    Q_OBJECT
public:
    explicit kotlinCompile(QObject *parent = 0);
    virtual ~kotlinCompile();
public:
    void startCompileFile(const QString &sourceFilePath);
protected slots:
    virtual void finishCompile(int exitcode,QProcess::ExitStatus exitStatus);
    virtual void onReadStandardOutput();
    virtual void onReadStandardError();
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
