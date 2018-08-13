#ifndef COMPILEMANAGER_H
#define COMPILEMANAGER_H

#include <QObject>
#include <QString>
//编译器解决方案，外部调用编译器进行编译时，能且只能访问本类接口（对各类型编译器封装，隔离）
class CompileManager : public QObject
{
    Q_OBJECT
public:
    explicit CompileManager(QObject *parent = 0);
    ~CompileManager();
public:
    void startCompile(const QString &filePath);
signals:
    void finishCompile(const QString &dstPath);
    void CompileOutput(const QString &outString);
private:
    void InitManager();
private:
    class DataPrivate;
    DataPrivate *_p;
};

#endif // COMPILEMANAGER_H
