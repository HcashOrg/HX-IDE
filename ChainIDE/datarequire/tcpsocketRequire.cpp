#include "tcpsocketRequire.h"

#include <QHostAddress>
#include <QTcpSocket>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonParseError>

class tcpsocketRequire::DataPrivate
{
public:
    DataPrivate()
        :socket(new QTcpSocket())
    {

    }
    ~DataPrivate()
    {
        if(socket)
        {
            delete socket;
            socket = nullptr;
        }
    }

public:
    QTcpSocket* socket;
};

tcpsocketRequire::tcpsocketRequire(const QString &ip,const QString & connectPort,QObject *parent)
    : RequireBase(ip,connectPort,parent)
    ,_p(new DataPrivate())
{
    Init();
}

tcpsocketRequire::~tcpsocketRequire()
{
    delete _p;
    _p = nullptr;
}

void tcpsocketRequire::postData(const QString &data)
{
    if(!isConnected())
    {
        qDebug()<<"tcpsocket not connect to "<<getConnectIP()<<":"<<getConnectPort();
        return;
    }

    _p->socket->write(data.toUtf8());

    QString receive;

    // 如果数据太长， 循环读取 直到读到的 "{" 和 "}" 一样多
    do
    {
        _p->socket->waitForReadyRead(120000);

        receive += _p->socket->readAll();

        // 判断是否接收到一个完整的json  完整的就退出read循环
        QJsonParseError json_error;
        QJsonDocument::fromJson(receive.toLatin1(), &json_error);
        if(QJsonParseError::NoError == json_error.error)
        {
            break;
        }

    }while(1);

    emit receiveData(receive);
}

void tcpsocketRequire::startConnect()
{
    _p->socket->connectToHost(QHostAddress(getConnectIP()), getConnectPort().toInt());
    if (!_p->socket->waitForConnected()) {
        qDebug() << "could not connect to server: "<< _p->socket->errorString();
        return;
    }

    emit connectFinish();
}

bool tcpsocketRequire::isConnected()const
{
    return  QAbstractSocket::ConnectedState == _p->socket->state();
}

void tcpsocketRequire::Init()
{

}
