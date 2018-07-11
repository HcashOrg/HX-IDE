#include "DataManager.h"

#include <mutex>
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

class DataManager::DataPrivate
{
public:
    DataPrivate()
        :accountData(std::make_shared<AccountData>())
        ,contractData(std::make_shared<AddressContractData>())
    {

    }
public:
    DataDefine::AccountDataPtr accountData;
    DataDefine::AddressContractDataPtr contractData;
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
    _p->accountData->clear();

<<<<<<< HEAD
    ChainIDE::getInstance()->postRPC("query-listaccounts",IDEUtil::toJsonFormat("list_my_accounts",QJsonArray()));
=======
    ChainIDE::getInstance()->postRPC("query-listaccounts",IDEUtil::toJsonFormat("listaccounts",QJsonArray()));
>>>>>>> b41c4ef8ab0b45651d7ad4ccd9b34bd3691512f8
}

const DataDefine::AccountDataPtr &DataManager::getAccount() const
{
    return _p->accountData;
}

void DataManager::queryContract()
{
<<<<<<< HEAD
    _p->contractData->clear();

    for(auto it = _p->accountData->getAccount().begin();it != _p->accountData->getAccount().end();++it)
    {
        ChainIDE::getInstance()->postRPC("query-get_contract_"+(*it)->getAccountName(),IDEUtil::toJsonFormat("get_contract_addresses_by_owner",
                                                              QJsonArray()<<(*it)->getAccountName()));
    }
    ChainIDE::getInstance()->postRPC("query-getcontract-finish",IDEUtil::toJsonFormat("finishquery",QJsonArray()));
}

const AddressContractDataPtr &DataManager::getContract() const
{
    return _p->contractData;
}

