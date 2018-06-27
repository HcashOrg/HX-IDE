#include "DataManager.h"

#include <mutex>
#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QJsonValue>

#include "ChainIDE.h"
#include "IDEUtil.h"
using namespace DataDefine;

class DataManager::DataPrivate
{
public:
    DataPrivate()
        :accountData(std::make_shared<AccountData>())
    {

    }
public:
    DataDefine::AccountDataPtr accountData;
    std::mutex dataMutex;
};

DataManager::DataManager(QObject *parent)
    : QObject(parent)
    ,_p(new DataPrivate())
{

}

DataManager::~DataManager()
{
    delete _p;
}

void DataManager::queryAccount()
{
    std::lock_guard<std::mutex> guard(_p->dataMutex);
    _p->accountData->clear();

    ChainIDE::getInstance()->postRPC("query-listaccounts",IDEUtil::toUbcdHttpJsonFormat("listaccounts",QJsonArray()));
}

const DataDefine::AccountDataPtr &DataManager::getAccount() const
{
    return _p->accountData;
}

void DataManager::InitManager()
{
    connect(ChainIDE::getInstance(),&ChainIDE::jsonDataUpdated,this,&DataManager::jsonDataUpdated);
}

void DataManager::jsonDataUpdated(const QString &id, const QString &data)
{
    if("query-listaccounts" == id)
    {
        if(parseListAccount(data))
        {
            //查询每一个账户对应的地址
            std::for_each(_p->accountData->getAccount().begin(),_p->accountData->getAccount().end(),[](const AccountInfoPtr &account){
                ChainIDE::getInstance()->postRPC(QString("query-getaddressesbyaccount_%1").arg(account->getAccountName()),
                                                 IDEUtil::toUbcdHttpJsonFormat("getaddressesbyaccount",
                                               QJsonArray()<<account->getAccountName()));
            });
            ChainIDE::getInstance()->postRPC("query-getaddresses-finish","finishquery");
        }
    }
    else if(id.startsWith("query-getaddressesbyaccount_"))
    {
        QString accountName = id.mid(QString("query-getaddressesbyaccount_").length());
        //查找地址对应的余额
        parseAddresses(accountName,data);
    }
    else if("query-getaddresses-finish" == id)
    {
        //查询每个地址对应的金额
        ChainIDE::getInstance()->postRPC("listunspent",IDEUtil::toUbcdHttpJsonFormat("listunspent",QJsonArray()));
    }
    else if("listunspent" == id)
    {
        QString lidata = data;
        qDebug()<<"query listunspent"<<lidata;
        if(parseAddressBalances(lidata))
        {
            emit queryAccountFinish();
        }
        else
        {
            qDebug()<<"toLong"<<data.size();
        }
    }
}

bool DataManager::parseListAccount(const QString &data)
{
    qDebug()<<"query listaccounts"<<data;
    QJsonParseError json_error;
    QJsonDocument parse_doucment = QJsonDocument::fromJson(data.toLatin1(),&json_error);
    if(json_error.error != QJsonParseError::NoError || !parse_doucment.isObject()) return false;
    QJsonObject jsonObject = parse_doucment.object();

    foreach (QString name, jsonObject.keys()) {
        _p->accountData->insertAccount(name,jsonObject.value(name).toDouble());
    }
    return true;
}

bool DataManager::parseAddresses(const QString &accountName,const QString &data)
{
    qDebug()<<"query getaddressesbyaccount"<<data << "accountname"<<accountName;
    QJsonParseError json_error;
    QJsonDocument parse_doucment = QJsonDocument::fromJson(data.toLatin1(),&json_error);
    if(json_error.error != QJsonParseError::NoError || !parse_doucment.isArray())
    {
         qDebug()<<json_error.errorString();
         return false;
    }
    QJsonArray jsonArr = parse_doucment.array();

    foreach (QJsonValue addr, jsonArr) {
        _p->accountData->insertAddress(accountName,addr.toString(),0);
    }
    return true;

}

bool DataManager::parseAddressBalances(const QString &data)
{
    QJsonParseError json_error;
    QJsonDocument parse_doucment = QJsonDocument::fromJson(data.toLatin1(),&json_error);
    if(json_error.error != QJsonParseError::NoError || !parse_doucment.isArray())
    {
         qDebug()<<json_error.errorString();
         return false;
    }
    QJsonArray jsonArr = parse_doucment.array();
    foreach(QJsonValue addr, jsonArr){
        if(!addr.isObject()) continue;
        QJsonObject obj = addr.toObject();
        qDebug()<<obj.value("address").toString()<<obj.value("amount").toDouble();
        _p->accountData->addAddressBalance(obj.value("address").toString(),obj.value("amount").toDouble());
    }
    return true;
}
