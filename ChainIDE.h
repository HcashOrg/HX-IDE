#ifndef CHAINIDE_H
#define CHAINIDE_H

#include <QObject>
#include "DataDefine.h"

class ExeManager;
class CompileManager;
class QProcess;
class ChainIDE : public QObject
{
    Q_OBJECT
public:
//数据发送接收
    void updateJsonDataMap(QString id, QString data);
    QString jsonDataValue(QString id);
    void postRPC(QString _rpcId, QString _rpcCmd);
//链类型
    int getCurrentChainType()const;
    void setCurrentChainType(int type);
//配置
    QString getEnvAppDataPath()const;
    QString getConfigAppDataPath()const;
    DataDefine::ThemeStyle getCurrentTheme()const;
    void setCurrentTheme(DataDefine::ThemeStyle)const;

    void setConfigAppDataPath(const QString &path);
//后台
    ExeManager *testManager()const;
    ExeManager *formalManager()const;
    QProcess *getProcess(int type = 0)const;//0当前 1测试  2正式
//编译
    CompileManager *getCompileManager()const;
public:
    void refreshStyleSheet();
private:
    void getSystemEnvironmentPath();
    void InitConfig();//初始化配置
    void InitExeManager();//初始化后台
signals:
    void jsonDataUpdated(QString);
    void rpcPosted(QString rpcId, QString rpcCmd);
    void rpcPostedFormal(QString rpcId, QString rpcCmd);
public:
    static ChainIDE *getInstance();
    ~ChainIDE();
private:
    explicit ChainIDE(QObject *parent = 0);
    static ChainIDE *_instance;
private:
    class DataPrivate;
    DataPrivate *_p;
};

#endif // CHAINIDE_H
