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

void DataManagerCTC::queryContract()
{
    _p->contractData->clear();

    for(auto it = _p->accountData->getAccount().begin();it != _p->accountData->getAccount().end();++it)
    {
        ChainIDE::getInstance()->postRPC("query-get_contract_"+(*it)->getAccountName(),IDEUtil::toJsonFormat("wallet_get_contracts",
                                                              QJsonArray()<<(*it)->getAccountName(),true));
    }
    ChainIDE::getInstance()->postRPC("query-getcontract-finish",IDEUtil::toJsonFormat("finishquery",QJsonArray(),true));
}

const AddressContractDataPtr &DataManagerCTC::getContract() const
{
    return _p->contractData;
}

void DataManagerCTC::dealNewState()
{
    ChainIDE::getInstance()->postRPC("deal-is_new",IDEUtil::toJsonFormat("wallet_open",QJsonArray()<<"wallet",true));
}

void DataManagerCTC::initTestChain()
{
    ChainIDE::getInstance()->postRPC("init_",IDEUtil::toJsonFormat("import_key",QJsonArray()<<"5KPLxT3EYUWsUBrtEoPRFy8WLj4m7JooYVTUnZbPNipCX78ajtK",true));
    ChainIDE::getInstance()->postRPC("init_",IDEUtil::toJsonFormat("delegate_set_network_min_connection_count",QJsonArray()<<0,true));
    ChainIDE::getInstance()->postRPC("init_",IDEUtil::toJsonFormat("wallet_delegate_set_block_production",QJsonArray()<<"ALL"<<true,true));
    ChainIDE::getInstance()->postRPC("init_",IDEUtil::toJsonFormat("wallet_set_transaction_scanning",QJsonArray()<<true,true));
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
        QJsonDocument parse_doucment = QJsonDocument::fromJson(data.toUtf8(),&json_error);
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
            if(ChainIDE::getInstance()->getStartChainTypes() & DataDefine::TEST)
            {
                DataManagerCTC::getInstance()->initTestChain();
            }
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
            emit queryAccountFinish();
        }
    }
    else if(id.startsWith("query-get_contract_"))
    {
        QString accountName = id.mid(QString("query-get_contract_").length());
        parseContract(accountName,data);
    }
    else if("query-getcontract-finish" == id)
    {
        std::for_each(_p->contractData->getAllData().begin(),_p->contractData->getAllData().end(),[](const DataManagerStruct::AddressContractPtr& data){
            std::for_each(data->GetContracts().begin(),data->GetContracts().end(),[](const DataManagerStruct::ContractInfoPtr &info){
                ChainIDE::getInstance()->postRPC("query-contractinfo_"+info->GetContractAddr(),IDEUtil::toJsonFormat("contract_get_info",QJsonArray()<<info->GetContractAddr(),true));
            });
        });
        ChainIDE::getInstance()->postRPC("query-getcontract-info-finish",IDEUtil::toJsonFormat("finishquery",QJsonArray(),true));
    }
    else if(id.startsWith("query-contractinfo_"))
    {
        QString contractAddr = id.mid(QString("query-contractinfo_").length());
        parseContractInfo(contractAddr,data);
    }
    else if("query-getcontract-info-finish" == id)
    {
        emit queryContractFinish();
    }
    else if("init_" == id)
    {
        qDebug()<<data;
    }

}

bool DataManagerCTC::parseListAccount(const QString &data)
{
    QJsonParseError json_error;
    QJsonDocument parse_doucment = QJsonDocument::fromJson(data.toUtf8(),&json_error);
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
    QJsonDocument parse_doucment = QJsonDocument::fromJson(data.toUtf8(),&json_error);
    if(json_error.error != QJsonParseError::NoError || !parse_doucment.isObject())
    {
         qDebug()<<json_error.errorString();
         return false;
    }
    QJsonArray jsonArr = parse_doucment.object().value("result").toArray();
    foreach (QJsonValue addr, jsonArr) {
        if(!addr.isArray()) continue;
        QJsonArray arr = addr.toArray();
        foreach(QJsonValue val,arr){
            if(!val.isArray()) continue;
            QJsonArray valArr = val.toArray();
            foreach (QJsonValue ass, valArr) {
                if(!ass.isArray()) continue;
                QJsonArray asset = ass.toArray();
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
    QJsonDocument parse_doucment = QJsonDocument::fromJson(data.toUtf8(),&json_error);
    if(json_error.error != QJsonParseError::NoError || !parse_doucment.isObject())
    {
         qDebug()<<json_error.errorString();
         return false;
    }
    QJsonArray jsonArr = parse_doucment.object().value("result").toArray();
    foreach(QJsonValue addr, jsonArr){
        if(!addr.isObject()) continue;
        QJsonObject obj = addr.toObject();
        _p->accountData->makeupInfo(obj.value("id").toInt(),obj.value("symbol").toString(),IDEUtil::getDigit<int>(obj.value("precision").toInt()));
    }
    return true;
}

bool DataManagerCTC::parseContract(const QString &accountName, const QString &data)
{
    QJsonParseError json_error;
    QJsonDocument parse_doucment = QJsonDocument::fromJson(data.toUtf8(),&json_error);
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

bool DataManagerCTC::parseContractInfo(const QString &contaddr, const QString &data)
{
    QJsonParseError json_error;
    QJsonDocument parse_doucment = QJsonDocument::fromJson(data.toUtf8(),&json_error);
    if(json_error.error != QJsonParseError::NoError || !parse_doucment.isObject())
    {
         qDebug()<<json_error.errorString();
         _p->contractData->DeleteContract(contaddr);
         return false;
    }
    QJsonObject jsonObj = parse_doucment.object().value("result").toObject();
    QString contractAddr = jsonObj.value("id").toString();
    DataManagerStruct::ContractInfoPtr contractInfo = _p->contractData->getContractInfo(contractAddr);
    if(!contractInfo) return false;
    contractInfo->SetContractName(jsonObj.value("contract_name").toString());
    contractInfo->SetContractDes(jsonObj.value("description").toString());
    DataDefine::ApiEventPtr apis = contractInfo->GetInterface();

    QJsonArray apisArr = jsonObj.value("code_printable").toObject().value("abi").toArray();
    foreach (QJsonValue val, apisArr) {
        if(!val.isString()) continue;
        apis->addApi(val.toString());
    }
    QJsonArray offapisArr = jsonObj.value("code_printable").toObject().value("offline_abi").toArray();
    foreach (QJsonValue val, offapisArr) {
        if(!val.isString()) continue;
        apis->addOfflineApi(val.toString());
    }
    QJsonArray eventArr = jsonObj.value("code_printable").toObject().value("events").toArray();
    foreach (QJsonValue val, eventArr) {
        if(!val.isString()) continue;
        apis->addEvent(val.toString());
    }

    return true;
}
