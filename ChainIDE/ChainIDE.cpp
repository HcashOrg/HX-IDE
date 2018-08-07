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

static QMutex mutexForChainType;

class ChainIDE::DataPrivate
{
public:
    DataPrivate()
        :configFile(new QSettings( QCoreApplication::applicationDirPath() + QDir::separator() + "config.ini", QSettings::IniFormat))
        ,chainType(DataDefine::NONE)
        ,startChainTypes(DataDefine::TEST | DataDefine::FORMAL)
        ,chainClass(DataDefine::HX)
        ,themeStyle(DataDefine::Black_Theme)
        ,testManager(nullptr)
        ,formalManager(nullptr)
        ,compileManager(new CompileManager())
    {

    }
    ~DataPrivate()
    {
        if(configFile)
        {
            delete configFile;
            configFile = nullptr;
        }
        if(testManager)
        {
            delete testManager;
            testManager = nullptr;
        }
        if(formalManager)
        {
            delete formalManager;
            formalManager = nullptr;
        }
        if(compileManager)
        {
            delete compileManager;
            compileManager = nullptr;
        }
    }

public:
    QSettings *configFile;//配置文件
    QString appDataPath;//系统环境变量的appdatapath

    BackStageBase *testManager;
    BackStageBase *formalManager;

    DataDefine::ChainType chainType;//链类型1==测试 2==正式
    DataDefine::BlockChainClass chainClass;//链类ub hx等
    DataDefine::ThemeStyle themeStyle;//主题
    DataDefine::ChainTypes startChainTypes;//配置文件的启动链类型