void DataManager::dealNewState()
{
    ChainIDE::getInstance()->postRPC("deal-is_new",IDEUtil::toJsonFormat("is_new",QJsonArray()));
=======
    ChainIDE::getInstance()->postRPC("data-checkaddress",IDEUtil::toJsonFormat("validateaddress",QJsonArray()<<addr));
>>>>>>> b41c4ef8ab0b45651d7ad4ccd9b34bd3691512f8
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
<<<<<<< HEAD
            AccountInfoVec accounts = _p->accountData->getAccount();
            std::for_each(accounts.begin(),accounts.end(),[](const AccountInfoPtr &account){
                ChainIDE::getInstance()->postRPC(QString("query-getassetbyaccount_%1").arg(account->getAccountName()),
                                                 IDEUtil::toJsonFormat("get_account_balances",
=======
            std::for_each(_p->accountData->getAccount().begin(),_p->accountData->getAccount().end(),[](const AccountInfoPtr &account){
                ChainIDE::getInstance()->postRPC(QString("query-getaddressesbyaccount_%1").arg(account->getAccountName()),
                                                 IDEUtil::toJsonFormat("getaddressesbyaccount",
>>>>>>> b41c4ef8ab0b45651d7ad4ccd9b34bd3691512f8
                                               QJsonArray()<<account->getAccountName()));
            });
            ChainIDE::getInstance()->postRPC("query-getaddresses-finish",IDEUtil::toJsonFormat("finishquery",QJsonArray()));
        }
    }
    else if(id.startsWith("query-getassetbyaccount_"))
    {
        QString accountName = id.mid(QString("query-getassetbyaccount_").length());
        //查找地址对应的余额
        parseAddresses(accountName,data);
    }
    else if("query-getaddresses-finish" == id)
    {
<<<<<<< HEAD
        //查询资产信息
        ChainIDE::getInstance()->postRPC("query-list_assets",IDEUtil::toJsonFormat("list_assets",QJsonArray()<<"A"<<100));
=======
        //查询每个地址对应的金额
        ChainIDE::getInstance()->postRPC("listunspent",IDEUtil::toJsonFormat("listunspent",QJsonArray()));
>>>>>>> b41c4ef8ab0b45651d7ad4ccd9b34bd3691512f8
    }
    else if("query-list_assets" == id)
    {
        if(parseAddressBalances(data))
        {
            emit queryAccountFinish();
        }
    }
    //处理新内容
    else if("deal-is_new" == id)
    {
        QJsonParseError json_error;
        QJsonDocument parse_doucment = QJsonDocument::fromJson(data.toLatin1(),&json_error);
        if(json_error.error != QJsonParseError::NoError || !parse_doucment.isObject())
        {
            qDebug()<<json_error.errorString();
            return;
        }
        if(parse_doucment.object().value("result").toBool())
        {
            //设置默认密码 11111111
            ChainIDE::getInstance()->postRPC("deal-set_password",IDEUtil::toJsonFormat("set_password",QJsonArray()<<"11111111"));
        }
        else if(ChainIDE::getInstance()->getCurrentChainType() == 1)
        {
            //如果是测试连，就直接解锁
            ChainIDE::getInstance()->postRPC("deal-unlocktestchain",IDEUtil::toJsonFormat("unlock",QJsonArray()<<"11111111"));
        }

    }
    //处理查询结果
    else if(id.startsWith("query-get_contract_"))
    {
        QString accountName = id.mid(QString("query-get_contract_").length());
        parseContract(accountName,data);
    }
    else if("query-getcontract-finish" == id)
    {
        emit queryContractFinish();
    }
}

bool DataManager::parseListAccount(const QString &data)
{
    std::lock_guard<std::mutex> lo(_p->dataMutex);
    QJsonParseError json_error;
    QJsonDocument parse_doucment = QJsonDocument::fromJson(data.toLatin1(),&json_error);
    if(json_error.error != QJsonParseError::NoError || !parse_doucment.isObject())
    {
        qDebug()<<json_error.errorString();
        return false;
    }
    QJsonArray resultArray = parse_doucment.object().value("result").toArray();
    foreach (QJsonValue addr, resultArray) {
        if(!addr.isObject()) continue;
        QJsonObject obj = addr.toObject();
        _p->accountData->insertAccount(obj.value("name").toString(),obj.value("addr").toString());
    }
    return true;
}

bool DataManager::parseAddresses(const QString &accountName,const QString &data)
{
    std::lock_guard<std::mutex> lo(_p->dataMutex);
//    qDebug()<<"query getaddressesbyaccount"<<data << "accountname"<<accountName;
    QJsonParseError json_error;
    QJsonDocument parse_doucment = QJsonDocument::fromJson(data.toLatin1(),&json_error);
    if(json_error.error != QJsonParseError::NoError || !parse_doucment.isObject())
    {
         qDebug()<<json_error.errorString();
         return false;
    }
    QJsonArray jsonArr = parse_doucment.object().value("result").toArray();

    foreach (QJsonValue addr, jsonArr) {
        if(!addr.isObject()) continue;
        QJsonObject obj = addr.toObject();
        _p->accountData->insertAsset(accountName,obj.value("asset_id").toString(),obj.value("amount").toDouble());
    }
    return true;

}

bool DataManager::parseAddressBalances(const QString &data)
{
    std::lock_guard<std::mutex> lo(_p->dataMutex);
    QJsonParseError json_error;
    QJsonDocument parse_doucment = QJsonDocument::fromJson(data.toLatin1(),&json_error);
    if(json_error.error != QJsonParseError::NoError || !parse_doucment.isObject())
    {
         qDebug()<<json_error.errorString();
         return false;
    }
    QJsonArray jsonArr = parse_doucment.object().value("result").toArray();
    foreach(QJsonValue addr, jsonArr){
        if(!addr.isObject()) continue;
        QJsonObject obj = addr.toObject();
        _p->accountData->makeupInfo(obj.value("id").toString(),obj.value("symbol").toString(),obj.value("precision").toInt());
    }
    return true;
}

bool DataManager::parseContract(const QString &accountName,const QString &data)
{
    std::lock_guard<std::mutex> lo(_p->dataMutex);
    QJsonParseError json_error;
    QJsonDocument parse_doucment = QJsonDocument::fromJson(data.toLatin1(),&json_error);
    if(json_error.error != QJsonParseError::NoError || !parse_doucment.isObject())
    {
         qDebug()<<json_error.errorString();
         return false;
    }
    QJsonArray jsonArr = parse_doucment.object().value("result").toArray();
    foreach(QJsonValue addr, jsonArr){
        if(!addr.isString()) continue;
        _p->contractData->AddContract(accountName,addr.toString());
    }
    return true;

}


