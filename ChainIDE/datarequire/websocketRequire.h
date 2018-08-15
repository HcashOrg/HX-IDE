#ifndef WEBSOCKETREQUIRE_H
#define WEBSOCKETREQUIRE_H

#include "RequireBase.h"
#include <QAbstractSocket>
//websocket请求，接收内容需要为完整json
class websocketRequire : public RequireBase
{
    Q_OBJECT
public:
    explicit websocketRequire(const QString &ip,const QString & connectPort,QObject *parent = 0);
    ~websocketRequire();
public:
    virtual void postData(const QString &data) override final;
    virtual void startConnect() override final;
    virtual bool isConnected()const override final;

private slots:
    void onTextFrameReceived(QString,bool);
    void onStateChanged(QAbstractSocket::SocketState);
private:
    void Init();
private:
    class DataPrivate;
    DataPrivate *_p;
};

#endif // WEBSOCKETREQUIRE_H
