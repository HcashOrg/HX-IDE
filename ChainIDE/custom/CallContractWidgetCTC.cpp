#include "CallContractWidgetCTC.h"
#include "ui_CallContractWidgetCTC.h"

#include <math.h>
#include <QHeaderView>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QCoreApplication>
#include <QDir>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QDebug>

#include "ChainIDE.h"
#include "datamanager/DataManagerCTC.h"
#include "IDEUtil.h"
#include "ConvenientOp.h"

CallContractWidgetCTC::CallContractWidgetCTC(QWidget *parent) :
    MoveableDialog(parent),
    ui(new Ui::CallContractWidgetCTC)
{
    ui->setupUi(this);
    InitWidget();
}

CallContractWidgetCTC::~CallContractWidgetCTC()
{
    delete ui;
}

void CallContractWidgetCTC::jsonDataUpdated(const QString &id, const QString &data)
{
    if("call_callcontract" == id)
    {
        ConvenientOp::ShowSyncCommonDialog(data);
        close();
    }
    else if("call_callcontract_test" == id)
    {
        ui->fee->setValue(parseTestCallFee(data));
        ui->fee->setToolTip(tr("approximatefee:%1").arg(ui->fee->text()));
    }
}

void CallContractWidgetCTC::CallContract()
{
    if(ui->contractAddress->currentText().isEmpty() || ui->callAddress->currentText().isEmpty() || ui->function->currentText().isEmpty())
    {
        return;
    }

    if(ui->fee->isEnabled() )
    {
        ChainIDE::getInstance()->postRPC("call_callcontract",IDEUtil::toJsonFormat("contract_call_offline",QJsonArray()<<
                                         ui->contractAddress->currentText()<<ui->callAddress->currentText()
                                         <<ui->function->currentText()<<ui->param->text()
                                         <<"CTC"<<ui->fee->text()
                                         ,true));
    }
    else if(!ui->fee->isEnabled())
    {
        ChainIDE::getInstance()->postRPC("call_callcontract",IDEUtil::toJsonFormat("invoke_contract_offline",QJsonArray()<<
                                         ui->contractAddress->currentText()<<ui->callAddress->currentText()
                                         <<ui->function->currentText()<<ui->param->text(),true));
    }
}

void CallContractWidgetCTC::contractAddressChanged()
{
    ui->function->clear();
    //构建合约树
    DataManagerStruct::ContractInfoPtr info = DataManagerCTC::getInstance()->getContract()->getContractInfo(ui->contractAddress->currentText());
    if(!info) return;

    QTreeWidget *tree = new QTreeWidget(this);
    tree->header()->setVisible(false);
    QTreeWidgetItem *item = new QTreeWidgetItem(QStringList()<<"api");
    item->setFlags(Qt::ItemIsEnabled);
    tree->addTopLevelItem(item);

    QTreeWidgetItem *item1 = new QTreeWidgetItem(QStringList()<<"offline-api");
    item1->setFlags(Qt::ItemIsEnabled);
    tree->addTopLevelItem(item1);

    for(auto it = info->GetInterface()->getAllApiName().begin();it != info->GetInterface()->getAllApiName().end();++it)
    {
        QTreeWidgetItem *itemChild = new QTreeWidgetItem(QStringList()<<*it);
        itemChild->setData(0,Qt::UserRole,QVariant::fromValue<QString>("api"));
        item->addChild(itemChild);
    }
    for(auto it = info->GetInterface()->getAllOfflineApiName().begin();it != info->GetInterface()->getAllOfflineApiName().end();++it)
    {
        QTreeWidgetItem *itemChild = new QTreeWidgetItem(QStringList()<<*it);
        itemChild->setData(0,Qt::UserRole,QVariant::fromValue<QString>("offline-api"));
        item1->addChild(itemChild);
    }

    tree->expandAll();
    ui->function->setModel(tree->model());
    ui->function->setView(tree);
}

void CallContractWidgetCTC::functionChanged()
{
    ui->fee->setEnabled(ui->function->currentData().toString() == "api");
}

void CallContractWidgetCTC::testCallContract()
{
    if(ui->function->currentData().toString() != "api")
    {
        ui->fee->setValue(0);
        ui->fee->setToolTip(tr("离线函数无需费用"));
        return;
    }
    //测试费用
    ChainIDE::getInstance()->postRPC("call_callcontract_test",IDEUtil::toJsonFormat("contract_call_testing",QJsonArray()<<
                                     ui->contractAddress->currentText()<<ui->callAddress->currentText()
                                     <<ui->function->currentText()<<ui->param->text()
                                     ,true));
}

