#include "httpRequire.h"

#include <QNetworkRequest>
#include <QNetworkReply>
#include <QNetworkAccessManager>
class httpRequire::DataPrivate
{
public:
    DataPrivate()
        :networkAccessManager(new QNetworkAccessManager())
    {

    }
    ~DataPrivate()
    {
        delete networkAccessManager;
        networkAccessManager = nullptr;
    }

public:
    QNetworkRequest httpRequest;
    QNetworkAccessManager   *networkAccessManager;
};

httpRequire::httpRequire(const QString &ip,const QString & connectPort,QObject *parent)
    : RequireBase(ip,connectPort,parent)
    ,_p(new DataPrivate())
{
    Init();
}

httpRequire::~httpRequire()
{

}

void httpRequire::postData(const QString &data)
{
    _p->networkAccessManager->post(_p->httpRequest, data.toUtf8());
}

void httpRequire::startConnect()
{
    _p->httpRequest.setRawHeader("Content-Type","application/json");
    _p->httpRequest.setUrl(QUrl(QString("http://%1:%2").arg(getConnectIP()).arg(getConnectPort())));
    emit connectFinish();
}

void httpRequire::requestFinished(QNetworkReply *reply)
{
    if(reply->error() != QNetworkReply::NoError) return;

    emit receiveData(reply->readAll());

    reply->deleteLater();
}

void httpRequire::Init()
{
    connect(_p->networkAccessManager, &QNetworkAccessManager::finished, this, &httpRequire::requestFinished); //关联信号和槽
}