    CompileManager *compileManager;//编译器
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
    qDebug()<<"delete chainide";
    delete _p;
    _p = nullptr;
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
ChainIDE::CGarbo ChainIDE::Garbo;

void ChainIDE::postRPC(QString _rpcId, QString _rpcCmd)
{
    switch (getCurrentChainType())
    {
    case DataDefine::TEST:
        emit rpcPosted(_rpcId,_rpcCmd);
        break;
    case DataDefine::FORMAL:
        emit rpcPostedFormal(_rpcId,_rpcCmd);
        break;
    default:
        break;
    }
}

DataDefine::ChainType ChainIDE::getCurrentChainType() const
{
    return _p->chainType;
}

void ChainIDE::setCurrentChainType(DataDefine::ChainType type)
{
    mutexForChainType.lock();
    _p->chainType = type;
    mutexForChainType.unlock();
}

QString ChainIDE::getEnvAppDataPath() const
{
    return _p->appDataPath;
}

QString ChainIDE::getConfigAppDataPath() const
{
    return _p->configFile->value("/settings/chainPath").toString();
}

void ChainIDE::setConfigAppDataPath(const QString &path)
{
    _p->configFile->setValue("/settings/chainPath",path);
}

DataDefine::ThemeStyle ChainIDE::getCurrentTheme() const
{
    return _p->themeStyle;
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

DataDefine::BlockChainClass ChainIDE::getChainClass() const
{
    return _p->chainClass;
}

void ChainIDE::setChainClass(DataDefine::BlockChainClass name)
{
    switch (name) {
    case DataDefine::HX:
        _p->configFile->setValue("/settings/chainClass","hx");
        break;
    case DataDefine::UB:
        _p->configFile->setValue("/settings/chainClass","ub");
        break;
    default:
        break;
    }
}

DataDefine::ChainTypes ChainIDE::getStartChainTypes() const
{
    return _p->startChainTypes;
}

void ChainIDE::setStartChainTypes(DataDefine::ChainTypes ty)
{
    if(ty == (DataDefine::TEST | DataDefine::FORMAL))
    {
        _p->configFile->setValue("/settings/starTypes","all");
    }
    else if(ty == DataDefine::NONE)
    {
        _p->configFile->setValue("/settings/starTypes","none");
    }
    else if(ty == DataDefine::TEST)
    {
        _p->configFile->setValue("/settings/starTypes","test");
    }
    else if(ty == DataDefine::FORMAL)
    {
        _p->configFile->setValue("/settings/starTypes","formal");
    }
}

BackStageBase *ChainIDE::testManager() const
{
    return _p->testManager;
}

BackStageBase *ChainIDE::formalManager() const
{
    return _p->formalManager;
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

void ChainIDE::refreshTranslator()
{
    QTranslator*  translator = new QTranslator();
    translator->load(getCurrentLanguage() == DataDefine::English ? ":/IDE_English.qm" : ":/IDE_Simplified_Chinese.qm");
    QApplication::installTranslator(translator);
}

void ChainIDE::startExe()
{
    //不启动
    if(getStartChainTypes() == DataDefine::NONE)
    {
        emit startExeFinish();
        return ;
    }

    //启动后台
    if(getStartChainTypes() & DataDefine::TEST)
    {
        connect(testManager(),&BackStageBase::exeStarted,this,&ChainIDE::exeStartedSlots);
        testManager()->startExe(getConfigAppDataPath());
    }
    if(getStartChainTypes() & DataDefine::FORMAL)
    {
        connect(formalManager(),&BackStageBase::exeStarted,this,&ChainIDE::exeStartedSlots);
        formalManager()->startExe(getConfigAppDataPath());
    }
}

void ChainIDE::exeStartedSlots()
{
    bool test = false;
    bool formal = false;
    if(getStartChainTypes() & DataDefine::TEST)
    {
        if(testManager()->exeRunning())
        {
            test = true;
            disconnect(testManager(),&BackStageBase::exeStarted,this,&ChainIDE::exeStartedSlots);
        }
    }
    else
    {
        test = true;
    }

    if(!test) return;

    if(getStartChainTypes() & DataDefine::FORMAL)
    {
        if(formalManager()->exeRunning())
        {
          formal = true;
          disconnect(formalManager(),&BackStageBase::exeStarted,this,&ChainIDE::exeStartedSlots);
        }
    }
    else
    {
        formal = true;
    }

    if(!formal) return;

    emit startExeFinish();
}

void ChainIDE::InitConfig()
{
    if("ub" != _p->configFile->value("/settings/chainClass").toString() &&
       "hx" != _p->configFile->value("/settings/chainClass").toString())
    {
        _p->configFile->setValue("/settings/chainClass","ub");
    }

    if("all" != _p->configFile->value("/settings/starTypes").toString() &&
       "test" != _p->configFile->value("/settings/starTypes").toString() &&
       "formal" != _p->configFile->value("/settings/starTypes").toString() &&
       "none" != _p->configFile->value("/settings/starTypes").toString() )
    {
        _p->configFile->setValue("/settings/starTypes","test");
    }

    if("black" != _p->configFile->value("/settings/theme").toString() &&
       "white" != _p->configFile->value("/settings/theme").toString())
    {
        _p->configFile->setValue("/settings/theme","black");
    }

    if("SimChinese" != _p->configFile->value("/settings/language").toString() &&
       "English" != _p->configFile->value("/settings/language").toString())
    {
        _p->configFile->setValue("/settings/language","English");
    }


    //链类型设置
    if(_p->configFile->value("/settings/chainClass").toString() == "hx")
    {
        _p->chainClass =  DataDefine::HX;
    }
    else if(_p->configFile->value("/settings/chainClass").toString() == "ub")
    {
        _p->chainClass =  DataDefine::UB;
    }

    //启动类型设置
    if(_p->configFile->value("/settings/starTypes").toString() == "all")
    {
        _p->startChainTypes =  DataDefine::TEST | DataDefine::FORMAL;
        _p->chainType = DataDefine::TEST;
    }
    else if(_p->configFile->value("/settings/starTypes").toString() == "test")
    {
        _p->startChainTypes =  DataDefine::TEST;
        _p->chainType = DataDefine::TEST;
    }
    if(_p->configFile->value("/settings/starTypes").toString() == "formal")
    {
        _p->startChainTypes =  DataDefine::FORMAL;
        _p->chainType = DataDefine::FORMAL;
    }
    else if(_p->configFile->value("/settings/starTypes").toString() == "none")
    {
        _p->startChainTypes =  DataDefine::NONE;
        _p->chainType = DataDefine::NONE;
    }

    //主题类型
    if(_p->configFile->value("/settings/theme").toString() == "black")
    {
        _p->themeStyle =  DataDefine::Black_Theme;
    }
    else if(_p->configFile->value("/settings/theme").toString() == "white")
    {
        _p->themeStyle =  DataDefine::White_Theme;
    }
}

void ChainIDE::InitExeManager()
{
    if(getChainClass() == DataDefine::HX)
    {
        if(getStartChainTypes() & DataDefine::TEST)
        {
            _p->testManager = new LinkBackStage(1);
        }
        if(getStartChainTypes() & DataDefine::FORMAL)
        {
            _p->formalManager = new LinkBackStage(2);
        }
    }
    else if(getChainClass() == DataDefine::UB)
    {
        if(getStartChainTypes() & DataDefine::TEST)
        {
            _p->testManager = new UbtcBackStage(1);
        }
        if(getStartChainTypes() & DataDefine::FORMAL)
        {
            _p->formalManager = new UbtcBackStage(2);
        }
    }

    if(_p->testManager)
    {
        connect(this,&ChainIDE::rpcPosted,_p->testManager,&BackStageBase::rpcPostedSlot);
        connect(_p->testManager,&BackStageBase::rpcReceived,this,&ChainIDE::jsonDataUpdated);
    }
    if(_p->formalManager)
    {
        connect(this,&ChainIDE::rpcPostedFormal,_p->formalManager,&BackStageBase::rpcPostedSlot);
        connect(_p->formalManager,&BackStageBase::rpcReceived,this,&ChainIDE::jsonDataUpdated);
    }
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
    //qDebug() << "appDataPath:" << _p->appDataPath;
}
