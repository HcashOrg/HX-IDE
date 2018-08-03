#ifndef LINKBACKSTAGE_H
#define LINKBACKSTAGE_H

#include "BackStageBase.h"
class LinkBackStage : public BackStageBase
{
    Q_OBJECT
public:
    explicit LinkBackStage(int type = 1,const QString &appDataPath = "",QObject *parent = 0);//链类型1==测试 2==正式
    ~LinkBackStage();

public:
    void startExe();
    bool exeRunning();
    QProcess *getProcess()const;
    void ReadyClose();
private slots:
    void onNodeExeStateChanged();
    void checkNodeExeIsReady();
    void delayedLaunchClient();
    void onClientExeStateChanged();
public slots:
    void rpcPostedSlot(const QString &,const QString &);
private:
    void initSocketManager();
private:
    class DataPrivate;
    DataPrivate *_p;
};

#endif // LINKBACKSTAGE_H
