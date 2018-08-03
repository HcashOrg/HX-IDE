#ifndef BACKSTAGEBASE_H
#define BACKSTAGEBASE_H

#include <QObject>
#include <QProcess>

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
public slots:
    virtual void rpcPostedSlot(const QString &,const QString &) = 0;
};

#endif // BACKSTAGEBASE_H
