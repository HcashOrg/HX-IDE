#include "DataManagerCTC.h"

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
using namespace DataManagerStruct;

class DataManagerCTC::DataPrivate
{
public:
    DataPrivate()
        :accountData(std::make_shared<AccountCTC::AccountData>())
        ,contractData(std::make_shared<AddressContractData>())
    {

    }
public:
    DataManagerStruct::AccountCTC::AccountDataPtr accountData;
    DataManagerStruct::AddressContractDataPtr contractData;
};

DataManagerCTC *DataManagerCTC::getInstance()
{
    if(_instance == nullptr)
    {
        _instance = new DataManagerCTC();
    }
    return _instance;
}

DataManagerCTC::DataManagerCTC(QObject *parent)
    : QObject(parent)
    ,_p(new DataPrivate())
{

}
DataManagerCTC * DataManagerCTC::_instance = nullptr;
DataManagerCTC::CGarbo DataManagerCTC::Garbo;

DataManagerCTC::~DataManagerCTC()
{
    qDebug()<<"delete DataManagerCTC";
    delete _p;
    _p = nullptr;
}

void DataManagerCTC::queryAccount()
{
    _p->accountData->clear();

    ChainIDE::getInstance()->postRPC("query-listaccounts-address",IDEUtil::toJsonFormat("wallet_list_my_addresses",QJsonArray(),true));
}

const AccountCTC::AccountDataPtr &DataManagerCTC::getAccount() const
{
    return _p->accountData;
}

void DataManagerCTC::dealNewState()
{
    ChainIDE::getInstance()->postRPC("deal-is_new",IDEUtil::toJsonFormat("wallet_open",QJsonArray()<<"wallet",true));
}

void DataManagerCTC::InitManager()
{
    connect(ChainIDE::getInstance(),&ChainIDE::jsonDataUpdated,this,&DataManagerCTC::jsonDataUpdated);
}

void DataManagerCTC::jsonDataUpdated(const QString &id, const QString &data)
{
    if("deal-is_new" == id)
    {
        QJsonParseError json_error;
        QJsonDocument parse_doucment = QJsonDocument::fromJson(data.toLatin1(),&json_error);
        if(json_error.error != QJsonParseError::NoError || !parse_doucment.isObject())
        {
            qDebug()<<json_error.errorString();
            return;
        }
        if(parse_doucment.object().value("result").isNull())
        {
            //解锁钱包
            ChainIDE::getInstance()->postRPC("deal-is_new_unlock",IDEUtil::toJsonFormat("wallet_unlock",QJsonArray()<<"45"<<"11111111",true));
        }
        else
        {
            ChainIDE::getInstance()->postRPC("deal-is_new_create",IDEUtil::toJsonFormat("wallet_create",QJsonArray()<<"wallet"<<"11111111",true));
        }
    }
    else if("query-listaccounts-address" == id)
    {
        if(parseListAccount(data))
        {
            //查询每一个账户对应的地址
            AccountCTC::AccountInfoVec accounts = _p->accountData->getAccount();
            std::for_each(accounts.begin(),accounts.end(),[](const AccountCTC::AccountInfoPtr &account){
                ChainIDE::getInstance()->postRPC(QString("query-getassetbyaccount_%1").arg(account->getAccountName()),
                                                 IDEUtil::toJsonFormat("wallet_account_balance",
                                               QJsonArray()<<account->getAccountName(),true));
            });
            ChainIDE::getInstance()->postRPC("query-getaddresses-finish",IDEUtil::toJsonFormat("finishquery",QJsonArray(),true));
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
        //查询资产信息
        ChainIDE::getInstance()->postRPC("query-list_assets",IDEUtil::toJsonFormat("blockchain_list_assets",QJsonArray(),true));
    }
    else if("query-list_assets" == id)
    {
        if(parseAddressBalances(data))
        {
            //ceshi





            emit queryAccountFinish();
        }
    }

}

bool DataManagerCTC::parseListAccount(const QString &data)
{
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
        _p->accountData->insertAccount(obj.value("name").toString(),obj.value("owner_address").toString());
    }
    return true;
}

bool DataManagerCTC::parseAddresses(const QString &accountName, const QString &data)
{
    QJsonParseError json_error;
    QJsonDocument parse_doucment = QJsonDocument::fromJson(data.toLatin1(),&json_error);
    if(json_error.error != QJsonParseError::NoError || !parse_doucment.isObject())
    {
         qDebug()<<json_error.errorString();
         return false;
    }
    QJsonArray jsonArr = parse_doucment.object().value("result").toArray();
    qDebug()<<parse_doucment;
    foreach (QJsonValue addr, jsonArr) {
        if(!addr.isArray()) continue;
        QJsonArray arr = addr.toArray();
        foreach(QJsonValue val,arr){
            if(!val.isArray()) continue;
            QJsonArray valArr = val.toArray();
            foreach (QJsonValue ass, valArr) {
                if(!ass.isArray()) continue;
                QJsonArray asset = ass.toArray();
                qDebug()<<asset;
                qDebug()<<accountName<<asset.at(0).toInt()<<asset.at(1).toString().toDouble();
                if(asset.at(1).isDouble())
                {
                    _p->accountData->insertAsset(accountName,asset.at(0).toInt(),asset.at(1).toDouble());
                }
                else if(asset.at(1).isString())
                {
                    _p->accountData->insertAsset(accountName,asset.at(0).toInt(),asset.at(1).toString().toDouble());
                }
            }
        }
    }
    return true;
}

bool DataManagerCTC::parseAddressBalances(const QString &data)
{
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
        _p->accountData->makeupInfo(obj.value("id").toInt(),obj.value("symbol").toString(),obj.value("precision").toInt());
    }
    return true;
}
