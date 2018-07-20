#include "httpRequire.h"

#include <QNetworkRequest>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

class httpRequire::DataPrivate
{
public:
    DataPrivate()
        :networkAccessManager(new QNetworkAccessManager())
    {

    }
    ~DataPrivate()
    {
        if(networkAccessManager)
        {
            delete networkAccessManager;
            networkAccessManager = nullptr;
        }
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
    delete _p;
    _p = nullptr;
}

void httpRequire::postData(const QString &data)
{
    _p->networkAccessManager->post(_p->httpRequest, data.toUtf8());
}

void httpRequire::startConnect()
{
    _p->httpRequest.setHeader(QNetworkRequest::ContentTypeHeader,"application/json");
    _p->httpRequest.setUrl(QUrl(QString("http://%1:%2").arg(getConnectIP()).arg(getConnectPort())));
    emit connectFinish();
}

void httpRequire::setRawHeader(const QByteArray &headerName, const QByteArray &value)
{
    //_p->httpRequest.setRawHeader("Authorization","Basic YTpi");
    _p->httpRequest.setRawHeader(headerName,value);
}

void httpRequire::requestFinished(QNetworkReply *reply)
{
    QString res = reply->readAll();
    int index = res.indexOf(",\"error\":");
    QString result = res.mid(0,index) + "}";
    emit receiveData(result);

    reply->deleteLater();
}

void httpRequire::Init()
{
    connect(_p->networkAccessManager, &QNetworkAccessManager::finished, this, &httpRequire::requestFinished); //关联信号和槽
}
