#include "FunctionWidget.h"
#include "ui_FunctionWidget.h"

#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>

#include <mutex>


#include "ChainIDE.h"
#include "IDEUtil.h"
#include "ConvenientOp.h"

static std::mutex datamutex;
FunctionWidget::FunctionWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FunctionWidget)
{
    ui->setupUi(this);
    InitWidget();
}

FunctionWidget::~FunctionWidget()
{
    delete ui;
}

void FunctionWidget::RefreshContractAddr(const QString &addr)
{
    ui->treeWidget_offline->clear();
    ui->treeWidget_online->clear();
    ui->tabWidget->setCurrentIndex(0);
    //查询合约对应的api
    ChainIDE::getInstance()->postRPC("function-contractinfo_"+addr,
                                     IDEUtil::toJsonFormat("get_contract_info",QJsonArray()<<addr));
}

void FunctionWidget::jsonDataUpdated(const QString &id, const QString &data)
{
    if(id.startsWith("function-contractinfo_"))
    {
        QString ownerAddr = id.mid(QString("function-contractinfo_").length());
        parseContractInfo(ownerAddr,data);
    }
}

void FunctionWidget::InitWidget()
{
    ui->treeWidget_offline->header()->setVisible(false);
    ui->treeWidget_online->header()->setVisible(false);
    connect(ChainIDE::getInstance(),&ChainIDE::jsonDataUpdated,this,&FunctionWidget::jsonDataUpdated);
}

bool FunctionWidget::parseContractInfo(const QString &addr, const QString &data)
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

void FunctionWidget::retranslator()
{
    ui->retranslateUi(this);
}
