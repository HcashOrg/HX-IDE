#include "workerthread.h"
#include "hxchain.h"

#include <QTcpSocket>
#include <QHostAddress>
#include <QTime>
#include <QJsonDocument>

QEvent::Type TaskEvent::s_evType = QEvent::None;

WorkerThread::WorkerThread(int id, int port, QObject *parent)
    : QThread(parent)
    , m_id(id)
    , port(port)
    , busy(true)
{
    qDebug() << "workthread construct " << m_id;
}


WorkerThread::~WorkerThread()
{
    qDebug() << "workthread destroy " << m_id;
}

bool WorkerThread::isBusy()
{
    return busy;
}

void WorkerThread::setBusy(bool busyOrNot)
{
    busy = busyOrNot;
}

void WorkerThread::processRPC( QString cmd)
{
    busy = true;


//              qDebug() << m_id << cmd;


    socket->write(cmd.toUtf8());

    QString receive;

//    // 如果数据太长， 循环读取 直到读到的 "{" 和 "}" 一样多
    do
    {
        if( !socket->waitForReadyRead(120000) )
        {
            sleep(1000);
        }

        receive += socket->readAll();

        // 判断是否接收到一个完整的json  完整的就退出read循环
        QJsonParseError json_error;
        QJsonDocument parse_doucment = QJsonDocument::fromJson(receive.toLatin1(), &json_error);
        if(json_error.error == QJsonParseError::NoError)
        {
            break;
        }
        else
        {
//            qDebug() << "json error: " << m_id << json_error.errorString();
        }

    }while(1);
//    }while( receive.count("{") > receive.count("}"));

    if( cmd.contains("upgrade_contract_testing_"))
    {
              qDebug() << m_id << receive;

    }
    QString rpcId = receive.mid( receive.indexOf("\"id\":") + 6,
                   receive.indexOf(",") - receive.indexOf("\"id\":") - 7);

    QString result = receive.mid( receive.indexOf(",") + 1);
    result = result.left( result.size() - 2);

    HXChain::getInstance()->updateJsonDataMap(rpcId, result);

    busy = false;
}

void WorkerThread::run()
{
    socket = new QTcpSocket(this);
    socket->connectToHost(QHostAddress("127.0.0.1"), port);
    if (!socket->waitForConnected()) {
        qDebug() << "could not connect to server: " << m_id << socket->errorString();
        return;
    }

    QString loginRpc = "{\"jsonrpc\":\"2.0\",\"id\":\"id_login\",\"method\":\"login\",\"params\":[\"" + QString( LOGIN_USER) + "\",\"" + QString( LOGIN_PWD) + "\"]}";
    socket->write(loginRpc.toUtf8());
    socket->waitForReadyRead(120000);
 qDebug() << "rpc login " << m_id <<   socket->readAll();
qDebug() << QThread::currentThreadId();
    busy = false;

    exec();
}

bool WorkerThread::event(QEvent *e)
{
    if(e->type() == TaskEvent::evType())
    {

        processRPC( static_cast<TaskEvent*>(e)->rpcCmd);
        return true;
    }
    QThread::event(e);
}

