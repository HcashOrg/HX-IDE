#include "RequireBase.h"

class RequireBase::DataPrivate
{
public:
    DataPrivate(const QString &hostip,const QString & connectPort)
        :port(connectPort)
        ,ip(hostip)
    {

    }
public:
    QString port;
    QString ip;
};

RequireBase::RequireBase(const QString &hostip,const QString & connectPort,QObject *parent)
    : QObject(parent)
    ,_p(new DataPrivate(hostip,connectPort))
{

}

RequireBase::~RequireBase()
{
    delete _p;
    _p = nullptr;
}

const QString & RequireBase::getConnectPort() const
{
    return _p->port;
}

const QString &RequireBase::getConnectIP() const
{
    return _p->ip;
}

