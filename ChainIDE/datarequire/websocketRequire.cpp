#include "websocketRequire.h"

#include <mutex>
#include <QWebSocket>
class websocketRequire::DataPrivate
{
public:
    DataPrivate()
        :m_webSocket(new QWebSocket())
    {

    }
public:
    QWebSocket *m_webSocket;
    QString m_buff;
    std::mutex buffMutex;
};


websocketRequire::websocketRequire(const QString &ip,const QString & connectPort,QObject *parent)
    : RequireBase(ip,connectPort,parent)
    ,_p(new DataPrivate())
{
    Init();
}

websocketRequire::~websocketRequire()
{
    disconnect(_p->m_webSocket,&QWebSocket::textFrameReceived,this,&websocketRequire::onTextFrameReceived);
    disconnect(_p->m_webSocket,&QWebSocket::stateChanged,this,&websocketRequire::onStateChanged);
    delete _p;
    _p = nullptr;
}

void websocketRequire::postData(const QString &data)
{
    if(!isConnected())
    {
        qDebug()<<"websocket not connect to "<<getConnectIP()<<":"<<getConnectPort()<<data<<" abandoned";
        return;
    }
    {
        std::lock_guard<std::mutex> loc(_p->buffMutex);
        _p->m_buff.clear();
    }
    _p->m_webSocket->sendTextMessage(data);
}

void websocketRequire::startConnect()
{
    _p->m_webSocket->open( QUrl(QString("ws://%1:%2").arg(getConnectIP()).arg(getConnectPort())) );
}

bool websocketRequire::isConnected()const
{
    return _p->m_webSocket->state() == QAbstractSocket::ConnectedState;
}

void websocketRequire::onTextFrameReceived(QString _message, bool _isLastFrame)
{
    {
        std::lock_guard<std::mutex> loc(_p->buffMutex);
        _p->m_buff += _message;
    }

    if(_isLastFrame)
    {
        emit receiveData(_p->m_buff);
        std::lock_guard<std::mutex> loc(_p->buffMutex);
        _p->m_buff.clear();
    }
}

void websocketRequire::onStateChanged(QAbstractSocket::SocketState _state)
{
    //qDebug() << "websocket onStateChanged: "  <<_state<< _p->m_webSocket->errorString();

    if( _state == QAbstractSocket::UnconnectedState)
    {
        qDebug()<<"dis connected ,reconnect";
        startConnect();
    }
    else if(_state == QAbstractSocket::ConnectedState)
    {
        qDebug() << "websocket connected " << getConnectIP()<<getConnectPort();
        emit connectFinish();
    }
}

void websocketRequire::Init()
{
    connect(_p->m_webSocket,&QWebSocket::textFrameReceived,this,&websocketRequire::onTextFrameReceived);
    connect(_p->m_webSocket,&QWebSocket::stateChanged,this,&websocketRequire::onStateChanged);
}
