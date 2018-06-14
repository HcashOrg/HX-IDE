#include "hxchain.h"
#include "workerthreadmanager.h"
#include "sandboxcommandmap.h"

#include "ExeManager.h"

#include <QTextCodec>
#include <QDebug>
#include <Windows.h>
#include <QTimer>
#include <QThread>
#include <QApplication>
#include <QDir>
#include <QJsonObject>
#include <QJsonDocument>


HXChain* HXChain::goo = 0;

HXChain::HXChain()
{
//    proc = new QProcess;
    currenChainType = 1;
    testProcess = new QProcess;
    formalProcess = new QProcess;

    testManager = new ExeManager(1);
    connect(this,&HXChain::rpcPosted,testManager,&ExeManager::rpcPostedSlot);
    formalManager = new ExeManager(2);
    connect(this,&HXChain::rpcPostedFormal,formalManager,&ExeManager::rpcPostedSlot);


    workerManager = NULL;

    getSystemEnvironmentPath();

    threadForWorkerManager = NULL;

//    currentPort = RPC_PORT;

    configFile = new QSettings("config.ini", QSettings::IniFormat);
    currentAccount = configFile->value("/settings/testCurrentAccount").toString();

    QFile file("log.txt");       // 每次启动清空 log.txt文件
    file.open(QIODevice::Truncate | QIODevice::WriteOnly);
    file.close();

    initSandBoxCommandMap();
    isInSandBox = false;

}

HXChain::~HXChain()
{
	if (configFile)
	{
		delete configFile;
		configFile = NULL;
	}

    if( threadForWorkerManager)
    {
        delete threadForWorkerManager;
        threadForWorkerManager = NULL;
    }

    if( workerManager)
    {
        delete workerManager;
        workerManager = NULL;
    }

    if( threadForWorkerManagerFormal)
    {
        delete threadForWorkerManagerFormal;
        threadForWorkerManagerFormal = NULL;
    }

    if( workerManagerFormal)
    {
        delete workerManagerFormal;
        workerManagerFormal = NULL;
    }

    if( testManager)
    {
        delete testManager;
        testManager = NULL;
    }
    if(formalManager)
    {
        delete formalManager;
        formalManager = NULL;
    }

}

HXChain*   HXChain::getInstance()
{
    if( goo == NULL)
    {
        goo = new HXChain;
    }
    return goo;
}


qint64 HXChain::write(QString cmd)
{
    QTextCodec* gbkCodec = QTextCodec::codecForName("GBK");
    QByteArray cmdBa = gbkCodec->fromUnicode(cmd);  // 转为gbk的bytearray
    currentProcess()->readAll();
    return currentProcess()->write(cmdBa.data());
}

