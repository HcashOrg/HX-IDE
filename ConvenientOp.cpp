#include "ConvenientOp.h"

#include <QTimer>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QTextCodec>
#include <QCoreApplication>
#include <QDir>

#include "popwidget/commondialog.h"
#include "IDEUtil.h"

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
    ConvenientOp::ReadContractFromFile(QCoreApplication::applicationDirPath()+QDir::separator()+DataDefine::LOCAL_CONTRACT_PATH,contractData);
    contractData->AddContract(owneraddr, contractaddr,contractname);
    ConvenientOp::WriteContractToFile(QCoreApplication::applicationDirPath()+QDir::separator()+DataDefine::LOCAL_CONTRACT_PATH,contractData);
}

void ConvenientOp::DeleteContract(const QString &ownerOrcontract)
{
    DataDefine::AddressContractDataPtr contractData = std::make_shared<DataDefine::AddressContractData>();
    ConvenientOp::ReadContractFromFile(QCoreApplication::applicationDirPath()+QDir::separator()+DataDefine::LOCAL_CONTRACT_PATH,contractData);
    contractData->DeleteContract(ownerOrcontract);
    ConvenientOp::WriteContractToFile(QCoreApplication::applicationDirPath()+QDir::separator()+DataDefine::LOCAL_CONTRACT_PATH,contractData);
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

    return resultDir+"/"+QFileInfo(resultDir).baseName()+".meta.json";
}
