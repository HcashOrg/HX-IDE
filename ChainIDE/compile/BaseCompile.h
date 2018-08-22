#ifndef BASECOMPILE_H
#define BASECOMPILE_H

#include <QString>
#include <QProcess>
//基类编译器
class BaseCompile : public QObject
{
    Q_OBJECT
public:
    enum CompileStage{
        StageBegin = 0,
        StageOne = 1 << 0,
        StageTwo = 1 << 1,
        StageThree = 1 << 2,
        StageFour = 1 << 3,
        StageFive = 1 << 4,
        StageSix = 1 << 5,
        StageSeven = 1 << 6,
        StageEight = 1 << 7,
        StageNine = 1 << 8,
        StageTen = 1 << 9,
    };
public:
    explicit BaseCompile(QObject *parent = 0);
    virtual ~BaseCompile();
signals:
    void finishCompileFile(const QString &dstFilePath);
    void errorCompileFile(const QString &sourceFilePath);
    void CompileOutput(const QString &);
public:
    virtual void startCompileFile(const QString &sourceFilePath) = 0;

protected slots:
    virtual void finishCompile(int exitcode,QProcess::ExitStatus exitStatus) = 0;
    virtual void onReadStandardOutput() = 0;
    virtual void onReadStandardError() = 0;
protected:
    QProcess *getCompileProcess()const;

    void setTempDir(const QString &dir);
    const QString &getTempDir()const;

    void setSourceDir(const QString &dir);
    const QString &getSourceDir()const;

    const QString &getDstByteFilePath()const;

    const QString &getDstMetaFilePath()const;

    void setCompileStage(BaseCompile::CompileStage sta);
    BaseCompile::CompileStage getCompileStage()const;
protected:
    void readyBuild();//根据临时目录、源目录，设置目标文件，清除记录，构建临时目录等
private:
    void setDstByteFilePath(const QString &path);//（.gpc）
    void setDstMetaFilePath(const QString &path);// .meta.json
private:
    class DataPrivate;
    DataPrivate *_p;
};

#endif // BASECOMPILE_H
