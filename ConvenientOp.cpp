#include "ConvenientOp.h"

#include <QTimer>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QTextCodec>
#include <QCoreApplication>
#include <QDir>
#include <QFileDialog>
#include <QMessageBox>

#include "popwidget/commondialog.h"
#include "IDEUtil.h"
#include "ChainIDE.h"

ConvenientOp::ConvenientOp(QObject *parent) : QObject(parent)
{

}

void ConvenientOp::ShowSyncCommonDialog(const QString &data)
{
    QTimer::singleShot(1,[data](){
        CommonDialog dia(CommonDialog::OkOnly);
        dia.setText(data);
        dia.exec();
    });
}

bool ConvenientOp::ReadContractFromFile(const QString &filePath, DataDefine::AddressContractDataPtr &results)
{
    //解析json文档
    if (!results) results = std::make_shared<DataDefine::AddressContractData>();
    results->clear();

    QFile contractFile(filePath);
    if(!contractFile.open(QIODevice::ReadOnly)) return false;

    QString jsonStr(contractFile.readAll());
    contractFile.close();

    QTextCodec* utfCodec = QTextCodec::codecForName("UTF-8");
    QByteArray ba = utfCodec->fromUnicode(jsonStr);

    QJsonParseError json_error;
    QJsonDocument parse_doucment = QJsonDocument::fromJson(ba, &json_error);

    if(json_error.error != QJsonParseError::NoError || !parse_doucment.isObject()) return false;

    QJsonObject jsonObject = parse_doucment.object();
    //开始分析
    foreach(QString key,jsonObject.keys()){
        QJsonValue val = jsonObject.value(key);
        if(!val.isArray()) continue;
        QJsonArray arr = val.toArray();
        foreach(QJsonValue obj , arr){
            if(!obj.isObject()) continue;
            QJsonObject contract = obj.toObject();
            foreach(QString addr,contract.keys()){
                if(!contract.value(addr).isString()) continue;
                results->AddContract(key,addr,contract.value(addr).toString());
            }
        }
    }
    return true;
}

void ConvenientOp::AddContract(const QString &owneraddr, const QString &contractaddr, const QString &contractname)
{
    DataDefine::AddressContractDataPtr contractData = std::make_shared<DataDefine::AddressContractData>();
    QString contractPath = ChainIDE::getInstance()->getCurrentChainType() == 1? DataDefine::LOCAL_CONTRACT_TEST_PATH : DataDefine::LOCAL_CONTRACT_FORMAL_PATH;
    ConvenientOp::ReadContractFromFile(QCoreApplication::applicationDirPath()+QDir::separator()+contractPath,contractData);
    contractData->AddContract(owneraddr, contractaddr,contractname);
    ConvenientOp::WriteContractToFile(QCoreApplication::applicationDirPath()+QDir::separator()+contractPath,contractData);
}

void ConvenientOp::DeleteContract(const QString &ownerOrcontract)
{
    DataDefine::AddressContractDataPtr contractData = std::make_shared<DataDefine::AddressContractData>();
    QString contractPath = ChainIDE::getInstance()->getCurrentChainType() == 1? DataDefine::LOCAL_CONTRACT_TEST_PATH : DataDefine::LOCAL_CONTRACT_FORMAL_PATH;
    ConvenientOp::ReadContractFromFile(QCoreApplication::applicationDirPath()+QDir::separator()+contractPath,contractData);
    contractData->DeleteContract(ownerOrcontract);
    ConvenientOp::WriteContractToFile(QCoreApplication::applicationDirPath()+QDir::separator()+contractPath,contractData);
}

bool ConvenientOp::WriteContractToFile(const QString &filePath, const DataDefine::AddressContractDataPtr &data)
{
    if(!data) return false;
    //构建QJsonDocument
    QJsonObject mainObject;
    foreach(DataDefine::AddressContractPtr ownerContract,data->getAllData())
    {
        QJsonArray memberArr;
        foreach(DataDefine::ContractInfoPtr cont,ownerContract->GetContracts())
        {
            QJsonObject personObj;
            personObj.insert(cont->GetContractAddr(),cont->GetContractName());
            memberArr.push_back(personObj);
        }

        mainObject.insert(ownerContract->GetOwnerAddr(),memberArr);
    }
    QJsonDocument document;
    document.setObject(mainObject);

    QByteArray byte_array = document.toJson(QJsonDocument::Compact);
    //写文件
    QFile file(filePath);
    if(!file.open(QIODevice::WriteOnly)) return false;

    QTextStream stream(&file);
    stream.setCodec("UTF-8");
    stream<<byte_array;
    stream.setGenerateByteOrderMark(true);
    file.close();
    return true;

}

