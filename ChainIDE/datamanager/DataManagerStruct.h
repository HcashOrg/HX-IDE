#ifndef DATAMANAGERSTRUCT_H
#define DATAMANAGERSTRUCT_H

#include <memory>
#include <mutex>
#include <vector>
#include <map>
#include <algorithm>

#include <QString>
#include "DataDefine.h"

namespace DataManagerStruct {

//本地合约类
    //合约最简信息
    class ContractInfo{
    public:
        ContractInfo():interface(std::make_shared<DataDefine::ApiEvent>()){}
    public:
        void SetContractAddr(const QString &addr){contractAddress = addr;}
        const QString &GetContractAddr()const{return contractAddress;}

        void SetContractName(const QString &name){contractName = name;}
        const QString &GetContractName()const{return contractName;}

        void SetContractDes(const QString &des){contractDes = des;}
        const QString &GetContractDes()const{return contractDes;}

        void SetInterface(const DataDefine::ApiEventPtr &data){interface = data;}
        DataDefine::ApiEventPtr GetInterface()const{return interface;}

    private:
        QString contractAddress;
        QString contractName;
        QString contractDes;
        DataDefine::ApiEventPtr interface;
    };
    typedef std::shared_ptr<ContractInfo> ContractInfoPtr;
    typedef std::vector<ContractInfoPtr> ContractInfoVec;
    //单个地址相关合约
    class AddressContract{
    public:
        void SetOwnerAddr(const QString &add){ownerAddr = add;}
        const QString &GetOwnerAddr()const{return ownerAddr;}

        const ContractInfoVec &GetContracts()const{return contracts;}

        void AddContract(const QString &contractaddr,const QString &contractname){//添加或者修改名称
            auto it = std::find_if(contracts.begin(),contracts.end(),[contractaddr](const ContractInfoPtr &info){
                return contractaddr == info->GetContractAddr();
            });
            if(it != contracts.end()){
                (*it)->SetContractName(contractname);
            }
            else{
                ContractInfoPtr cont = std::make_shared<ContractInfo>();
                cont->SetContractAddr(contractaddr);
                cont->SetContractName(contractname);
                contracts.push_back(cont);
            }
        }

        void DeleteContract(const QString &addrOrname){
            contracts.erase(std::remove_if(contracts.begin(),contracts.end(),[addrOrname](const ContractInfoPtr &info){
                                           return (addrOrname == info->GetContractName() || addrOrname == info->GetContractAddr());})
                            ,contracts.end());

        }

    private:
        QString ownerAddr;//地址或名称
        ContractInfoVec contracts;
    };
    typedef std::shared_ptr<AddressContract> AddressContractPtr;
    typedef std::vector<AddressContractPtr> AddressContractVec;
    //所有合约集合
    class AddressContractData{
    public:
        const AddressContractVec& getAllData()const{return data;}

        AddressContractPtr getAddressContract(const QString &owneraddr)const{
            auto it = std::find_if(data.begin(),data.end(),[owneraddr](const AddressContractPtr &cont){return owneraddr == cont->GetOwnerAddr();});
            if(it != data.end())
            {//说明已经有了拥有者
                return *it;
            }
            else
            {
                return nullptr;
            }
        }

        ContractInfoPtr getContractInfo(const QString &conAddressOrName)const{
            for(auto it = data.begin();it != data.end();++it)
            {
                for(auto iter = (*it)->GetContracts().begin();iter != (*it)->GetContracts().end();++iter)
                {
                    if((*iter)->GetContractAddr() == conAddressOrName || (*iter)->GetContractName() == conAddressOrName)
                    {
                        return *iter;
                    }
                }
            }
            return nullptr;
        }

        void AddContract(const QString &owneraddr,const QString &contractaddr,const QString &contractname = ""){
            std::lock_guard<std::mutex> lockguard(mutexLock);
            auto it = std::find_if(data.begin(),data.end(),[owneraddr](const AddressContractPtr &cont){return owneraddr == cont->GetOwnerAddr();});
            if(it != data.end())
            {//说明已经有了拥有者
                (*it)->AddContract(contractaddr,contractname);
            }
            else
            {//说明都是新建的
                AddressContractPtr cont = std::make_shared<AddressContract>();
                cont->SetOwnerAddr(owneraddr);
                cont->AddContract(contractaddr,contractname);
                data.push_back(cont);
            }
        }

