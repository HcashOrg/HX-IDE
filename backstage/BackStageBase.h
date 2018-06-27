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
    virtual void startExe() = 0;
    virtual bool exeRunning() = 0;
    virtual QProcess *getProcess()const = 0;
    virtual void ReadyClose();
signals:
    void exeStarted();
public slots:
    virtual void rpcPostedSlot(const QString &,const QString &) = 0;
};

#endif // BACKSTAGEBASE_H
