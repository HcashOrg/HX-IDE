#ifndef DATAMANAGERCTC_H
#define DATAMANAGERCTC_H

#include <QObject>
#include "DataManagerStruct.h"

class DataManagerCTC : public QObject
{
    Q_OBJECT
public:
    void queryAccount();//查询账户信息
    const DataManagerStruct::AccountCTC::AccountDataPtr &getAccount()const;

    void dealNewState();//处理第一次打开ide的情况
public:
    void InitManager();
private slots:
    void jsonDataUpdated(const QString &id,const QString &data);

signals:
    void queryAccountFinish();
private:
    bool parseListAccount(const QString &data);
    bool parseAddresses(const QString &accountName,const QString &data);
    bool parseAddressBalances(const QString &data);
public:
    static DataManagerCTC *getInstance();
public:
    DataManagerCTC(const DataManagerCTC &)=delete;
    DataManagerCTC(DataManagerCTC &&)=delete;
    DataManagerCTC& operator =(DataManagerCTC &&) = delete;
    DataManagerCTC& operator =(const DataManagerCTC &) = delete;
private:
    explicit DataManagerCTC(QObject *parent = 0);
    ~DataManagerCTC();
    static DataManagerCTC *_instance;
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

#endif // DATAMANAGERCTC_H
