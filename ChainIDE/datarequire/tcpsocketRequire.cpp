#include "tcpsocketRequire.h"

#include <QHostAddress>
#include <QTcpSocket>

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
            socket->close();
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
        qDebug()<<"tcpsocket not connect to "<<getConnectIP()<<":"<<getConnectPort()<<data<<" abandoned";
        return;
    }

    _p->socket->write(data.toUtf8());    
}

void tcpsocketRequire::startConnect()
{
    connect(_p->socket,&QTcpSocket::readyRead,this,&tcpsocketRequire::readyReadSlots);
    _p->socket->connectToHost(QHostAddress(getConnectIP()), getConnectPort().toInt());
    if (!_p->socket->waitForConnected()) {
        qDebug() << "could not connect to server: "<< _p->socket->errorString();
        emit connectFailed();
        return;
    }

    emit connectFinish();
}

bool tcpsocketRequire::isConnected()const
{
    return  QAbstractSocket::ConnectedState == _p->socket->state();
}

void tcpsocketRequire::readyReadSlots()
{
//    QString receive;

//    // 如果数据太长， 循环读取 直到读到的 "{" 和 "}" 一样多
//    do
//    {
//        _p->socket->waitForReadyRead(120000);

//        receive += _p->socket->readAll();

//        // 判断是否接收到一个完整的json  完整的就退出read循环
//        QJsonParseError json_error;
//        QJsonDocument::fromJson(receive.toUtf8(), &json_error);
//        if(QJsonParseError::NoError == json_error.error)
//        {
//            break;
//        }

//    }while(1);

    emit receiveData(_p->socket->readAll());
}

void tcpsocketRequire::Init()
{
}
