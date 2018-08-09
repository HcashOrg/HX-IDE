#ifndef LocalPeer_h__
#define LocalPeer_h__

#include <QObject>
//利用qlocalserver实现单程序功能
class LocalPeer : public QObject
{
	Q_OBJECT
public:
	LocalPeer(QObject *parent = nullptr);
	~LocalPeer();
signals:
    void newConnection();
public:
	bool IsAlreadyRunning()const;
private:
	void InitPeer();
	void CreateNewServer();
private:
	class DataPrivate;
	DataPrivate *_p;
};
#endif // LocalPeer_h__
