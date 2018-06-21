#include "ChainIDE.h"

#include <QMutex>
#include <QMap>
#include <QSettings>
#include <QProcess>
#include <QCoreApplication>
#include <QDir>
#include <QDebug>
#include <QApplication>

#include "ExeManager.h"
#include "compile/CompileManager.h"


static QMutex mutexForJsonData;

class ChainIDE::DataPrivate
{
public:
    DataPrivate()
        :configFile(new QSettings( QCoreApplication::applicationDirPath() + QDir::separator() + "config.ini", QSettings::IniFormat))
        ,chainType(1)
        ,testManager(new ExeManager(1))
        ,formalManager(new ExeManager(2))
        ,compileManager(new CompileManager())
    {

    }
public:
    QMap<QString,QString> jsonDataMap;   //  各指令的id,各指令的返回
    QSettings *configFile;//配置文件
    QString appDataPath;//系统环境变量的appdatapath
    ExeManager *testManager;
    ExeManager *formalManager;
    int chainType;

    CompileManager *compileManager;
};

ChainIDE *ChainIDE::getInstance()
{
    if(_instance == nullptr)
    {
        _instance = new ChainIDE();
    }
    return _instance;
}

ChainIDE::~ChainIDE()
{
    delete _p;
}

ChainIDE::ChainIDE(QObject *parent)
    : QObject(parent)
    ,_p(new DataPrivate())

{
    getSystemEnvironmentPath();
    InitConfig();
    InitExeManager();
}

ChainIDE * ChainIDE::_instance = nullptr;


void ChainIDE::updateJsonDataMap(QString id, QString data)
{
    mutexForJsonData.lock();

    _p->jsonDataMap.insert( id, data);
    emit jsonDataUpdated(id);

    mutexForJsonData.unlock();
}

QString ChainIDE::jsonDataValue(QString id)
{
    mutexForJsonData.lock();

    QString value = _p->jsonDataMap.value(id);

    mutexForJsonData.unlock();

    return value;
}

void ChainIDE::postRPC(QString _rpcId, QString _rpcCmd)
{
    switch (getCurrentChainType())
    {
    case 1:
        emit rpcPosted(_rpcId,_rpcCmd);
        break;
    case 2:
        emit rpcPostedFormal(_rpcId,_rpcCmd);
        break;
    default:
        break;
    }
}

int ChainIDE::getCurrentChainType() const
{
    return _p->chainType;
}

void ChainIDE::setCurrentChainType(int type)
{
    _p->chainType = type;
}

QString ChainIDE::getEnvAppDataPath() const
{
    return _p->appDataPath;
}

QString ChainIDE::getConfigAppDataPath() const
{
    return _p->configFile->value("/settings/chainPath").toString();
}

DataDefine::ThemeStyle ChainIDE::getCurrentTheme() const
{
    return _p->configFile->value("/settings/theme").toString() == "black"?DataDefine::Black_Theme : DataDefine::White_Theme;
}

void ChainIDE::setCurrentTheme(DataDefine::ThemeStyle style) const
{
    _p->configFile->setValue("/settings/theme",style == DataDefine::Black_Theme ? "black":"white");
}

void ChainIDE::setConfigAppDataPath(const QString &path)
{
    _p->configFile->setValue("/settings/chainPath",path);
}

ExeManager *ChainIDE::testManager() const
{
    return _p->testManager;
}

ExeManager *ChainIDE::formalManager() const
{
    return _p->formalManager;
}

QProcess *ChainIDE::getProcess(int type)const
{
    QProcess* p = NULL;
    if(type == 0)
    {
        if( getCurrentChainType() == 1)
        {
            p = _p->testManager->getProcess();
        }
        else if( getCurrentChainType() == 2)
        {
            p = _p->formalManager->getProcess();
        }
    }
    else if(type == 1)
    {
        p = _p->testManager->getProcess();
    }
    else if( type == 2)
    {
        p = _p->formalManager->getProcess();
    }

    return p;
}

CompileManager *ChainIDE::getCompileManager() const
{
    return _p->compileManager;
}

void ChainIDE::refreshStyleSheet()
{
    QString path ;
    if(DataDefine::Black_Theme == getCurrentTheme())
    {
        path = ":/qss/black_style.qss";
    }
    else if(DataDefine::White_Theme == getCurrentTheme())
    {
        path = ":/qss/white_style.qss";
    }


    QFile inputFile(path);
    inputFile.open(QIODevice::ReadOnly);
    QString css = inputFile.readAll();
    inputFile.close();
    qApp->setStyleSheet( css);
}

void ChainIDE::InitConfig()
{
    if("black" != _p->configFile->value("setting/theme").toString() && "white" != _p->configFile->value("setting/theme").toString())
    {
        _p->configFile->setValue("settings/theme","black");
    }
}

void ChainIDE::InitExeManager()
{
    connect(this,&ChainIDE::rpcPosted,_p->testManager,&ExeManager::rpcPostedSlot);
    connect(this,&ChainIDE::rpcPostedFormal,_p->formalManager,&ExeManager::rpcPostedSlot);
}

void ChainIDE::getSystemEnvironmentPath()
{
    QStringList environment = QProcess::systemEnvironment();
    QString str;

#ifdef WIN32
    foreach(str,environment)
    {
        if (str.startsWith("APPDATA="))
        {
            _p->appDataPath = str.mid(8) + "\\ChainIDE";
            qDebug() << "appDataPath:" << _p->appDataPath;
            break;
        }
    }
#elif defined(TARGET_OS_MAC)
    foreach(str,environment)
    {
        if (str.startsWith("HOME="))
        {
            _p->appDataPath = str.mid(5) + "/Library/Application Support/HXChainIDE";
            qDebug() << "appDataPath:" << _p->appDataPath;
            break;
        }
    }
#else
    foreach(str,environment)
    {
        if (str.startsWith("HOME="))
        {
            _p->appDataPath = str.mid(5) + "/HXChainIDE";
            qDebug() << "appDataPath:" << _p->appDataPath;
            break;
        }
    }
#endif
}
