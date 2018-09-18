#ifndef BACKSTAGEBASE_H
#define BACKSTAGEBASE_H

#include <QObject>

class QProcess;
class BackStageBase : public QObject
{
    Q_OBJECT
public:
    explicit BackStageBase(QObject *parent = 0);
    virtual ~BackStageBase();
public:
    virtual void startExe(const QString &appDataPath = "") = 0;
    virtual bool exeRunning() = 0;
    virtual QProcess *getProcess()const = 0;
    virtual void ReadyClose() = 0;
signals:
    void exeStarted();
    void exeClosed();
    void rpcReceived(const QString &,const QString &);

    void AdditionalOutputMessage(const QString &);
public slots:
    virtual void rpcPostedSlot(const QString &,const QString &) = 0;
protected slots:
    virtual void rpcReceivedSlot(const QString &id,const QString &message) = 0;
};

#endif // BACKSTAGEBASE_H
