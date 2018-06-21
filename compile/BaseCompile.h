#ifndef BASECOMPILE_H
#define BASECOMPILE_H

#include <QString>
#include <QProcess>
//基类编译器
class BaseCompile : public QObject
{
    Q_OBJECT
public:
    explicit BaseCompile(QObject *parent = 0);
    virtual ~BaseCompile();
signals:
    void finishCompileFile(const QString &dstFilePath);
    void CompileOutput(const QString &);
public:
    virtual void startCompileFile(const QString &sourceFilePath) = 0;

protected slots:
    virtual void finishCompile(int exitcode,QProcess::ExitStatus exitStatus);
    virtual void onReadStandardOutput();
    virtual void onReadStandardError();
protected:
    QProcess *getCompileProcess()const;
private:
    class DataPrivate;
    DataPrivate *_p;
};

#endif // BASECOMPILE_H
