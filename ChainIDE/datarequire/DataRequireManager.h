#ifndef DATAREQUIREMANAGER_H
#define DATAREQUIREMANAGER_H

#include <QObject>
//数据对外请求封装，任何对外的数据请求都通过本类，，
//有变动的仅为requirebase链接类，，例如使用websocketrequire类链接、或者使用其他链接方式
class DataRequireManager : public QObject
{
    Q_OBJECT
public:
    explicit DataRequireManager(const QString &ip="127.0.0.1",const QString & connectPort = "50320",QObject *parent = 0);
    ~DataRequireManager();
    enum ConnectType{WEBSOCKET,TCP,HTTP,HTTPWITHUSER};
public:
    //开始连接器,需要链接的时候，调用此函数
    void startManager(ConnectType connecttype = WEBSOCKET);
    //清空请求
    void requireClear();
    //设置额外信息---主要给http使用，有点多余，但没办法
    void setAdditional(const QByteArray &headerName, const QByteArray &value);

    //判断是否连接
    bool isConnected()const;
signals:
    void requireResponse(const QString &_rpcId,const QString &message);//回到远程回复
    void connectFinish();
public slots:
    void requirePosted(const QString &_rpcId, const QString &_rpcCmd);//收到数据请求
    void receiveResponse(const QString &message);

private slots:
    void processRequire();
private:
    void InitManager();
private:
    class DataPrivate;
    DataPrivate *_p;
};

#endif // DATAREQUIREMANAGER_H
