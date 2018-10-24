#include "DataManagerHX.h"

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

class DataManagerHX::DataPrivate
{
public:
    DataPrivate()
        :accountData(std::make_shared<AccountHX::AccountData>())
        ,contractData(std::make_shared<AddressContractData>())
    {

    }
public:
    DataManagerStruct::AccountHX::AccountDataPtr accountData;
    DataManagerStruct::AddressContractDataPtr contractData;
};

DataManagerHX *DataManagerHX::getInstance()
{
    if(_instance == nullptr)
    {
        _instance = new DataManagerHX();
    }
    return _instance;
}

DataManagerHX::DataManagerHX(QObject *parent)
    : QObject(parent)
    ,_p(new DataPrivate())
{

}
DataManagerHX * DataManagerHX::_instance = nullptr;
DataManagerHX::CGarbo DataManagerHX::Garbo;

DataManagerHX::~DataManagerHX()
{
    qDebug()<<"delete DataManagerHX";
    delete _p;
    _p = nullptr;
}

void DataManagerHX::queryAccount()
{
    _p->accountData->clear();

    ChainIDE::getInstance()->postRPC("query-listaccounts",IDEUtil::toJsonFormat("list_my_accounts",QJsonArray()));
}

const DataManagerStruct::AccountHX::AccountDataPtr &DataManagerHX::getAccount() const
{
    return _p->accountData;
}

void DataManagerHX::queryContract()
{
    _p->contractData->clear();

    for(auto it = _p->accountData->getAccount().begin();it != _p->accountData->getAccount().end();++it)
    {
        ChainIDE::getInstance()->postRPC("query-get_contract_"+(*it)->getAccountName(),IDEUtil::toJsonFormat("get_contract_addresses_by_owner",
                                                              QJsonArray()<<(*it)->getAccountName()));
    }
    ChainIDE::getInstance()->postRPC("query-getcontract-finish",IDEUtil::toJsonFormat("finishquery",QJsonArray()));
}

const AddressContractDataPtr &DataManagerHX::getContract() const
{
    return _p->contractData;
}

void DataManagerHX::dealNewState()
{
    ChainIDE::getInstance()->postRPC("deal-is_new",IDEUtil::toJsonFormat("is_new",QJsonArray()));
}

void DataManagerHX::unlockWallet(const QString &password)
{
    ChainIDE::getInstance()->postRPC( "deal_unlock-lockpage", IDEUtil::toJsonFormat( "unlock", QJsonArray() << password ));
}

void DataManagerHX::InitManager()
{
    connect(ChainIDE::getInstance(),&ChainIDE::jsonDataUpdated,this,&DataManagerHX::jsonDataUpdated);
}

void DataManagerHX::jsonDataUpdated(const QString &id, const QString &data)
{
    if("query-listaccounts" == id)
    {
        if(parseListAccount(data))
        {
            //查询每一个账户对应的地址
            AccountHX::AccountInfoVec accounts = _p->accountData->getAccount();
            std::for_each(accounts.begin(),accounts.end(),[](const AccountHX::AccountInfoPtr &account){
                ChainIDE::getInstance()->postRPC(QString("query-getassetbyaccount_%1").arg(account->getAccountName()),
                                                 IDEUtil::toJsonFormat("get_account_balances",
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
        //查询资产信息
        ChainIDE::getInstance()->postRPC("query-list_assets",IDEUtil::toJsonFormat("list_assets",QJsonArray()<<"A"<<100));
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
        QJsonDocument parse_doucment = QJsonDocument::fromJson(data.toUtf8(),&json_error);
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
        //直接解锁
        ChainIDE::getInstance()->postRPC("deal-unlockchain",IDEUtil::toJsonFormat("unlock",QJsonArray()<<"11111111"));
    }
    //处理查询结果
    else if(id.startsWith("query-get_contract_"))
    {
        QString accountName = id.mid(QString("query-get_contract_").length());
        parseContract(accountName,data);
    }
    else if("query-getcontract-finish" == id)
    {
        std::for_each(_p->contractData->getAllData().begin(),_p->contractData->getAllData().end(),[](const DataManagerStruct::AddressContractPtr& data){
            std::for_each(data->GetContracts().begin(),data->GetContracts().end(),[](const DataManagerStruct::ContractInfoPtr &info){
                ChainIDE::getInstance()->postRPC("query-contractinfo_"+info->GetContractAddr(),IDEUtil::toJsonFormat("get_simple_contract_info",QJsonArray()<<info->GetContractAddr()));
            });
        });
        ChainIDE::getInstance()->postRPC("query-getcontract-info-finish",IDEUtil::toJsonFormat("finishquery",QJsonArray()));
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
}

bool DataManagerHX::parseListAccount(const QString &data)
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
        _p->accountData->insertAccount(obj.value("name").toString(),obj.value("addr").toString());
    }
    return true;
}

bool DataManagerHX::parseAddresses(const QString &accountName,const QString &data)
{
//    qDebug()<<"query getaddressesbyaccount"<<data << "accountname"<<accountName;
    QJsonParseError json_error;
    QJsonDocument parse_doucment = QJsonDocument::fromJson(data.toUtf8(),&json_error);
    if(json_error.error != QJsonParseError::NoError || !parse_doucment.isObject())
    {
         qDebug()<<json_error.errorString();
         return false;
    }
    QJsonArray jsonArr = parse_doucment.object().value("result").toArray();

    foreach (QJsonValue addr, jsonArr) {
        if(!addr.isObject()) continue;
        QJsonObject obj = addr.toObject();
        if(obj.value("amount").isDouble())
        {
            _p->accountData->insertAsset(accountName,obj.value("asset_id").toString(),obj.value("amount").toDouble());
        }
        else if(obj.value("amount").isString())
        {
            _p->accountData->insertAsset(accountName,obj.value("asset_id").toString(),obj.value("amount").toString().toDouble());
        }
    }
    return true;

}

bool DataManagerHX::parseAddressBalances(const QString &data)
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
        _p->accountData->makeupInfo(obj.value("id").toString(),obj.value("symbol").toString(),obj.value("precision").toInt());
    }
    return true;
}

bool DataManagerHX::parseContract(const QString &accountName,const QString &data)
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

bool DataManagerHX::parseContractInfo(const QString &contaddr,const QString &data)
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
    contractInfo->SetContractName(jsonObj.value("name").toString());
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



