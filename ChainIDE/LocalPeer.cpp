#include "LocalPeer.h"

#include <QtNetwork/QLocalServer>
#include <QtNetwork/QLocalSocket>

class LocalPeer::DataPrivate
{
public:
	DataPrivate()
		:localServer(new QLocalServer())
		, isRuning(false)
        , OutTime(5000)
	{
		ServerName = "onlyApp";
	}
	~DataPrivate()
	{
		delete localServer;
		localServer = nullptr;
	}
public:
	QLocalServer * localServer;
	bool isRuning;
	QString ServerName;
	int OutTime;
};

LocalPeer::LocalPeer(QObject *parent)
	: QObject(parent)
	,_p(new DataPrivate())
{
	InitPeer();
}

LocalPeer::~LocalPeer()
{
	delete _p;
}

bool LocalPeer::IsAlreadyRunning() const
{
	return _p->isRuning;
}

void LocalPeer::InitPeer()
{
	QLocalSocket socket;
	socket.connectToServer(_p->ServerName);
	if (socket.waitForConnected(_p->OutTime)) {
		//说明已经有实例，自身不启动
		_p->isRuning = true;
	}
	else {
		_p->isRuning = false;
		CreateNewServer();
	}
	
}

void LocalPeer::CreateNewServer()
{
	if (!_p->localServer->listen(_p->ServerName)) {
	    // 此时监听失败，可能是程序崩溃时,残留进程服务导致的,移除之
		if (_p->localServer->serverError() == QAbstractSocket::AddressInUseError) {
			QLocalServer::removeServer(_p->ServerName); // <-- 重点
			_p->localServer->listen(_p->ServerName); // 再次监听
		}
	}
}