QString HXChain::read(QProcess* process)
{
    QTextCodec* gbkCodec = QTextCodec::codecForName("GBK");
    QString result;
    QString str;
    QApplication::setOverrideCursor(QCursor(Qt::BusyCursor));
    while ( !result.contains(">>>"))        // 确保读取全部输出
    {
        process->waitForReadyRead(50);
        str = gbkCodec->toUnicode(process->readAll());
        result += str;
        if( str.right(2) == ": " )  break;

        //  手动调用处理未处理的事件，防止界面阻塞
//        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    }

    QApplication::restoreOverrideCursor();
    return result;

}

QString HXChain::readAll(QProcess *process)
{
    QTextCodec* gbkCodec = QTextCodec::codecForName("GBK");
    QString result = gbkCodec->toUnicode(process->readAll());

    return result;
}

void HXChain::getSystemEnvironmentPath()
{
    QStringList environment = QProcess::systemEnvironment();
    QString str;

#ifdef WIN32
    foreach(str,environment)
    {
        if (str.startsWith("APPDATA="))
        {
            appDataPath = str.mid(8) + "\\HXChainIDE";
            qDebug() << "appDataPath:" << appDataPath;
            break;
        }
    }
#elif defined(TARGET_OS_MAC)
    foreach(str,environment)
    {
        if (str.startsWith("HOME="))
        {
            appDataPath = str.mid(5) + "/Library/Application Support/HXChainIDE";
            qDebug() << "appDataPath:" << appDataPath;
            break;
        }
    }
#else
    foreach(str,environment)
    {
        if (str.startsWith("HOME="))
        {
            appDataPath = str.mid(5) + "/HXChainIDE";
            qDebug() << "appDataPath:" << appDataPath;
            break;
        }
    }
#endif
}

void HXChain::quit()
{
    if (testProcess)
    {
        testProcess->close();
        qDebug() << "testProcess: close";
        delete testProcess;
        testProcess = NULL;
    }

    if (formalProcess)
    {
        formalProcess->close();
        qDebug() << "formalProcess: close";
        delete formalProcess;
        formalProcess = NULL;
    }
}

void HXChain::updateJsonDataMap(QString id, QString data)
{
    mutexForJsonData.lock();

    jsonDataMap.insert( id, data);
    emit jsonDataUpdated(id);

    mutexForJsonData.unlock();

}

QString HXChain::jsonDataValue(QString id)
{

    mutexForJsonData.lock();

    QString value = jsonDataMap.value(id);

    mutexForJsonData.unlock();

    return value;
}

QString doubleTo2Decimals(double number, bool rounding)  // 5位小数去掉后三位小数, rounding为false舍尾，为true进1
{
    if( !rounding)
    {
        QString num = QString::number(number,'f', 5);
        int pos = num.indexOf('.') + 3;
        return num.mid(0,pos);
    }
    else
    {
        QString num = QString::number(number,'f', 5);
        int pos = num.indexOf('.') + 3;
        QString round = num.mid(0,pos);
        double sub = num.toDouble() - round.toDouble();
        if( sub > 0.000001 )
        {
            return QString::number(round.toDouble() + 0.01,'f', 2);
        }
        else
        {
            return round;
        }

    }

}


void HXChain::initWorkerThreadManagerForTestChain()
{
    qDebug() << "initWorkerThreadManagerForTestChain " << QThread::currentThreadId();
//    if( workerManager)
//    {
//        delete workerManager;
//    }
//    if( threadForWorkerManager)
//    {
//        delete threadForWorkerManager;
//    }

    threadForWorkerManager = new QThread;
    threadForWorkerManager->start();
    workerManager = new WorkerThreadManager(RPC_PORT + 1);
    workerManager->moveToThread(threadForWorkerManager);
    connect(this, SIGNAL(rpcPosted(QString)), workerManager, SLOT(processRPCs(QString)));
}

void HXChain::initWorkerThreadManagerForFormalChain()
{
    qDebug() << "initWorkerThreadManagerForFormalChain " << QThread::currentThreadId();

    threadForWorkerManagerFormal = new QThread;
    threadForWorkerManagerFormal->start();
    workerManagerFormal = new WorkerThreadManager(RPC_PORT);
    workerManagerFormal->moveToThread(threadForWorkerManagerFormal);
    connect(this, SIGNAL(rpcPostedFormal(QString)), workerManagerFormal, SLOT(processRPCs(QString)));
}

//void HXChain::destroyWorkerThreadManager()
//{
//    qDebug() << "destroyWorkerThreadManager " << QThread::currentThreadId();
//    if( workerManager)
//    {
//        workerManager->deleteLater();
//        workerManager = NULL;

//        if( threadForWorkerManager)
//        {
//            threadForWorkerManager->deleteLater();
//            threadForWorkerManager = NULL;
//        }
//    }

//}

void HXChain::postRPC(QString _rpcId, QString _rpcCmd, int type)
{  
    switch (type)
    {
    case 0:
        if( currenChain() == 1)
        {
            emit rpcPosted(_rpcId,_rpcCmd);
        }
        else if( currenChain() == 2)
        {
            emit rpcPostedFormal(_rpcId,_rpcCmd);
        }

        break;
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

QString HXChain::getAccountNameByOwnerKey(QString ownerKey)
{
    // 钱包内的账户 根据公钥获得账户名
    QString accountName;
    foreach (QString key, accountInfoMap.keys())
    {
        if( accountInfoMap.value(key).ownerKey == ownerKey)
        {
            accountName = key;
            break;
        }
    }

    return accountName;
}

QString HXChain::changePathFormat(QString path)
{
    // qsetting 配置文件中不能 key不能有/和\   把斜杠换为?
    path.replace("\\", "?");
    path.replace("/", "?");
    return path;
}

QString HXChain::restorePathFormat(QString path)
{
    path.replace("?", "\\");
    return path;
}

bool HXChain::isContractFileRegistered(QString path)
{
    QString address;
    if( path.endsWith(".glua") ||  path.endsWith(".gpc") )
    {
        address = configGetContractAddress(path);
    }
    else
    {
        address = path;
    }

    return address.startsWith("CON");
}

bool HXChain::isContractFileUpgraded(QString path)
{
    QString address;
    if( path.endsWith(".glua") || path.endsWith(".gpc") )
    {
        address = configGetContractAddress(path);
    }
    else
    {
        address = path;
    }

    if( address.startsWith("CON"))
    {
        QString level = contractInfoMap.value(address).level;

        return !( level == "temp");
    }
    else
    {
        return false;
    }
}

bool HXChain::isScriptAdded(QString path)
{
     return !configGetScriptId(path).isEmpty();
}

bool HXChain::isInContracts(QString filePath)
{
    QDir dir("contracts");
    QFileInfo fileInfo(filePath);
    return (dir.absolutePath() + "/" + fileInfo.fileName() == filePath);
}

bool HXChain::isInScripts(QString filePath)
{
    QDir dir("scripts");
    QFileInfo fileInfo(filePath);
    return (dir.absolutePath() + "/" + fileInfo.fileName() == filePath);
}

QString HXChain::configGetContractAddress(QString path)
{
    QString configKey;
    if( currenChain() == 1)
    {
        configKey = "/testRegisteredContracts/";
    }
    else
    {
        configKey = "/formalRegisteredContracts/";
    }
    return configFile->value( configKey + changePathFormat(path)).toString();

}

void HXChain::configSetContractAddress(QString path, QString address)
{
    if( currenChain() == 1)
    {
        configFile->setValue("/testRegisteredContracts/" + changePathFormat( path) , address);
    }
    else
    {
        configFile->setValue("/formalRegisteredContracts/" + changePathFormat( path) , address);
    }
}

QString HXChain::configRemoveContractAddress(QString address)
{
    QString configKey;
    if( currenChain() == 1)
    {
        configKey = "/testRegisteredContracts/";
    }
    else
    {
        configKey = "/formalRegisteredContracts/";
    }

    configFile->beginGroup(configKey);
    QStringList keys = configFile->childKeys();
    QString key;
    foreach (QString k, keys)
    {
        if( configFile->value(k) == address)
        {
            key = k;
            if( isInSandBox &&  !(key.endsWith("_sandbox.glua") || key.endsWith("_sandbox.gpc")) )
            {
                // 如果在沙盒中 且不是沙盒中注册的合约 config中记录添加 _removed 后缀
                QString pre = k.left( k.count() - 4);
                QString newKey = pre + "_removed" + k.right(4);
                configFile->setValue(newKey, configFile->value(k));
                configFile->remove(k);
                key = "";

                // 文件添加_remove后缀
        qDebug() << "configRemoveContractAddress rename: " <<   QFile::rename( HXChain::getInstance()->restorePathFormat(k), HXChain::getInstance()->restorePathFormat(newKey));
            }
            else
            {
                configFile->remove(k);
            }
            break;
        }
    }
    configFile->endGroup();
    return key;
}

QString HXChain::configGetScriptId(QString path)
{
    QString configKey;
    if( currenChain() == 1)
    {
        configKey = "/testAddedScripts/";
    }
    else
    {
        configKey = "/formalAddedScripts/";
    }
    return configFile->value( configKey + changePathFormat(path)).toString();

}

void HXChain::configSetScriptId(QString path, QString scriptId)
{
    if( currenChain() == 1)
    {
        configFile->setValue("/testAddedScripts/" + changePathFormat( path) , scriptId);
    }
    else
    {
        configFile->setValue("/formalAddedScripts/" + changePathFormat( path) , scriptId);
    }
}

QStringList HXChain::configGetAllScripts()
{
    QString configKey;
    if( currenChain() == 1)
    {
        configKey = "/testAddedScripts/";
    }
    else
    {
        configKey = "/formalAddedScripts/";
    }

    configFile->beginGroup(configKey);
    QStringList keys = configFile->childKeys();
    configFile->endGroup();
    return keys;
}

QString HXChain::configRemoveScriptId(QString scriptId)
{
    QString configKey;
    if( currenChain() == 1)
    {
        configKey = "/testAddedScripts/";
    }
    else
    {
        configKey = "/formalAddedScripts/";
    }

    configFile->beginGroup(configKey);
    QStringList keys = configFile->childKeys();
    QString key;
    foreach (QString k, keys)
    {
        if( configFile->value(k) == scriptId)
        {
            key = k;
            configFile->remove(k);
            break;
        }
    }
    configFile->endGroup();
    return key;
}

void HXChain::clearSandboxOperation()
{
    configFile->beginGroup("/testRegisteredContracts/");
    QStringList keys = configFile->childKeys();
    foreach (QString key, keys)
    {
        if( key.endsWith("_sandbox.glua") || key.endsWith("_sandbox.gpc"))
        {
            QString path = restorePathFormat(key);
            QFile file(path);
            if( file.exists())
            {
                qDebug() << "sandbox file remove: " << file.remove();
            }

            configFile->remove(key);
        }
        else if( key.endsWith("_removed.glua") || key.endsWith("_removed.gpc"))
        {
            // 退出沙盒时获取 沙盒里被销毁的合约（因为如果是刚注册的合约，不wallet_scan_contract的话wallet_get_contracts获取不到）
            QString contract = configFile->value(key).toString();
            postRPC(  "id_get_contract_info_" + contract, toJsonFormat("get_contract_info", QJsonArray() << contract  ));

            QString pre = key.left(key.count() - 12);
            QString oldKey = pre + key.right(4);
            configFile->setValue(oldKey, configFile->value(key));
            configFile->remove(key);

            // 将沙盒模式下可能的销毁合约操作时重命名为_removed文件的改回来
       qDebug() << "clearSandboxOperation rename: " << QFile::rename( restorePathFormat(key), restorePathFormat(oldKey));
        }
    }
    configFile->endGroup();

    configFile->beginGroup("/formalRegisteredContracts/");
    keys = configFile->childKeys();
    foreach (QString key, keys)
    {
        if( key.endsWith("_sandbox.glua") || key.endsWith("_sandbox.gpc"))
        {
            QString path = restorePathFormat(key);
            QFile file(path);
            if( file.exists())
            {
                qDebug() << "sandbox file remove: " << file.remove();
            }

            configFile->remove(key);
        }
        else if( key.endsWith("_removed.glua") || key.endsWith("_removed.gpc"))
        {
            // 退出沙盒时获取 沙盒里被销毁的合约（因为如果是刚注册的合约，不wallet_scan_contract的话wallet_get_contracts获取不到）
            QString contract = configFile->value(key).toString();
            postRPC(  "id_get_contract_info_" + contract, toJsonFormat("get_contract_info", QJsonArray() << contract  ));

            QString pre = key.left(key.count() - 12);
            QString oldKey = pre + key.right(4);
            configFile->setValue(oldKey, configFile->value(key));
            configFile->remove(key);

            // 将沙盒模式下可能的销毁合约操作时重命名为_removed文件的改回来
       qDebug() << "clearSandboxOperation rename: " << QFile::rename( restorePathFormat(key), restorePathFormat(oldKey));
        }
    }
    configFile->endGroup();

    configFile->beginGroup("/testAddedScripts/");
    keys = configFile->childKeys();
    foreach (QString key, keys)
    {
        if( key.endsWith("_sandbox.glua"))
        {
            QString path = restorePathFormat(key);
            QFile file(path);
            if( file.exists())
            {
                qDebug() << "sandbox file remove: " << file.remove();
            }

            configFile->remove(key);
        }
    }
    configFile->endGroup();

    configFile->beginGroup("/formalAddedScripts/");
    keys = configFile->childKeys();
    foreach (QString key, keys)
    {
        if( key.endsWith("_sandbox.glua"))
        {
            QString path = restorePathFormat(key);
            QFile file(path);
            if( file.exists())
            {
                qDebug() << "sandbox file remove: " << file.remove();
            }

            configFile->remove(key);
        }
    }
    configFile->endGroup();
}

QProcess *HXChain::currentProcess()
{
    QProcess* p = NULL;
    if( currenChain() == 1)
    {
        p = testProcess;
    }
    else if( currenChain() == 2)
    {
        p = formalProcess;
    }

    return p;
}

void HXChain::setCurrentAccount(QString accountName)
{
    currentAccount = accountName;
    if( currenChain() == 1)
    {
        configFile->setValue("/settings/testCurrentAccount",accountName);
    }
    else
    {
        configFile->setValue("/settings/formalCurrentAccount",accountName);
    }
}

int HXChain::currenChain()
{
    mutexForCurrentProcess.lock();

    int type = currenChainType;

    mutexForCurrentProcess.unlock();

    return type;
}

void HXChain::setCurrentChain(int type)
{
    mutexForCurrentProcess.lock();

    currenChainType = type;

    mutexForCurrentProcess.unlock();
}


QString toJsonFormat(QString instruction,
                      QJsonArray parameters)
{
    QString sandboxCommand = instruction;

    if( HXChain::getInstance()->isInSandBox)
    {
        if( g_sandBoxCommandMap.contains(instruction))
        {
            sandboxCommand = g_sandBoxCommandMap.value(instruction);
        }
    }

    QJsonObject object;
    object.insert("id", 32800);
    object.insert("method", sandboxCommand);
    object.insert("params",parameters);

    return QJsonDocument(object).toJson();
}

template <class key,class value>
key getKeyByValue(QMap<key, value> m, value v)
{
    foreach (key k, m)
    {
        if( m.value(k) == v)   return k;
    }

    return m.end();
}
