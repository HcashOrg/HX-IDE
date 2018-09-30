#include "CallContractWidgetHX.h"
#include "ui_CallContractWidgetHX.h"

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
#include "datamanager/DataManagerHX.h"
#include "IDEUtil.h"
#include "ConvenientOp.h"

CallContractWidgetHX::CallContractWidgetHX(QWidget *parent) :
    MoveableDialog(parent),
    ui(new Ui::CallContractWidgetHX)
{
    ui->setupUi(this);
    InitWidget();
}

CallContractWidgetHX::~CallContractWidgetHX()
{
    delete ui;
}

void CallContractWidgetHX::jsonDataUpdated(const QString &id, const QString &data)
{
    if("call_callcontract" == id)
    {
        ConvenientOp::ShowSyncCommonDialog(data);
        close();
    }
    else if("call_callcontract_test" == id)
    {
        double fee = parseTestCallFee(data);
        ui->gaslimit->setToolTip(tr("approximatefee:%1").arg(QString::number(fee,'f',5)));
        ui->gasprice->setToolTip(tr("approximatefee:%1").arg(QString::number(fee,'f',5)));
    }

}

void CallContractWidgetHX::CallContract()
{
    if(ui->contractAddress->currentText().isEmpty() || ui->callAddress->currentText().isEmpty() || ui->function->currentText().isEmpty())
    {
        return;
    }

    if(ui->gaslimit->isEnabled() && ui->gasprice->isEnabled())
    {
        ChainIDE::getInstance()->postRPC("call_callcontract",IDEUtil::toJsonFormat("invoke_contract",QJsonArray()<<
                                         ui->callAddress->currentText()<<ui->gasprice->text()<<ui->gaslimit->text()
                                         <<ui->contractAddress->currentText()<<ui->function->currentText()
                                         <<ui->param->text()));
    }
    else if(!ui->gaslimit->isEnabled() && !ui->gasprice->isEnabled())
    {
        ChainIDE::getInstance()->postRPC("call_callcontract",IDEUtil::toJsonFormat("invoke_contract_offline",QJsonArray()<<
                                         ui->callAddress->currentText()<<ui->contractAddress->currentText()
                                         <<ui->function->currentText()<<ui->param->text()));
    }
}

void CallContractWidgetHX::contractAddressChanged()
{
    ui->function->clear();
    //构建合约树
    DataManagerStruct::ContractInfoPtr info = DataManagerHX::getInstance()->getContract()->getContractInfo(ui->contractAddress->currentText());
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

void CallContractWidgetHX::functionChanged()
{
    ui->gaslimit->setEnabled(ui->function->currentData().toString() == "api");
    ui->gasprice->setEnabled(ui->function->currentData().toString() == "api");
}

void CallContractWidgetHX::testCallContract()
{
    if(ui->function->currentData().toString() != "api")
    {
        ui->gaslimit->setValue(0);
        ui->gasprice->setValue(10);
        ui->gasprice->setToolTip(tr("离线函数无需费用"));
        ui->gaslimit->setToolTip(tr("离线函数无需费用"));
        return;
    }
    //测试费用
    ChainIDE::getInstance()->postRPC("call_callcontract_test",IDEUtil::toJsonFormat("invoke_contract_testing",QJsonArray()<<
                                     ui->callAddress->currentText()<<ui->contractAddress->currentText()
                                     <<ui->function->currentText()<<ui->param->text()));
}

void CallContractWidgetHX::InitWidget()
{
    ui->gaslimit->setRange(0,999999);
    ui->gaslimit->setSingleStep(1);
    ui->gasprice->setRange(10,999999);

    setWindowFlags(Qt::FramelessWindowHint);

    connect(ui->closeBtn,&QToolButton::clicked,this,&CallContractWidgetHX::close);
    connect(ui->cancelBtn,&QToolButton::clicked,this,&CallContractWidgetHX::close);
    connect(ui->okBtn,&QToolButton::clicked,this,&CallContractWidgetHX::CallContract);

    connect(ChainIDE::getInstance(),&ChainIDE::jsonDataUpdated,this,&CallContractWidgetHX::jsonDataUpdated);


    connect(ui->contractAddress,static_cast<void (QComboBox::*)(const QString &)>(&QComboBox::activated),this,
            &CallContractWidgetHX::contractAddressChanged);

    connect(ui->function,static_cast<void (QComboBox::*)(const QString &)>(&QComboBox::activated),this,
            &CallContractWidgetHX::functionChanged);


    connect(ui->contractAddress,static_cast<void (QComboBox::*)(const QString &)>(&QComboBox::activated),this,
            &CallContractWidgetHX::testCallContract);
    connect(ui->function,static_cast<void (QComboBox::*)(const QString &)>(&QComboBox::activated),this,
            &CallContractWidgetHX::testCallContract);
    connect(ui->param,&QLineEdit::textEdited,this,&CallContractWidgetHX::testCallContract);

    //读取所有账户信息
    connect(DataManagerHX::getInstance(),&DataManagerHX::queryAccountFinish,this,&CallContractWidgetHX::InitAccountAddress);
    connect(DataManagerHX::getInstance(),&DataManagerHX::queryContractFinish,this,&CallContractWidgetHX::InitContractAddress);
    DataManagerHX::getInstance()->queryAccount();

}

void CallContractWidgetHX::InitAccountAddress()
{
    DataManagerStruct::AccountHX::AccountDataPtr data = DataManagerHX::getInstance()->getAccount();
    for(auto it = data->getAccount().begin();it != data->getAccount().end();++it)
    {
        ui->callAddress->addItem((*it)->getAccountName());
    }

    //查询合约
    DataManagerHX::getInstance()->queryContract();
}

void CallContractWidgetHX::InitContractAddress()
{

    DataManagerStruct::AddressContractDataPtr data = DataManagerHX::getInstance()->getContract();

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

double CallContractWidgetHX::parseTestCallFee(const QString &data) const
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
        if(addr.isObject())
        {
            if(addr.toObject().value("amount").isString())
            {
                resultVal += addr.toObject().value("amount").toString().toULongLong()/pow(10,5);
            }
            else if(addr.toObject().value("amount").isDouble())
            {
                resultVal += addr.toObject().value("amount").toDouble()/pow(10,5);
            }
        }
        else if(addr.isDouble())
        {
            ui->gaslimit->setValue(addr.toInt());
        }
    }
    return resultVal;
}