        void DeleteContract(const QString &ownerOrcontract){
            std::lock_guard<std::mutex> lockguard(mutexLock);
            for(auto it = data.begin();it != data.end();)
            {
                if(ownerOrcontract == (*it)->GetOwnerAddr())
                {
                    it = data.erase(it);
                    break;
                }
                else
                {
                    (*it)->DeleteContract(ownerOrcontract);
                    if((*it)->GetContracts().empty())
                    {
                        it = data.erase(it);
                    }
                    else
                    {
                        ++it;
                    }
                }
            }
        }

        void clear(){data.clear();}
    private:
        AddressContractVec data;
        std::mutex mutexLock;//数据修改锁
    };
    typedef std::shared_ptr<AddressContractData> AddressContractDataPtr;

//账户类型
namespace AccountHX {
    //地址类
    class AssetInfo
    {
    public:
        void SetAssetID(const QString &id){ assetId = id;}
        const QString &GetAssetID()const{return assetId;}

        void SetAssetType(const QString &type){assetType = type;}
        const QString &GetAssetType()const{return assetType;}

        void SetPrecision(int pre){precision = pre;}
        int GetPrecision()const{return precision;}

        void SetBalance(double bal){balances = bal;}
        double GetBalance()const{return balances;}

    private:
        QString assetId;//资产类型id
        QString assetType;//资产类型名称
        int precision;//精度
        double balances;//资产余额
    };
    typedef std::shared_ptr<AssetInfo> AssetInfoPtr;
    //账户类
    class AccountInfo
    {
    public:
        void SetAccountName(const QString &name){accountName = name;}
        const QString &getAccountName()const{return accountName;}

        void SetAccountAddress(const QString &addr){accountAddress = addr;}
        const QString &GetAccountAddress()const{return accountAddress;}

        const std::vector<AssetInfoPtr> &getAssetInfos()const{return assets;}
    public:
        AssetInfoPtr getAssetInfoById(const QString &id)const{
            auto it = std::find_if(assets.begin(),assets.end(),[id](const AssetInfoPtr &info){
                return info->GetAssetID() == id;
            });
            if(it != assets.end())
            {
                return *it;
            }
            return nullptr;
        }

        AssetInfoPtr getAssetInfoByType(const QString &type)const{
            auto it = std::find_if(assets.begin(),assets.end(),[type](const AssetInfoPtr &info){
                return info->GetAssetType() == type;
            });
            if(it != assets.end())
            {
                return *it;
            }
            return nullptr;
        }

        bool insertAsset(const QString &id,double bal){
            if(getAssetInfoById(id)) return false;
            AssetInfoPtr asset = std::make_shared<AssetInfo>();
            asset->SetAssetID(id);
            asset->SetBalance(bal);
            assets.push_back(asset);
            return true;
        }
        //补全账户信息
        void makeUpInfo(const QString &id,const QString &type,int pre){
            AssetInfoPtr info = getAssetInfoById(id);
            if(!info) return;
            info->SetAssetType(type);
            info->SetPrecision(pre);
        }

    private:
        QString accountName;
        QString accountAddress;
        std::vector<AssetInfoPtr> assets;
    };
    typedef std::shared_ptr<AccountInfo> AccountInfoPtr;
    typedef std::vector<AccountInfoPtr> AccountInfoVec;
//总类
    class AccountData
    {
    public:
        const AccountInfoVec &getAccount()const{return data;}
    public:
        //根据账户名，获取账户信息
        AccountInfoPtr getAccountByName(const QString &name)const{
            auto it = std::find_if(data.begin(),data.end(),[name](const AccountInfoPtr &info){
                return info->getAccountName() == name;
            });
            if(it != data.end())
            {
                return *it;
            }
            return nullptr;
        }

        //根据地址，获取账户信息
        AccountInfoPtr getAccountByAddr(const QString &addr)const{
            auto it = std::find_if(data.begin(),data.end(),[addr](const AccountInfoPtr &info){
                return info->GetAccountAddress() == addr;
            });
            if(it != data.end())
            {
                return *it;
            }
            return nullptr;
        }

