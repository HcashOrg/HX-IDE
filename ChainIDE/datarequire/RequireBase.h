#ifndef REQUIREBASE_H
#define REQUIREBASE_H

#include <QObject>
//数据请求基类，tcp websocket都从此继承，采用单线程模式
class RequireBase : public QObject
{
    Q_OBJECT
public:
    explicit RequireBase(const QString &ip,const QString & connectPort,QObject *parent = 0);
    virtual ~RequireBase();
public:
    virtual void postData(const QString &data) = 0;
    virtual void startConnect() = 0;
    virtual bool isConnected()const = 0;
signals:
    void receiveData(const QString &);
    void connectFinish();
public:
    const QString & getConnectPort()const;
    const QString &getConnectIP()const;
private:
    class DataPrivate;
    DataPrivate *_p;

};

#endif // REQUIREBASE_H
