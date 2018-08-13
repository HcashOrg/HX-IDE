#ifndef BACKSTAGEMANAGER_H
#define BACKSTAGEMANAGER_H

#include "DataDefine.h"
#include <QObject>
//后台管理类
class BackStageManager : public QObject
{
    Q_OBJECT
public:
    explicit BackStageManager(DataDefine::BlockChainClass chainClass, DataDefine::ChainTypes startType,const QString &dataPath,QObject *parent = nullptr);
    ~BackStageManager();
signals:
    void jsonDataUpdated(const QString &id,const QString &data);//接收到返回
    void rpcPosted(const QString & rpcId, const QString & rpcCmd);//测试链发出请求
    void rpcPostedFormal(const QString & rpcId, const QString & rpcCmd);//正式链发出请求
    void startBackStageFinish();//后台启动完成
    void closeBackStageFinish();//后台关闭完成
public:
    //数据发送
    void postRPC(QString _rpcId, QString _rpcCmd, DataDefine::ChainType chainType);
    //启动后台
    void startBackStage();
    //关闭后台，需要时间，所以单独实现
    void closeBackStage();
private:
    void InitBackStage(DataDefine::BlockChainClass chainClass,DataDefine::ChainTypes startType);
private slots:
    void exeStartedSlots();
private:
    class DataPrivate;
    DataPrivate *_p;
};

#endif // BACKSTAGEMANAGER_H