        //插入账户
        bool insertAccount(const QString &name,const QString &addr){
            if(getAccountByName(name))
            {
                return false;
            }
            AccountInfoPtr info = std::make_shared<AccountInfo>();
            info->SetAccountName(name);
            info->SetAccountAddress(addr);
            std::lock_guard<std::mutex> lockguard(dataMutex);
            data.push_back(info);
            return true;
        }
        //插入地址
        bool insertAsset(const QString &accountName,const QString &assetid,double bal){
            AccountInfoPtr account = getAccountByName(accountName);
            if(!account) return false;
            std::lock_guard<std::mutex> lockguard(dataMutex);
            return account->insertAsset(assetid,bal);
        }
        //补全信息
        void makeupInfo(const QString &id,const QString &type,int pre){
            for(auto it = data.begin();it != data.end();++it){
                std::lock_guard<std::mutex> lockguard(dataMutex);
                (*it)->makeUpInfo(id,type,pre);
            }
        }
        //清空内容
        void clear(){
            std::lock_guard<std::mutex> lockguard(dataMutex);
            data.clear();
        }
    private:
        AccountInfoVec data;
        std::mutex dataMutex;

    };
    typedef std::shared_ptr<AccountData> AccountDataPtr;
}

namespace AccountUB {
//账户类型
    //地址类
    class AddressInfo
    {
    public:
        AddressInfo():balances(0){}
    public:
        void SetAddress(const QString &add){ address = add;}
        const QString &GetAddress()const{return address;}

        void SetBalance(double bal){balances = bal;}
        void AddBalance(double bal){balances += bal;}
        double GetBalance()const{return balances;}
    private:
        QString address;
        double balances;
    };
    typedef std::shared_ptr<AddressInfo> AddressInfoPtr;
    //账户类
    class AccountInfo
    {
    public:
        void SetAccountName(const QString &name){accountName = name;}
        const QString &getAccountName()const{return accountName;}

        void SetAccountBalance(double bal){totalBalances = bal;}
        double getTotalBalance()const{return totalBalances;}

        const std::vector<AddressInfoPtr> &getAddressInfos()const{return addresses;}
    public:
        AddressInfoPtr getAddressInfoByAddr(const QString &addr)const{
            auto it = std::find_if(addresses.begin(),addresses.end(),[addr](const AddressInfoPtr &info){
                return info->GetAddress() == addr;
            });
            if(it != addresses.end())
            {
                return *it;
            }
            return nullptr;
        }

        bool insertAddress(const QString &addr,double bal){
            if(getAddressInfoByAddr(addr)) return false;
            AddressInfoPtr asset = std::make_shared<AddressInfo>();
            asset->SetAddress(addr);
            asset->SetBalance(bal);
            addresses.push_back(asset);
            return true;
        }
    private:
        QString accountName;
        double totalBalances;
        std::vector<AddressInfoPtr> addresses;
    };
    typedef std::shared_ptr<AccountInfo> AccountInfoPtr;
    typedef std::vector<AccountInfoPtr> AccountInfoVec;
//总类
    class AccountData
    {
    public:
        const AccountInfoVec &getAccount()const{return data;}
    public:
        //根据账户名，获取账户信息
        AccountInfoPtr getAccountByName(const QString &name)const{
            auto it = std::find_if(data.begin(),data.end(),[name](const AccountInfoPtr &info){
                return info->getAccountName() == name;
            });
            if(it != data.end())
            {
                return *it;
            }
            return nullptr;
        }
        //根据地址，获取地址信息
        AddressInfoPtr getAddressInfoByAddr(const QString &addr)const{
            for(auto it = data.begin();it != data.end();++it){
                if(AddressInfoPtr add =  (*it)->getAddressInfoByAddr(addr))
                {
                    return add;
                }
            }
            return nullptr;
        }
        //插入账户
        bool insertAccount(const QString &name,double balance){
            if(getAccountByName(name))
            {
                return false;
            }
            AccountInfoPtr info = std::make_shared<AccountInfo>();
            info->SetAccountName(name);
            info->SetAccountBalance(balance);
            std::lock_guard<std::mutex> lockguard(dataMutex);
            data.push_back(info);
            return true;
        }
        //插入地址
        bool insertAddress(const QString &accountName,const QString &addr,double bal){
            AccountInfoPtr account = getAccountByName(accountName);
            if(!account) return false;
            std::lock_guard<std::mutex> lockguard(dataMutex);
            return account->insertAddress(addr,bal);
        }
        //增加余额
        bool addAddressBalance(const QString &addr,double bal){
            if(AddressInfoPtr addInfo = getAddressInfoByAddr(addr))
            {
                std::lock_guard<std::mutex> lockguard(dataMutex);
                addInfo->AddBalance(bal);
                return true;
            }
            return false;
        }

        //清空内容
        void clear(){
            std::lock_guard<std::mutex> lockguard(dataMutex);
            data.clear();
        }
    private:
        AccountInfoVec data;
        std::mutex dataMutex;

    };
    typedef std::shared_ptr<AccountData> AccountDataPtr;
}

namespace AccountCTC {
//地址类
class AssetInfo
{
public:
    AssetInfo():assetId(0),precision(10000000),balances(0){}
public:
    void SetAssetID(int id){ assetId = id;}
    int GetAssetID()const{return assetId;}

