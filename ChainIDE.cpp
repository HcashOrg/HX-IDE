#include "ChainIDE.h"

#include <QMutex>
#include <QMap>
#include <QSettings>
#include <QProcess>
#include <QCoreApplication>
#include <QDir>
#include <QDebug>
#include <QApplication>
#include <QTranslator>

#include "backstage/BackStageBase.h"
#include "backstage/LinkBackStage.h"
#include "backstage/UbtcBackStage.h"
#include "compile/CompileManager.h"

#include "DataManager.h"

static QMutex mutexForChainType;

class ChainIDE::DataPrivate
{
public:
    DataPrivate()
        :configFile(new QSettings( QCoreApplication::applicationDirPath() + QDir::separator() + "config.ini", QSettings::IniFormat))
        ,chainType(1)
        ,sanboxMode(false)
        ,testManager(new UbtcBackStage(1))
        ,formalManager(new UbtcBackStage(2))
        ,compileManager(new CompileManager())
        ,dataManager(new DataManager())
    {

    }
    ~DataPrivate()
    {
        delete configFile;
        delete testManager;
        delete formalManager;
        delete compileManager;
        delete dataManager;
    }

public:
    QSettings *configFile;//配置文件
    QString appDataPath;//系统环境变量的appdatapath
    BackStageBase *testManager;
    BackStageBase *formalManager;
    int chainType;//链类型1==测试 2==正式
    bool sanboxMode;//沙盒模式

    CompileManager *compileManager;//编译器
    DataManager *dataManager;
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
    mutexForChainType.lock();
    _p->chainType = type;
    mutexForChainType.unlock();
}

bool ChainIDE::isSandBoxMode() const
{
    return _p->sanboxMode;
}

void ChainIDE::setSandboxMode(bool mode)
{
    _p->sanboxMode = mode;
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

void ChainIDE::setCurrentTheme(DataDefine::ThemeStyle style)
{
    _p->configFile->setValue("/settings/theme",style == DataDefine::Black_Theme ? "black":"white");
}

DataDefine::Language ChainIDE::getCurrentLanguage() const
{
    return _p->configFile->value("/settings/language").toString() == "SimChinese"? DataDefine::SimChinese : DataDefine::English;
}

void ChainIDE::setCurrentLanguage(DataDefine::Language lan)
{
    _p->configFile->setValue("/settings/language",lan == DataDefine::SimChinese ? "SimChinese" : "English");
}

void ChainIDE::setConfigAppDataPath(const QString &path)
{
    _p->configFile->setValue("/settings/chainPath",path);
}

BackStageBase *ChainIDE::testManager() const
{
    return _p->testManager;
}

BackStageBase *ChainIDE::formalManager() const
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

DataManager *ChainIDE::getDataManager() const
{
    return _p->dataManager;
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

void ChainIDE::refreshTranslator()
{
    QTranslator*  translator = new QTranslator();
    translator->load(getCurrentLanguage() == DataDefine::English ? ":/IDE_English.qm" : ":/IDE_Simplified_Chinese.qm");
    QString a = getCurrentLanguage() == DataDefine::English ? ":/IDE_English.qm" : ":/IDE_Simplified_Chinese.qm";
    qDebug()<<a;
    QApplication::installTranslator(translator);
}

void ChainIDE::InitConfig()
{
    if("black" != _p->configFile->value("/settings/theme").toString() && "white" != _p->configFile->value("/settings/theme").toString())
    {

        _p->configFile->setValue("/settings/theme","black");
    }
}

void ChainIDE::InitExeManager()
{
    connect(this,&ChainIDE::rpcPosted,_p->testManager,&BackStageBase::rpcPostedSlot);
    connect(this,&ChainIDE::rpcPostedFormal,_p->formalManager,&BackStageBase::rpcPostedSlot);
}

void ChainIDE::getSystemEnvironmentPath()
{
    QString idePath = "/ChainIDE";

    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
#ifdef WIN32
    _p->appDataPath = env.value("APPDATA") + idePath;
#elif
    _p->appDataPath = env.valude("HOME") + idePath;
#endif
    qDebug() << "appDataPath:" << _p->appDataPath;
}
