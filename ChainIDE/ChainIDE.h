#ifndef CHAINIDE_H
#define CHAINIDE_H

#include <QObject>
#include "DataDefine.h"

class BackStageManager;
class CompileManager;
class QProcess;

//ide功能单例，工程中的配置、设置等全局功能通过本类提供，数据访问也通过本类中转（主要便于切换测试、正式链时数据访问通道修改）
class ChainIDE : public QObject
{
    Q_OBJECT
public:
//数据发送
    void postRPC(const QString &_rpcId, const QString &_rpcCmd);
//链类型
    DataDefine::ChainType getCurrentChainType()const;
    void setCurrentChainType(DataDefine::ChainType type);
//配置
    QString getEnvAppDataPath()const;

    QString getConfigAppDataPath()const;
    void setConfigAppDataPath(const QString &path);

    DataDefine::ThemeStyle getCurrentTheme()const;
    void setCurrentTheme(DataDefine::ThemeStyle);

    DataDefine::Language getCurrentLanguage()const;
    void setCurrentLanguage(DataDefine::Language);

    DataDefine::BlockChainClass getChainClass()const;
    void setChainClass(DataDefine::BlockChainClass);

    DataDefine::ChainTypes getStartChainTypes()const;
    void setStartChainTypes(DataDefine::ChainTypes ty);

//后台
    BackStageManager *const getBackStageManager()const;
//编译
    CompileManager *const getCompileManager()const;
public:
    void refreshStyleSheet();//刷新样式表
    void refreshTranslator();//刷新翻译
private:
    void getSystemEnvironmentPath();//系统环境变量寻找
    void InitConfig();//初始化配置
    void InitExeManager();//初始化后台
    void InitContractDir();//初始化合约目录
signals:
    void jsonDataUpdated(const QString &id,const QString &data);//接收到返回
public:
    static ChainIDE *getInstance();
    ~ChainIDE();
public:
    ChainIDE(const ChainIDE &) = delete;
    void operator =(const ChainIDE &) = delete;
private:
    explicit ChainIDE(QObject *parent = nullptr);
    static ChainIDE *_instance;
    class CGarbo // 它的唯一工作就是在析构函数中删除实例（利用在程序退出时，全局变量会被析构的特性）
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

#endif // CHAINIDE_H
