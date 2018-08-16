#ifndef TCPSOCKETREQUIRE_H
#define TCPSOCKETREQUIRE_H


#include "RequireBase.h"
#include <QAbstractSocket>
//tcp请求
class tcpsocketRequire : public RequireBase
{
    Q_OBJECT
public:
    explicit tcpsocketRequire(const QString &ip,const QString & port,QObject *parent = 0);
    virtual ~tcpsocketRequire();
public:
    void postData(const QString &data) override final;
    void startConnect() override final;
    bool isConnected()const override final;
private slots:
    void readyReadSlots();
private:
    void Init();
private:
    class DataPrivate;
    DataPrivate *_p;
};

#endif // TCPSOCKETREQUIRE_H
