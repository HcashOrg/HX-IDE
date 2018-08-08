#ifndef UBTCBACKSTAGE_H
#define UBTCBACKSTAGE_H

#include "BackStageBase.h"

class UbtcBackStage : public BackStageBase
{
    Q_OBJECT
public:
    explicit UbtcBackStage(int type = 1,QObject *parent = 0);//链类型1==测试 2==正式
    ~UbtcBackStage();

public:
    void startExe(const QString &appDataPath = "")override final;
    bool exeRunning()override final;
    QProcess *getProcess()const override final;
    void ReadyClose()override final;
public slots:
    void rpcPostedSlot(const QString &,const QString &)override final;
private slots:
    void onNodeExeStateChanged();

private:
    void initSocketManager();
private:
    class DataPrivate;
    DataPrivate *_p;
};

#endif // UBTCBACKSTAGE_H
