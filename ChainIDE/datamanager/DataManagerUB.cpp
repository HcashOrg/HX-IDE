#include "datamanager/DataManagerUB.h"

#include <QDebug>
#include <QCoreApplication>
#include <QDir>
#include <QTextCodec>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QJsonValue>

#include "ChainIDE.h"
#include "IDEUtil.h"
using namespace DataDefine;

class DataManagerUB::DataPrivate
{
public:
    DataPrivate()
        :accountData(std::make_shared<AccountUB::AccountData>())
    {

    }
public:
    DataDefine::AccountUB::AccountDataPtr accountData;
};

DataManagerUB *DataManagerUB::getInstance()
{
    if(_instance == nullptr)
    {
        _instance = new DataManagerUB();
    }
    return _instance;
}

DataManagerUB::DataManagerUB(QObject *parent)
    : QObject(parent)
    ,_p(new DataPrivate())
{

}
DataManagerUB * DataManagerUB::_instance = nullptr;
DataManagerUB::CGarbo DataManagerUB::Garbo;

DataManagerUB::~DataManagerUB()
{
    qDebug()<<"delete DataManagerUB";
    delete _p;
    _p = nullptr;
}

void DataManagerUB::queryAccount()
{
    _p->accountData->clear();

    ChainIDE::getInstance()->postRPC("query-listaccounts",IDEUtil::toJsonFormat("listaccounts",QJsonArray()));
}

const DataDefine::AccountUB::AccountDataPtr &DataManagerUB::getAccount() const
{
    return _p->accountData;
}

void DataManagerUB::checkAddress(const QString &addr)
{
    ChainIDE::getInstance()->postRPC("data-checkaddress",IDEUtil::toJsonFormat("validateaddress",QJsonArray()<<addr));
}

void DataManagerUB::InitManager()
{
    connect(ChainIDE::getInstance(),&ChainIDE::jsonDataUpdated,this,&DataManagerUB::jsonDataUpdated);
}

void DataManagerUB::jsonDataUpdated(const QString &id, const QString &data)
{
    if("query-listaccounts" == id)
    {
        if(parseListAccount(data))
        {
            //查询每一个账户对应的地址
            std::for_each(_p->accountData->getAccount().begin(),_p->accountData->getAccount().end(),[](const AccountUB::AccountInfoPtr &account){
                ChainIDE::getInstance()->postRPC(QString("query-getaddressesbyaccount_%1").arg(account->getAccountName()),
                                                 IDEUtil::toJsonFormat("getaddressesbyaccount",
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
        ChainIDE::getInstance()->postRPC("listunspent",IDEUtil::toJsonFormat("listunspent",QJsonArray()));
    }
    else if("listunspent" == id)
    {
        QString lidata = data;
        if(parseAddressBalances(lidata))
        {
            emit queryAccountFinish();
        }
    }
    //检测地址合法性
    else if("data-checkaddress" == id)
    {
        QJsonParseError json_error;
        QJsonDocument parse_doucment = QJsonDocument::fromJson(data.toLatin1(),&json_error);
        if(json_error.error != QJsonParseError::NoError)
        {
            emit addressCheckFinish(false);
            return;
        }
        QJsonObject jsonObject = parse_doucment.object().value("result").toObject();
        emit addressCheckFinish(jsonObject.value("isvalid").toBool());
    }
}

bool DataManagerUB::parseListAccount(const QString &data)
{
    QJsonParseError json_error;
    QJsonDocument parse_doucment = QJsonDocument::fromJson(data.toLatin1(),&json_error);
    if(json_error.error != QJsonParseError::NoError)
    {
        qDebug()<<json_error.errorString();
        return false;
    }
    QJsonObject jsonObject = parse_doucment.object().value("result").toObject();

    foreach (QString name, jsonObject.keys()) {
        _p->accountData->insertAccount(name,jsonObject.value(name).toDouble());
    }
    return true;
}

bool DataManagerUB::parseAddresses(const QString &accountName,const QString &data)
{
//    qDebug()<<"query getaddressesbyaccount"<<data << "accountname"<<accountName;
    QJsonParseError json_error;
    QJsonDocument parse_doucment = QJsonDocument::fromJson(data.toLatin1(),&json_error);
    if(json_error.error != QJsonParseError::NoError)
    {
         qDebug()<<json_error.errorString();
         return false;
    }
    QJsonArray jsonArr = parse_doucment.object().value("result").toArray();

    foreach (QJsonValue addr, jsonArr) {
        _p->accountData->insertAddress(accountName,addr.toString(),0);
    }
    return true;

}

bool DataManagerUB::parseAddressBalances(const QString &data)
{
    QJsonParseError json_error;
    QJsonDocument parse_doucment = QJsonDocument::fromJson(data.toLatin1(),&json_error);
    if(json_error.error != QJsonParseError::NoError )
    {
         qDebug()<<json_error.errorString();
         return false;
    }
    QJsonArray jsonArr = parse_doucment.object().value("result").toArray();
    foreach(QJsonValue addr, jsonArr){
        if(!addr.isObject()) continue;
        QJsonObject obj = addr.toObject();
        _p->accountData->addAddressBalance(obj.value("address").toString(),obj.value("amount").toDouble());
    }
    return true;
}


