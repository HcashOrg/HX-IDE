#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include <QObject>
#include "DataDefine.h"

class DataManager : public QObject
{
    Q_OBJECT
public:
    explicit DataManager(QObject *parent = 0);
    ~DataManager();
public:
    void queryAccount();//查询ub账户信息
    const DataDefine::AccountDataPtr &getAccount()const;

    void checkAddress(const QString &addr);//检测地址合法性
public:
    void InitManager();
private slots:
    void jsonDataUpdated(const QString &id,const QString &data);
signals:
    void queryAccountFinish();
    void addressCheckFinish(bool);

private:
    bool parseListAccount(const QString &data);
    bool parseAddresses(const QString &accountName,const QString &data);
    bool parseAddressBalances(const QString &data);


private:
    class DataPrivate;
    DataPrivate *_p;
};

#endif // DATAMANAGER_H
