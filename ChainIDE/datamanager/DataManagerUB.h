#ifndef DataManagerUB_H
#define DataManagerUB_H

#include <QObject>
#include "DataDefine.h"

class DataManagerUB : public QObject
{
    Q_OBJECT
public:
    void queryAccount();//查询ub账户信息
    const DataDefine::AccountUB::AccountDataPtr &getAccount()const;

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

public:
    static DataManagerUB *getInstance();
    ~DataManagerUB();
public:
    DataManagerUB(const DataManagerUB &)=delete;
    DataManagerUB( DataManagerUB &&)=delete;
    DataManagerUB& operator =(const DataManagerUB &) = delete;
    DataManagerUB& operator =( DataManagerUB &&) = delete;
private:
    explicit DataManagerUB(QObject *parent = 0);
    static DataManagerUB *_instance;
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

#endif // DataManagerUB_H
