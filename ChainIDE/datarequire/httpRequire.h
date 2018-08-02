#ifndef HTTPREQUIRE_H
#define HTTPREQUIRE_H

#include "RequireBase.h"
#include <QNetworkReply>

class httpRequire : public RequireBase
{
    Q_OBJECT
public:
    explicit httpRequire(const QString &ip,const QString & port,QObject *parent = 0);
    virtual ~httpRequire();
public:
    virtual void postData(const QString &data) ;
    virtual void startConnect() ;
    virtual bool isConnected()const ;
    void setRawHeader(const QByteArray &headerName, const QByteArray &value);
private slots:
    void requestFinished(QNetworkReply*);
private:
    void Init();
private:
    class DataPrivate;
    DataPrivate *_p;
};

#endif // HTTPREQUIRE_H
