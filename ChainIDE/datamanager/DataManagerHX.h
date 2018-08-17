#ifndef DataManagerHX_H
#define DataManagerHX_H

#include <QObject>
#include "DataManagerStruct.h"

class DataManagerHX : public QObject
{
    Q_OBJECT
public:
    void queryAccount();//查询账户信息
    const DataManagerStruct::AccountHX::AccountDataPtr &getAccount()const;

    void queryContract();//查询合约
    const DataManagerStruct::AddressContractDataPtr &getContract()const;

    void dealNewState();//处理第一次打开ide的情况

    void unlockWallet(const QString &password);
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
public:
    static DataManagerHX *getInstance();
    ~DataManagerHX();
public:
    DataManagerHX(const DataManagerHX &)=delete;
    DataManagerHX(DataManagerHX &&)=delete;
    DataManagerHX& operator =(DataManagerHX &&) = delete;
    DataManagerHX& operator =(const DataManagerHX &) = delete;
private:
    explicit DataManagerHX(QObject *parent = 0);
    static DataManagerHX *_instance;
    class CGarbo // 它的唯一工作就是在析构函数中删除 的实例
    {
    public:
        ~CGarbo()
        {
            if (_instance)
            {
                delete _instance;
                _instance = nullptr;
            }
        }
    };
    static CGarbo Garbo; // 定义一个静态成员，在程序结束时，系统会调用它的析构函数
private:
    class DataPrivate;
    DataPrivate *_p;
};

#endif // DataManagerHX_H
