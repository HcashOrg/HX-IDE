#ifndef TCPSOCKETREQUIRE_H
#define TCPSOCKETREQUIRE_H


#include "RequireBase.h"
#include <QAbstractSocket>
//tcp请求，接收内容需要为完整的json
class tcpsocketRequire : public RequireBase
{
    Q_OBJECT
public:
    explicit tcpsocketRequire(const QString &ip,const QString & port,QObject *parent = 0);
    virtual ~tcpsocketRequire();
public:
    virtual void postData(const QString &data) override final;
    virtual void startConnect() override final;
    virtual bool isConnected()const override final;
private:
    void Init();
private:
    class DataPrivate;
    DataPrivate *_p;
};

#endif // TCPSOCKETREQUIRE_H
