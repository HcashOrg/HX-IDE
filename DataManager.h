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
    void queryAccount();//查询账户信息
    const DataDefine::AccountDataPtr &getAccount()const;

    void queryContract();//查询合约
    const DataDefine::AddressContractDataPtr &getContract()const;

    void dealNewState();//处理第一次打开ide的情况
public:
    void InitManager();
private slots:
    void jsonDataUpdated(const QString &id,const QString &data);
signals:
    void queryAccountFinish();
    void queryContractFinish();
private:
    bool parseListAccount(const QString &data);
    bool parseAddresses(const QString &accountName,const QString &data);
    bool parseAddressBalances(const QString &data);

    bool parseContract(const QString &accountName,const QString &data);
private:
    class DataPrivate;
    DataPrivate *_p;
};

#endif // DATAMANAGER_H
