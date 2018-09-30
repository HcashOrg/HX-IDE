#include "ChainIDE.h"

#include <mutex>
#include <QSettings>
#include <QProcess>
#include <QCoreApplication>
#include <QDir>
#include <QDebug>
#include <QApplication>
#include <QTranslator>

#include "compile/CompileManager.h"
#include "backstage/BackStageManager.h"
#include "debugwidget/DebugManager.h"

static std::mutex dataMutex;
class ChainIDE::DataPrivate
{
public:
    DataPrivate()
        :configFile(new QSettings( QCoreApplication::applicationDirPath() + QDir::separator() + "config.ini", QSettings::IniFormat))
        ,chainType(DataDefine::NONE)
        ,startChainTypes(DataDefine::TEST | DataDefine::FORMAL)
        ,chainClass(DataDefine::HX)
        ,themeStyle(DataDefine::Black_Theme)
        ,backStageManager(nullptr)
        ,compileManager(new CompileManager())
        ,debugManager(new DebugManager())
    {

    }
    ~DataPrivate()
    {
        if(configFile)
        {
            delete configFile;
            configFile = nullptr;
        }
        if(backStageManager)
        {
            delete backStageManager;
            backStageManager = nullptr;
        }
        if(compileManager)
        {
            delete compileManager;
            compileManager = nullptr;
        }
        if(debugManager)
        {
            delete debugManager;
            debugManager = nullptr;
        }
    }

public:
    QSettings *configFile;//配置文件
    QString appDataPath;//系统环境变量的appdatapath

    DataDefine::ChainType chainType;//链类型1==测试 2==正式
    DataDefine::BlockChainClass chainClass;//链类ub hx等
    DataDefine::ThemeStyle themeStyle;//主题
    DataDefine::ChainTypes startChainTypes;//配置文件的启动链类型

    CompileManager *compileManager;//编译器
    BackStageManager *backStageManager;//后台
    DebugManager *debugManager;//调试器
};

ChainIDE *ChainIDE::getInstance()
{
    if(nullptr == _instance)
    {
        std::lock_guard<std::mutex> loc(dataMutex);
        if(nullptr == _instance)
        {
            _instance = new ChainIDE();
        }
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
    //初始化合约目录，确保存在
    InitContractDir();
}

ChainIDE * ChainIDE::_instance = nullptr;
ChainIDE::CGarbo ChainIDE::Garbo;

void ChainIDE::postRPC(const QString &_rpcId, const QString &_rpcCmd)
{
    if(_p->backStageManager)
    {
        _p->backStageManager->postRPC(_rpcId,_rpcCmd,getCurrentChainType());
    }
}

DataDefine::ChainType ChainIDE::getCurrentChainType() const
{
    return _p->chainType;
}

void ChainIDE::setCurrentChainType(DataDefine::ChainType type)
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
    case DataDefine::CTC:
        _p->configFile->setValue("/settings/chainClass","ctc");
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

BackStageManager * const ChainIDE::getBackStageManager() const
{
    return _p->backStageManager;
}

CompileManager *const ChainIDE::getCompileManager() const
{
    return _p->compileManager;
}

DebugManager * const ChainIDE::getDebugManager() const
{
    return _p->debugManager;
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

    QTranslator*  widgetTranslator = new QTranslator();
    widgetTranslator->load(getCurrentLanguage() == DataDefine::English ? ":/widgets_English.qm" : ":/widgets_Chinese.qm");
    QApplication::installTranslator(widgetTranslator);


}

void ChainIDE::InitConfig()
{
    if("ub" != _p->configFile->value("/settings/chainClass").toString() &&
       "hx" != _p->configFile->value("/settings/chainClass").toString() &&
       "ctc" != _p->configFile->value("/settings/chainClass").toString())
    {
        _p->configFile->setValue("/settings/chainClass","hx");
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
    else if(_p->configFile->value("/settings/chainClass").toString() == "ctc")
    {
        _p->chainClass =  DataDefine::CTC;
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
    _p->backStageManager = new BackStageManager(getChainClass(),getStartChainTypes(),getConfigAppDataPath());
    connect(_p->backStageManager,&BackStageManager::jsonDataUpdated,this,&ChainIDE::jsonDataUpdated);
}

void ChainIDE::InitContractDir()
{
    //确保合约路径存在
    QDir dir(QCoreApplication::applicationDirPath() + QDir::separator() + DataDefine::CONTRACT_DIR);
    if(!dir.exists())
    {
        dir.mkpath(QCoreApplication::applicationDirPath() + QDir::separator() + DataDefine::CONTRACT_DIR);
    }
    dir.setFilter(QDir::Dirs);
    QStringList list = dir.entryList();
    if(!list.contains(dir.absolutePath() + QDir::separator() + DataDefine::KOTLIN_DIR))
    {
        dir.mkpath(QCoreApplication::applicationDirPath() + QDir::separator() + DataDefine::KOTLIN_DIR);
    }
    if(!list.contains(dir.absolutePath() + QDir::separator() + DataDefine::CSHARP_DIR))
    {
        dir.mkpath(QCoreApplication::applicationDirPath() + QDir::separator() + DataDefine::CSHARP_DIR);
    }
    if(!list.contains(dir.absolutePath() + QDir::separator() + DataDefine::JAVA_DIR))
    {
        dir.mkpath(QCoreApplication::applicationDirPath() + QDir::separator() + DataDefine::JAVA_DIR);
    }
    if(!list.contains(dir.absolutePath() + QDir::separator() + DataDefine::GLUA_DIR))
    {
        dir.mkpath(QCoreApplication::applicationDirPath() + QDir::separator() + DataDefine::GLUA_DIR);
    }
}

void ChainIDE::getSystemEnvironmentPath()
{
    QString idePath = "/ChainIDE";

    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
#ifdef WIN32
    _p->appDataPath = env.value("APPDATA") + idePath;
#else
    _p->appDataPath = env.valude("HOME") + idePath;
#endif
    //qDebug() << "appDataPath:" << _p->appDataPath;
}
