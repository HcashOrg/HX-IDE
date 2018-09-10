#ifndef CTCBACKSTAGE_H
#define CTCBACKSTAGE_H

#include "BackStageBase.h"

class CTCBackStage : public BackStageBase
{
    Q_OBJECT
public:
    explicit CTCBackStage(int type = 1,QObject *parent = 0);
    ~CTCBackStage();
public:
    void startExe(const QString &appDataPath = "")override final;
    bool exeRunning()override final;
    QProcess *getProcess()const override final;
    void ReadyClose()override final;
public slots:
    void rpcPostedSlot(const QString &,const QString &)override final;
protected slots:
    void rpcReceivedSlot(const QString &id,const QString &message)override final;
private slots:
    void onNodeExeStateChanged();
    void testStartedFinish();
    void testStartReceiveSlot(const QString &id,const QString &message);

private:
    void initSocketManager();
private:
    class DataPrivate;
    DataPrivate *_p;
};

#endif // CTCBACKSTAGE_H