    void SetAssetType(const QString &type){assetType = type;}
    const QString &GetAssetType()const{return assetType;}

    void SetPrecision(int pre){precision = pre;}
    int GetPrecision()const{return precision;}

    void SetBalance(double bal){balances = bal;}
    double GetBalance()const{return balances;}

private:
    int assetId;//资产类型id
    QString assetType;//资产类型名称
    int precision;//精度
    double balances;//资产余额
};
typedef std::shared_ptr<AssetInfo> AssetInfoPtr;
//账户类
class AccountInfo
{
public:
    void SetAccountName(const QString &name){accountName = name;}
    const QString &getAccountName()const{return accountName;}

    void SetAccountAddress(const QString &addr){accountAddress = addr;}
    const QString &GetAccountAddress()const{return accountAddress;}

    const std::vector<AssetInfoPtr> &getAssetInfos()const{return assets;}
public:
    AssetInfoPtr getAssetInfoById(int id)const{
        auto it = std::find_if(assets.begin(),assets.end(),[id](const AssetInfoPtr &info){
            return info->GetAssetID() == id;
        });
        if(it != assets.end())
        {
            return *it;
        }
        return nullptr;
    }

    AssetInfoPtr getAssetInfoByType(const QString &type)const{
        auto it = std::find_if(assets.begin(),assets.end(),[type](const AssetInfoPtr &info){
            return info->GetAssetType() == type;
        });
        if(it != assets.end())
        {
            return *it;
        }
        return nullptr;
    }

    bool insertAsset(int id,double bal){
        if(getAssetInfoById(id)) return false;
        AssetInfoPtr asset = std::make_shared<AssetInfo>();
        asset->SetAssetID(id);
        asset->SetBalance(bal);
        assets.push_back(asset);
        return true;
    }
    //补全账户信息
    void makeUpInfo(int id,const QString &type,int pre){
        AssetInfoPtr info = getAssetInfoById(id);
        if(!info) return;
        info->SetAssetType(type);
        info->SetPrecision(pre);
    }

private:
    QString accountName;
    QString accountAddress;
    std::vector<AssetInfoPtr> assets;
};
typedef std::shared_ptr<AccountInfo> AccountInfoPtr;
typedef std::vector<AccountInfoPtr> AccountInfoVec;
//总类
class AccountData
{
public:
    const AccountInfoVec &getAccount()const{return data;}
public:
    //根据账户名，获取账户信息
    AccountInfoPtr getAccountByName(const QString &name)const{
        auto it = std::find_if(data.begin(),data.end(),[name](const AccountInfoPtr &info){
            return info->getAccountName() == name;
        });
        if(it != data.end())
        {
            return *it;
        }
        return nullptr;
    }

    //根据地址，获取账户信息
    AccountInfoPtr getAccountByAddr(const QString &addr)const{
        auto it = std::find_if(data.begin(),data.end(),[addr](const AccountInfoPtr &info){
            return info->GetAccountAddress() == addr;
        });
        if(it != data.end())
        {
            return *it;
        }
        return nullptr;
    }

    //插入账户
    bool insertAccount(const QString &name,const QString &addr){
        if(getAccountByName(name))
        {
            return false;
        }
        AccountInfoPtr info = std::make_shared<AccountInfo>();
        info->SetAccountName(name);
        info->SetAccountAddress(addr);
        std::lock_guard<std::mutex> lockguard(dataMutex);
        data.push_back(info);
        return true;
    }
    //插入资产
    bool insertAsset(const QString &accountName,int assetid,double bal){
        AccountInfoPtr account = getAccountByName(accountName);
        if(!account) return false;
        std::lock_guard<std::mutex> lockguard(dataMutex);
        return account->insertAsset(assetid,bal);
    }
    //补全信息
    void makeupInfo(int id,const QString &type,int pre){
        for(auto it = data.begin();it != data.end();++it){
            std::lock_guard<std::mutex> lockguard(dataMutex);
            (*it)->makeUpInfo(id,type,pre);
        }
    }
    //清空内容
    void clear(){
        std::lock_guard<std::mutex> lockguard(dataMutex);
        data.clear();
    }
private:
    AccountInfoVec data;
    std::mutex dataMutex;

};
typedef std::shared_ptr<AccountData> AccountDataPtr;
}
}

#endif // DATAMANAGERSTRUCT_H
