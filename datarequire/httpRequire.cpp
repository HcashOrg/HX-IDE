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
    _p->httpRequest.setHeader(QNetworkRequest::ContentTypeHeader,"application/json");
    _p->httpRequest.setRawHeader("Authorization","Basic YTpi");
    _p->httpRequest.setUrl(QUrl(QString("http://%1:%2").arg(getConnectIP()).arg(getConnectPort())));
    emit connectFinish();
}

void httpRequire::requestFinished(QNetworkReply *reply)
{
    QJsonParseError json_error;
    QJsonDocument parse_doucment = QJsonDocument::fromJson(reply->readAll(),&json_error);
    if(json_error.error != QJsonParseError::NoError || !parse_doucment.isObject())
    {
         emit receiveData("Error "+json_error.errorString());
         return;
    }
    else if(reply->error() != QNetworkReply::NoError)
    {
         emit receiveData("Error "+parse_doucment.object().value("error").toObject().value("message").toString());
         return;
    }
    QJsonValue val = parse_doucment.object().value("result");
    if(val.isArray())
    {
        emit receiveData(QJsonDocument(val.toArray()).toJson());
    }
    else if(val.isObject())
    {
        emit receiveData(QJsonDocument(val.toObject()).toJson());
    }
    else if(val.isString())
    {
        emit receiveData(val.toString());
    }
    else if(val.isDouble())
    {
        emit receiveData(QString::number(val.toDouble()));
    }
    else if(val.isNull())
    {
        emit receiveData("");
    }

    reply->deleteLater();
}

void httpRequire::Init()
{
    connect(_p->networkAccessManager, &QNetworkAccessManager::finished, this, &httpRequire::requestFinished); //关联信号和槽
}
