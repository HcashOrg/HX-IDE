#include "FunctionWidgetHX.h"
#include "ui_FunctionWidgetHX.h"

#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QDebug>

#include <mutex>


#include "ChainIDE.h"
#include "IDEUtil.h"
#include "ConvenientOp.h"

static std::mutex datamutex;
FunctionWidgetHX::FunctionWidgetHX(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FunctionWidgetHX)
{
    ui->setupUi(this);
    InitWidget();
}

FunctionWidgetHX::~FunctionWidgetHX()
{
    delete ui;
}

void FunctionWidgetHX::RefreshContractAddr(const QString &addr)
{
    ui->treeWidget_offline->clear();
    ui->treeWidget_online->clear();
    ui->tabWidget->setCurrentIndex(0);
    //查询合约对应的api
    ChainIDE::getInstance()->postRPC("function-contractinfo_"+addr,
                                     IDEUtil::toJsonFormat("get_contract_info",QJsonArray()<<addr));
}

void FunctionWidgetHX::jsonDataUpdated(const QString &id, const QString &data)
{
    if(id.startsWith("function-contractinfo_"))
    {
        QString ownerAddr = id.mid(QString("function-contractinfo_").length());
        parseContractInfo(ownerAddr,data);
    }
}

void FunctionWidgetHX::InitWidget()
{
    ui->treeWidget_offline->header()->setVisible(false);
    ui->treeWidget_online->header()->setVisible(false);
    connect(ChainIDE::getInstance(),&ChainIDE::jsonDataUpdated,this,&FunctionWidgetHX::jsonDataUpdated);
}

bool FunctionWidgetHX::parseContractInfo(const QString &addr, const QString &data)
{

    QJsonParseError json_error;
    QJsonDocument parse_doucment = QJsonDocument::fromJson(data.toLatin1(),&json_error);
    if(json_error.error != QJsonParseError::NoError || !parse_doucment.isObject())
    {
         qDebug()<<json_error.errorString();
         std::lock_guard<std::mutex> guard(datamutex);
         ConvenientOp::DeleteContract(addr);
         return false;
    }
    QJsonArray apisArr = parse_doucment.object().value("result").toObject().value("code_printable").toObject().value("abi").toArray();
    foreach (QJsonValue val, apisArr) {
        if(!val.isString()) continue;
        QTreeWidgetItem *itemChild = new QTreeWidgetItem(QStringList()<<val.toString());
        ui->treeWidget_online->addTopLevelItem(itemChild);
    }
    QJsonArray offapisArr = parse_doucment.object().value("result").toObject().value("code_printable").toObject().value("offline_abi").toArray();
    foreach (QJsonValue val, offapisArr) {
        if(!val.isString()) continue;
        QTreeWidgetItem *itemChild = new QTreeWidgetItem(QStringList()<<val.toString());
        ui->treeWidget_offline->addTopLevelItem(itemChild);
    }

    return true;
}

void FunctionWidgetHX::retranslator()
{
    ui->retranslateUi(this);
}
