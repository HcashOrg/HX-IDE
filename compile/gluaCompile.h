#ifndef GLUACOMPILE_H
#define GLUACOMPILE_H

#include "BaseCompile.h"
#include <QObject>
#include <QProcess>

class gluaCompile : public BaseCompile
{
    Q_OBJECT
public:
    explicit gluaCompile(QObject *parent = 0);
    virtual ~gluaCompile();
public:
    void startCompileFile(const QString &sourceFilePath);
private slots:
    void finishCompile(int exitcode,QProcess::ExitStatus exitStatus);

    void readyReadStandardOutput();
private:
    class DataPrivate;
    DataPrivate *_p;
};

#endif // GLUACOMPILE_H
