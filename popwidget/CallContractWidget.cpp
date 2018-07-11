#include "CallContractWidget.h"
#include "ui_CallContractWidget.h"

#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QCoreApplication>
#include <QDir>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>

#include "ChainIDE.h"
#include "DataManager.h"
#include "IDEUtil.h"
#include "ConvenientOp.h"

CallContractWidget::CallContractWidget(QWidget *parent) :
    MoveableDialog(parent),
    ui(new Ui::CallContractWidget)
{
    ui->setupUi(this);
    InitWidget();
}

CallContractWidget::~CallContractWidget()
{
    delete ui;
}

void CallContractWidget::jsonDataUpdated(const QString &id, const QString &data)
{
    if("call_callcontract" == id)
    {
        ConvenientOp::ShowSyncCommonDialog(data);
        close();
    }
    else if(id.startsWith("call-contractinfo_"))
    {
        QString ownerAddr = id.mid(QString("call-contractinfo_").length());
        parseContractInfo(ownerAddr,data);
    }
}

void CallContractWidget::CallContract()
{
<<<<<<< HEAD
    ChainIDE::getInstance()->postRPC("call_callcontract",IDEUtil::toJsonFormat("invoke_contract",QJsonArray()<<
                                     ui->callAddress->currentText()<<ui->gasprice->text()<<ui->gaslimit->text()
                                     <<ui->contractAddress->currentText()<<ui->function->currentText()
                                     <<ui->param->text()));
=======
    ChainIDE::getInstance()->postRPC("call_callcontract",IDEUtil::toJsonFormat("callcontract",QJsonArray()<<
                                     ui->callAddress->currentText()<<ui->contractAddress->currentText()<<ui->function->currentText()
                                     <<ui->param->text()<<ui->gaslimit->value()<<ui->gasprice->value()<<ui->fee->value()));
>>>>>>> b41c4ef8ab0b45651d7ad4ccd9b34bd3691512f8
}

void CallContractWidget::contractAddressChanged()
{
    //查询合约对应的api
    ChainIDE::getInstance()->postRPC("call-contractinfo_"+ui->contractAddress->currentText(),
<<<<<<< HEAD
                                     IDEUtil::toJsonFormat("get_contract_info",QJsonArray()<<ui->contractAddress->currentText()));
=======
                                     IDEUtil::toJsonFormat("getcontractinfo",QJsonArray()<<ui->contractAddress->currentText()));
>>>>>>> b41c4ef8ab0b45651d7ad4ccd9b34bd3691512f8
}

void CallContractWidget::InitWidget()
{
    ui->gaslimit->setRange(0,999999);
    ui->gaslimit->setSingleStep(1);
    ui->gasprice->setRange(10,999999);

    setWindowFlags(Qt::FramelessWindowHint);

    //ui->callAddress->setEditable(true);
    //ui->contractAddress->setEditable(true);

    //读取所有账户信息
    connect(ChainIDE::getInstance()->getDataManager(),&DataManager::queryAccountFinish,this,&CallContractWidget::InitAccountAddress);
    connect(ChainIDE::getInstance()->getDataManager(),&DataManager::queryContractFinish,this,&CallContractWidget::InitContractAddress);
    ChainIDE::getInstance()->getDataManager()->queryAccount();

    connect(ui->closeBtn,&QToolButton::clicked,this,&CallContractWidget::close);
    connect(ui->cancelBtn,&QToolButton::clicked,this,&CallContractWidget::close);
    connect(ui->okBtn,&QToolButton::clicked,this,&CallContractWidget::CallContract);

    connect(ChainIDE::getInstance(),&ChainIDE::jsonDataUpdated,this,&CallContractWidget::jsonDataUpdated);
}

void CallContractWidget::InitAccountAddress()
{
    DataDefine::AccountDataPtr data = ChainIDE::getInstance()->getDataManager()->getAccount();
    for(auto it = data->getAccount().begin();it != data->getAccount().end();++it)
    {
        ui->callAddress->addItem((*it)->getAccountName());
    }

    //查询合约
    ChainIDE::getInstance()->getDataManager()->queryContract();
}

void CallContractWidget::InitContractAddress()
{

    DataDefine::AddressContractDataPtr data = ChainIDE::getInstance()->getDataManager()->getContract();

    QTreeWidget *tree = new QTreeWidget(this);
    tree->header()->setVisible(false);

    for(auto it = data->getAllData().begin();it != data->getAllData().end();++it)
    {
        QTreeWidgetItem *item = new QTreeWidgetItem(QStringList()<<(*it)->GetOwnerAddr());
        item->setFlags(Qt::ItemIsEnabled);
        tree->addTopLevelItem(item);
        for(auto cont = (*it)->GetContracts().begin();cont != (*it)->GetContracts().end();++cont)
        {
            QTreeWidgetItem *childitem = new QTreeWidgetItem(QStringList()<<(*cont)->GetContractAddr());
            item->addChild(childitem);
        }

    }
    tree->expandAll();
    ui->contractAddress->setModel(tree->model());
    ui->contractAddress->setView(tree);

    connect(ui->contractAddress,static_cast<void (QComboBox::*)(const QString &)>(&QComboBox::activated),this,
            &CallContractWidget::contractAddressChanged);
}

bool CallContractWidget::parseContractInfo(const QString &addr, const QString &data)
{
    ui->function->clear();
    QTreeWidget *tree = new QTreeWidget(this);
    tree->header()->setVisible(false);
    QTreeWidgetItem *item = new QTreeWidgetItem(QStringList()<<"api");
    item->setFlags(Qt::ItemIsEnabled);
    tree->addTopLevelItem(item);

    QTreeWidgetItem *item1 = new QTreeWidgetItem(QStringList()<<"offline-api");
    item1->setFlags(Qt::ItemIsEnabled);
    tree->addTopLevelItem(item1);

    QJsonParseError json_error;
    QJsonDocument parse_doucment = QJsonDocument::fromJson(data.toLatin1(),&json_error);
    if(json_error.error != QJsonParseError::NoError || !parse_doucment.isObject())
    {
         qDebug()<<json_error.errorString();
         return false;
    }
    QJsonArray apisArr = parse_doucment.object().value("result").toObject().value("code_printable").toObject().value("abi").toArray();
    foreach (QJsonValue val, apisArr) {
        if(!val.isString()) continue;
        QTreeWidgetItem *itemChild = new QTreeWidgetItem(QStringList()<<val.toString());
        item->addChild(itemChild);
    }
    QJsonArray offapisArr = parse_doucment.object().value("result").toObject().value("code_printable").toObject().value("offline_abi").toArray();
    foreach (QJsonValue val, offapisArr) {
        if(!val.isString()) continue;
        QTreeWidgetItem *itemChild = new QTreeWidgetItem(QStringList()<<val.toString());
        item1->addChild(itemChild);
    }


    tree->expandAll();
    ui->function->setModel(tree->model());
    ui->function->setView(tree);
    return true;
}
