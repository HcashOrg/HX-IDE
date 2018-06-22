#include "websocketRequire.h"

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
};


websocketRequire::websocketRequire(const QString &ip,const QString & connectPort,QObject *parent)
    : RequireBase(ip,connectPort,parent)
    ,_p(new DataPrivate())
{
    Init();
}

websocketRequire::~websocketRequire()
{
    delete _p;
}

void websocketRequire::postData(const QString &data)
{
    if(!isConnected())
    {
        qDebug()<<"websocket not connect to "<<getConnectPort();
        return;
    }

    _p->m_buff.clear();
    _p->m_webSocket->sendTextMessage(data);
}

void websocketRequire::startConnect()
{
    _p->m_webSocket->open( QUrl(QString("ws://%1:%2").arg(getConnectIP()).arg(getConnectPort())) );
}

bool websocketRequire::isConnected()
{
    return _p->m_webSocket->state() == QAbstractSocket::ConnectedState;
}

void websocketRequire::onTextFrameReceived(QString _message, bool _isLastFrame)
{
    _p->m_buff += _message;

    if(_isLastFrame)
    {
        QString result = _p->m_buff.mid( QString("{\"id\":32800,\"jsonrpc\":\"2.0\",").size());
        result = result.left( result.size() - 1);

        emit receiveData(result);
        _p->m_buff.clear();
    }
}

void websocketRequire::onStateChanged(QAbstractSocket::SocketState _state)
{
    qDebug() << "websocket onStateChanged: "  <<_state<< _p->m_webSocket->errorString();

    if( _state == QAbstractSocket::UnconnectedState)
    {
        startConnect();
    }
    else if(_state == QAbstractSocket::ConnectedState)
    {
         qDebug() << "websocket connected ";
        emit connectFinish();
    }
}

void websocketRequire::Init()
{
    connect(_p->m_webSocket,&QWebSocket::textFrameReceived,this,&websocketRequire::onTextFrameReceived);
    connect(_p->m_webSocket,&QWebSocket::stateChanged,this,&websocketRequire::onStateChanged);
}
