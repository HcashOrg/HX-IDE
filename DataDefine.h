#ifndef DATADEFINE_H
#define DATADEFINE_H

#include <memory>
#include <mutex>
#include <vector>
#include <map>
#include <algorithm>

#include <QString>
#include <QStringList>
#include <QColor>
#include <QMap>
#include <QDebug>
namespace DataDefine
{
    //主题类型
    enum ThemeStyle{Black_Theme,White_Theme};
    //语言类型
    enum Language{SimChinese,English};

    static const QColor BLACK_BACKGROUND = QColor(41,41,41);
    static const QColor DARK_CLACK_BACKGROUND = QColor(30,30,30);
    static const QColor WHITE_BACKGROUND = QColor(255,255,255);
    static const QColor GRAY_COLOR = Qt::gray;

    //合约文件后缀
    static const QString META_SUFFIX = "meta.json";
    static const QString CONTRACT_SUFFIX = "gpc";
    static const QString GLUA_SUFFIX = "uvlua";
    static const QString JAVA_SUFFIX = "java";
    static const QString CSHARP_SUFFIX = "cs";
    static const QString KOTLIN_SUFFIX = "kt";

    //编辑器后缀转换
    static const std::map<QString,QString> SUFFIX_MAP= {{CONTRACT_SUFFIX,"text"},{GLUA_SUFFIX,"glua"},
                                                        {JAVA_SUFFIX,"java"},{CSHARP_SUFFIX,"csharp"},{KOTLIN_SUFFIX,"kotlin"}};

    //后台程序路径
    static const QString UBCD_NODE_EXE = "backstage/ubcd.exe";
    static const QString UBCD_CLIENT_EXE = "backstage/ubc-cli.exe";
    static const QString LINK_NODE_EXE = "backstage/lnk_node.exe";
    static const QString LINK_CLIENT_EXE = "backstage/lnk_client.exe";

    //合约存放路径
    static const QString CONTRACT_DIR = "contracts";
    static const QString GLUA_DIR = "contracts/uvlua";
    static const QString JAVA_DIR = "contracts/java";
    static const QString CSHARP_DIR = "contracts/csharp";
    static const QString KOTLIN_DIR = "contracts/kotlin";

    static const QString LOCAL_CONTRACT_PATH = "contracts/contracts.contract";


    //编译工具路径
    static const QString GLUA_COMPILE_PATH = "compile/glua/glua_compiler.exe";

    static const QString JAVA_CORE_PATH = "compile/java/gjavac-core-1.0.1-dev.jar";
    static const QString JAVA_COMPILE_PATH = "compile/java/gjavac-compiler-1.0.1-dev-jar-with-dependencies.jar";
    static const QString JAVA_UVM_ASS_PATH = "compile/java/uvm_ass.exe";
    static const QString JAVA_PACKAGE_GPC_PATH = "compile/java/package_gpc.exe";

    static const QString CSHARP_COMPILER_EXE_ENV = "CSC";//C#编译器环境变量名称
    static const QString CSHARP_COMPILE_DIR = "compile/csharp";
    static const QString CSHARP_JSON_DLL_PATH = "compile/csharp/Newtonsoft.Json.dll";
    static const QString CSHARP_CORE_DLL_PATH = "compile/csharp/GluaCoreLib.dll";
    static const QString CSHARP_COMPILE_PATH = "compile/csharp/gsharpc.exe";

    static const QString KOTLIN_COMPILE_PATH = "compile/kotlin/kotlinc/bin/kotlinc-jvm.bat";

    //编译临时路径(文件夹)
    static const QString GLUA_COMPILE_TEMP_DIR = "gluaTemp";
    static const QString JAVA_COMPILE_TEMP_DIR = "javaTemp";
    static const QString CSHARP_COMPILE_TEMP_DIR = "csharpTemp";
    static const QString KOTLIN_COMPILE_TEMP_DIR = "kotlinTemp";

//合约api类
    class ApiEvent
    {
    public:
        ApiEvent(){

        }
        void addApi(const QString &name){
            std::lock_guard<std::mutex> lock(lockMutex);
            apis<<name;
        }
        void addEvent(const QString &name){
            std::lock_guard<std::mutex> lock(lockMutex);
            events<<name;
        }

        const QStringList &getAllApiName()const{
            return apis;
        }
        const QStringList &getAllEventName()const{
            return events;
        }
        void clear(){
            std::lock_guard<std::mutex> lock(lockMutex);
            apis.clear();
            events.clear();
        }

    private:
        QStringList apis;
        QStringList events;
        std::mutex lockMutex;

    };

//账户类型
    //地址类
    class AddressInfo
    {
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
            data.push_back(info);
            return true;
        }
        //插入地址
        bool insertAddress(const QString &accountName,const QString &addr,double bal){
            AccountInfoPtr account = getAccountByName(accountName);
            if(!account) return false;
            return account->insertAddress(addr,bal);
        }
        //增加余额
        bool addAddressBalance(const QString &addr,double bal){
            if(AddressInfoPtr addInfo = getAddressInfoByAddr(addr))
            {
                addInfo->AddBalance(bal);
                return true;
            }
            return false;
        }

        //清空内容
        void clear(){data.clear();}
    private:
        AccountInfoVec data;

    };
    typedef std::shared_ptr<AccountData> AccountDataPtr;

//本地合约类
    //合约最简信息
    class ContractInfo{
    public:
        void SetContractAddr(const QString &addr){contractAddress = addr;}
        const QString &GetContractAddr()const{return contractAddress;}

        void SetContractName(const QString &name){contractName = name;}
        const QString &GetContractName()const{return contractName;}
    private:
        QString contractAddress;
        QString contractName;
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
        QString ownerAddr;
        ContractInfoVec contracts;
    };
    typedef std::shared_ptr<AddressContract> AddressContractPtr;
    typedef std::vector<AddressContractPtr> AddressContractVec;
    //所有合约集合
    class AddressContractData{
    public:
        const AddressContractVec& getAllData()const{return data;}

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

}

#endif // DATADEFINE_H