QString ConvenientOp::GetMetaJsonFile(const QString &filePath)
{
    QString file = filePath;
    file.replace("\\","/");
    QString topdir = QCoreApplication::applicationDirPath()+QDir::separator();
    topdir.replace("\\","/");

    QString resultDir;
    if(file.startsWith(topdir+DataDefine::GLUA_DIR))
    {
        resultDir = IDEUtil::getNextDir(topdir + DataDefine::GLUA_DIR,file);
    }
    else if(file.startsWith(topdir+DataDefine::JAVA_DIR))
    {
        resultDir = IDEUtil::getNextDir(topdir + DataDefine::JAVA_DIR,file);
    }
    else if(file.startsWith(topdir+DataDefine::CSHARP_DIR))
    {
        resultDir = IDEUtil::getNextDir(topdir + DataDefine::CSHARP_DIR,file);
    }
    else if(file.startsWith(topdir+DataDefine::KOTLIN_DIR))
    {
        resultDir = IDEUtil::getNextDir(topdir + DataDefine::KOTLIN_DIR,file);
    }

    return resultDir+"/"+QFileInfo(resultDir).baseName()+"." + DataDefine::META_SUFFIX;
}

QStringList ConvenientOp::GetContractSuffixByDir(const QString &dirPath)
{
    if(!QFileInfo(dirPath).isDir()) return QStringList();

    QString topDir = QCoreApplication::applicationDirPath() + "/" ;
    if(dirPath.startsWith(topDir + DataDefine::GLUA_DIR))
    {
        return QStringList()<<DataDefine::GLUA_SUFFIX;
    }
    else if(dirPath.startsWith(topDir + DataDefine::JAVA_DIR))
    {
        return QStringList()<<DataDefine::JAVA_SUFFIX;
    }
    else if(dirPath.startsWith(topDir + DataDefine::CSHARP_DIR))
    {
        return QStringList()<<DataDefine::CSHARP_SUFFIX;
    }
    else if(dirPath.startsWith(topDir + DataDefine::KOTLIN_DIR))
    {
        return QStringList()<<DataDefine::KOTLIN_SUFFIX;
    }
    else if(dirPath.startsWith(topDir + DataDefine::CONTRACT_DIR))
    {
        return QStringList()<<DataDefine::GLUA_SUFFIX
                           <<DataDefine::JAVA_SUFFIX<<DataDefine::CSHARP_SUFFIX<<DataDefine::KOTLIN_SUFFIX;
    }
    return QStringList();
}

bool ConvenientOp::ImportContractFile(const QString &parentDir)
{
    QStringList suffixs = GetContractSuffixByDir(parentDir);
    if(suffixs.isEmpty()) return false;

    QString limits;
    foreach(QString suffix,suffixs){
        limits += "*." + suffix + " ";
    }
    limits.remove(limits.length()-1,1);

    //源
    QString file = QFileDialog::getOpenFileName(nullptr, tr("Choose your contract file."),"",limits);
    if(!QFileInfo(file).exists()) return false;
    //目标
    QString dirPath = parentDir;
    if(suffixs.count() > 1)
    {
        //根目录,先定位到是哪个合约类型
        if(QFileInfo(file).suffix() == DataDefine::GLUA_SUFFIX){
            dirPath += "/" + DataDefine::GLUA_DIR;
        }
        else if(QFileInfo(file).suffix() == DataDefine::JAVA_SUFFIX){
            dirPath += "/" + DataDefine::JAVA_DIR;
        }
        else if(QFileInfo(file).suffix() == DataDefine::CSHARP_SUFFIX){
            dirPath += "/" + DataDefine::CSHARP_DIR;
        }
        else if(QFileInfo(file).suffix() == DataDefine::KOTLIN_SUFFIX){
            dirPath += "/" + DataDefine::KOTLIN_DIR;
        }
    }
    QString top = QCoreApplication::applicationDirPath()+"/";
    if(dirPath == top+DataDefine::GLUA_DIR || dirPath == top+DataDefine::JAVA_DIR ||
       dirPath == top+DataDefine::CSHARP_DIR || dirPath == top+DataDefine::KOTLIN_DIR )
    {
        QString dir = dirPath + "/" +QFileInfo(file).baseName();
        dirPath = IDEUtil::createDir(dir);
    }

    //开始复制
    //正式导入文件
    QString dstFilePath = dirPath + "/" + QFileInfo(file).fileName();
    if(QFileInfo(dstFilePath).exists())
    {
        //提示存在,是否覆盖
        if(QMessageBox::Yes == QMessageBox::question(nullptr,tr("question"),tr("already exists,yes to override!")))
        {
            QFile::remove(dstFilePath);
            QFile::copy(file, dstFilePath);
        }
        else
        {
            return false;
        }
    }
    else
    {
        QFile::copy(file, dstFilePath);
    }
    return true;
}
