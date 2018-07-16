#ifndef WEBSOCKETREQUIRE_H
#define WEBSOCKETREQUIRE_H

#include "RequireBase.h"
#include <QAbstractSocket>

class websocketRequire : public RequireBase
{
    Q_OBJECT
public:
    explicit websocketRequire(const QString &ip,const QString & connectPort,QObject *parent = 0);
    ~websocketRequire();
public:
    virtual void postData(const QString &data) ;
    virtual void startConnect() ;

private slots:
    void onTextFrameReceived(QString,bool);
    void onStateChanged(QAbstractSocket::SocketState);
private:
    void Init();
    bool isConnected();
private:
    class DataPrivate;
    DataPrivate *_p;
};

#endif // WEBSOCKETREQUIRE_H