void CallContractWidgetCTC::InitWidget()
{
    ui->fee->setRange(0,999999999);
    ui->fee->setDecimals(8);

    setWindowFlags(Qt::FramelessWindowHint);

    connect(ui->closeBtn,&QToolButton::clicked,this,&CallContractWidgetCTC::close);
    connect(ui->cancelBtn,&QToolButton::clicked,this,&CallContractWidgetCTC::close);
    connect(ui->okBtn,&QToolButton::clicked,this,&CallContractWidgetCTC::CallContract);

    connect(ChainIDE::getInstance(),&ChainIDE::jsonDataUpdated,this,&CallContractWidgetCTC::jsonDataUpdated);


    connect(ui->contractAddress,static_cast<void (QComboBox::*)(const QString &)>(&QComboBox::activated),this,
            &CallContractWidgetCTC::contractAddressChanged);

    connect(ui->function,static_cast<void (QComboBox::*)(const QString &)>(&QComboBox::activated),this,
            &CallContractWidgetCTC::functionChanged);

    connect(ui->contractAddress,static_cast<void (QComboBox::*)(const QString &)>(&QComboBox::activated),this,
            &CallContractWidgetCTC::testCallContract);
    connect(ui->function,static_cast<void (QComboBox::*)(const QString &)>(&QComboBox::activated),this,
            &CallContractWidgetCTC::testCallContract);
    connect(ui->param,&QLineEdit::textEdited,this,&CallContractWidgetCTC::testCallContract);

    //读取所有账户信息
    connect(DataManagerCTC::getInstance(),&DataManagerCTC::queryAccountFinish,this,&CallContractWidgetCTC::InitAccountAddress);
    connect(DataManagerCTC::getInstance(),&DataManagerCTC::queryContractFinish,this,&CallContractWidgetCTC::InitContractAddress);
    DataManagerCTC::getInstance()->queryAccount();
}

void CallContractWidgetCTC::InitAccountAddress()
{
    DataManagerStruct::AccountCTC::AccountDataPtr data = DataManagerCTC::getInstance()->getAccount();
    for(auto it = data->getAccount().begin();it != data->getAccount().end();++it)
    {
        ui->callAddress->addItem((*it)->getAccountName());
    }

    //查询合约
    DataManagerCTC::getInstance()->queryContract();
}

void CallContractWidgetCTC::InitContractAddress()
{
    DataManagerStruct::AddressContractDataPtr data = DataManagerCTC::getInstance()->getContract();

    QTreeWidget *tree = new QTreeWidget(this);
    tree->header()->setVisible(false);

    for(auto it = data->getAllData().begin();it != data->getAllData().end();++it)
    {
        QTreeWidgetItem *item = new QTreeWidgetItem(QStringList()<<(*it)->GetOwnerAddr());
        item->setFlags(Qt::ItemIsEnabled);
        tree->addTopLevelItem(item);
        for(auto cont = (*it)->GetContracts().begin();cont != (*it)->GetContracts().end();++cont)
        {
            QTreeWidgetItem *childitem = new QTreeWidgetItem(QStringList()<<((*cont)->GetContractName().isEmpty()?(*cont)->GetContractAddr():(*cont)->GetContractName()));
            childitem->setToolTip(0,(*cont)->GetContractDes());
            item->addChild(childitem);
        }

    }
    tree->expandAll();
    ui->contractAddress->setModel(tree->model());
    ui->contractAddress->setView(tree);
}

double CallContractWidgetCTC::parseTestCallFee(const QString &data) const
{
    double resultVal = 0;
    QJsonParseError json_error;
    QJsonDocument parse_doucment = QJsonDocument::fromJson(data.toLatin1(),&json_error);
    if(json_error.error != QJsonParseError::NoError || !parse_doucment.isObject())
    {
        qDebug()<<json_error.errorString();
        return 0;
    }
    QJsonArray resultArray = parse_doucment.object().value("result").toArray();
    foreach (QJsonValue addr, resultArray) {
        if(addr.toObject().value("amount").isString())
        {
            resultVal += addr.toObject().value("amount").toString().toULongLong()/pow(10,8);
        }
        else if(addr.toObject().value("amount").isDouble())
        {
            resultVal += addr.toObject().value("amount").toDouble()/pow(10,8);
        }

    }
    return resultVal;
}
