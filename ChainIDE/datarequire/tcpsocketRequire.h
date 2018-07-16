#ifndef TCPSOCKETREQUIRE_H
#define TCPSOCKETREQUIRE_H


#include "RequireBase.h"
#include <QAbstractSocket>

class tcpsocketRequire : public RequireBase
{
    Q_OBJECT
public:
    explicit tcpsocketRequire(const QString &ip,const QString & port,QObject *parent = 0);
    virtual ~tcpsocketRequire();
public:
    virtual void postData(const QString &data) ;
    virtual void startConnect() ;
private:
    void Init();
    bool isConnected();
private:
    class DataPrivate;
    DataPrivate *_p;
};

#endif // TCPSOCKETREQUIRE_H
