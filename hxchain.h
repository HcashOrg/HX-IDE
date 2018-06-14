/*
                   _ooOoo_
                  o8888888o
                  88" . "88
                  (| -_- |)
                  O\  =  /O
               ____/`---'\____
             .'  \\|     |//  `.
            /  \\|||  :  |||//  \
           /  _||||| -:- |||||-  \
           |   | \\\  -  /// |   |
           | \_|  ''\---/''  |   |
           \  .-\__  `-`  ___/-. /
         ___`. .'  /--.--\  `. . __
      ."" '<  `.___\_<|>_/___.'  >'"".
     | | :  `- \`.;`\ _ /`;.`/ - ` : | |
     \  \ `-.   \_ __\ /__ _/   .-` /  /
======`-.____`-.___\_____/___.-`____.-'======
                   `=---='
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
         佛祖保佑       永无BUG
*/

#ifndef HXCHAIN_H
#define HXCHAIN_H
#include <QObject>
#include <QMap>
#include <QSettings>
#include <QFile>
#include <QProcess>
#include <QMutex>
#include <QDialog>
#include <QJsonArray>

#include "outputmessage.h"


#define HXCHAINIDE_VERSION "1.0.0"           // 版本号
#define AUTO_REFRESH_TIME 10000           // 自动刷新时间(ms)
//  密码输入错误5次后 锁定一段时间 (秒)
#define PWD_LOCK_TIME  7200
#define RPC_PORT 58110
#define BTN_SELECTED_STYLESHEET   "QPushButton{ font: 14px \"微软雅黑\";border: 0px solid #000000;border-bottom: 3px solid rgb(68,217,199);}"
#define BTN_UNSELECTED_STYLESHEET "QPushButton{ font: 14px \"微软雅黑\";border: 0px solid #000000;border-bottom: 3px solid rgb(233,233,240);}"

class QTimer;
class QFrame;
class WorkerThreadManager;

static QMutex mutexForJsonData;
static QMutex mutexForCurrentProcess;
static QMutex mutexForEditor;

struct FileRecord
{
    enum FileType{ CONTRACT_FILE = 1, SCRIPT_FILE};  // 是合约文件还是脚本文件
    FileType type;
    QString path;   // 绝对路径
    bool newBuilt = false;  // 是否为新建   保存后置为false
};

struct AccountInfo
{
    enum ChainType{ TestChain = 1, FormalChain};
    ChainType type;
    QString name;
    QString address;
    QString balance;
    QString registerTime;
    QString ownerKey;
    bool isDelegate = false;
};

struct ContractInfo
{
    QString address;
    QString name;
    QString level;
    QString owner;
    QString ownerAddress;
    QString ownerName;
    QString state;
    QString description;
    QStringList abiList;
    QStringList eventList;
};

struct ScriptInfo
{
    QString scriptId;
    QString description;
};

template <class key,class value>
key getKeyByValue(QMap<key,value> m,value v);
class ExeManager;
class HXChain : public QObject
{
    Q_OBJECT
public:
    ~HXChain();
    static HXChain*   getInstance();
    qint64 write(QString);
    void quit();
    QString read(QProcess* process);
    QString readAll(QProcess* process);
public slots:
    void updateJsonDataMap(QString id, QString data);
public:
    QString jsonDataValue(QString id);

    void initWorkerThreadManagerForTestChain();
    void initWorkerThreadManagerForFormalChain();
//    void destroyWorkerThreadManager();
    void postRPC(QString _rpcId, QString _rpcCmd, int type = 0);    // type: 0当前链  1测试链  2正式链
    QString getAccountNameByOwnerKey(QString ownerKey);
    QString changePathFormat(QString path);
    QString restorePathFormat(QString path);

    bool isContractFileRegistered(QString path);
    bool isContractFileUpgraded(QString path);
    bool isScriptAdded(QString path);

    bool isInContracts(QString filePath);   // 通过路径判断是否在contracts 文件夹下
    bool isInScripts(QString filePath);   // 通过路径判断是否在scripts 文件夹下


    QString configGetContractAddress(QString path);
    void configSetContractAddress(QString path, QString address);
    QString configRemoveContractAddress(QString address);  // 删除指定地址的合约记录 返回被删除的key值
    QString configGetScriptId(QString path);
    void configSetScriptId(QString path, QString scriptId);
    QStringList configGetAllScripts();
    QString configRemoveScriptId(QString scriptId);   // 删除指定ID的记录 返回被删除的key值

    void clearSandboxOperation();   // 退出沙盒后删除config 以及文件夹下的所有沙盒里操作的记录  恢复销毁的合约记录

    QProcess* currentProcess();

    void setCurrentAccount(QString accountName);

//    QProcess* proc;
    QProcess* testProcess;
    QProcess* formalProcess;
    int currenChain();         // 返回currenChainType
    void setCurrentChain(int type);

    QSettings *configFile;

    QString appDataPath;

//    int currentPort;          // 当前rpc 端口

    QString localIP;   // 保存 peerinfo 获得的本机IP和端口

    QMap<QString,FileRecord>   fileRecordMap;
    QMap<QString,AccountInfo>  accountInfoMap;
    QMap<QString,QString>      ownerKeyAccountNameMap;
    QMap<QString,ContractInfo> contractInfoMap;
    QMap<QString,ContractInfo> foreverContractInfoMap;  // 保存链上的永久合约信息
    QMap<QString,ScriptInfo>   scriptInfoMap;  // 保存钱包的脚本信息

    QStringList myContracts;    // 钱包账户的所有合约

    QString currentAccount;   // 当前 合约使用的账户

    bool isInSandBox;

    QMap<QString,QString>       specialOperationMap;    // 记录合约升级之类的操作 在自动刷新的时候去刷新信息

    ExeManager* testManager;
    ExeManager* formalManager;


signals:
    void started();

    void jsonDataUpdated(QString);

    void rpcPostedFormal(QString _rpcId, QString _rpcCmd);

    void rpcPosted(QString _rpcId, QString _rpcCmd);
private:

    HXChain();
    static HXChain* goo;
    QMap<QString,QString> jsonDataMap;   //  各指令的id,各指令的返回
    WorkerThreadManager* workerManager;  // 处理rpc worker thread
    WorkerThreadManager* workerManagerFormal;  // 处理rpc worker thread   正式链
    QThread* threadForWorkerManager;
    QThread* threadForWorkerManagerFormal;
    int currenChainType;       // 当前链是: 1测试链  2正式链

    void getSystemEnvironmentPath();

    class CGarbo // 它的唯一工作就是在析构函数中删除CSingleton的实例
    {
    public:
        ~CGarbo()
        {
            if (HXChain::goo)
                delete HXChain::goo;
        }
    };
    static CGarbo Garbo; // 定义一个静态成员，在程序结束时，系统会调用它的析构函数
};
#endif // HXCHAIN_H

QString doubleTo2Decimals(double number, bool rounding = false);
QString toJsonFormat(QString instruction, QJsonArray parameters);


