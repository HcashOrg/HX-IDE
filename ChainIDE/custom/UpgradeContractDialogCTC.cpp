#include "UpgradeContractDialogCTC.h"
#include "ui_UpgradeContractDialogCTC.h"

#include <math.h>
#include <QDebug>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>

#include "datamanager/DataManagerCTC.h"
#include "ConvenientOp.h"
#include "ChainIDE.h"
#include "IDEUtil.h"

UpgradeContractDialogCTC::UpgradeContractDialogCTC(QWidget *parent) :
    MoveableDialog(parent),
    ui(new Ui::UpgradeContractDialogCTC)
{
    ui->setupUi(this);
    InitWidget();
}

UpgradeContractDialogCTC::~UpgradeContractDialogCTC()
{
    delete ui;
}

void UpgradeContractDialogCTC::jsonDataUpdated(const QString &id, const QString &data)
{
    if("upgrade_upgrade_contract" == id)
    {
        ConvenientOp::ShowSyncCommonDialog(data);
        close();
    }
    else if("upgrade_upgrade_contract_test" == id)
    {
        ui->fee->setValue(parseTestUpgrade(data));
        ui->fee->setToolTip(tr("approximatefee:%1").arg(ui->fee->text()));
    }
}

void UpgradeContractDialogCTC::UpgradeContract()
{
    //对信息不足的操作不予执行
    if(ui->callAddress->currentText().isEmpty() || ui->contractAddress->currentText().isEmpty() || ui->contractName->text().isEmpty())
    {
        return;
    }
    ChainIDE::getInstance()->postRPC("upgrade_upgrade_contract",IDEUtil::toJsonFormat("contract_upgrade",QJsonArray()<<
                                     ui->contractAddress->currentText()<<ui->callAddress->currentText()
                                     <<ui->contractName->text()<<(ui->contractDes->text().isEmpty()?" ":ui->contractDes->text())
                                     <<"CTC"<<ui->fee->text(),true));
}

void UpgradeContractDialogCTC::testUpgradeContract()
{
    if(ui->contractName->text().isEmpty())
    {
        ui->fee->setValue(0);
        ui->fee->setToolTip(tr("approximatefee:%1").arg(ui->fee->text()));
    }
    ChainIDE::getInstance()->postRPC("upgrade_upgrade_contract_test",IDEUtil::toJsonFormat("contract_upgrade_testing",QJsonArray()<<
                                     ui->contractAddress->currentText()<<ui->callAddress->currentText()
                                     <<ui->contractName->text()<<(ui->contractDes->text().isEmpty()?" ":ui->contractDes->text())
                                     ,true));
}

void UpgradeContractDialogCTC::InitWidget()
{
    ui->fee->setRange(0,9999999999);
    ui->fee->setDecimals(8);

    setWindowFlags(Qt::FramelessWindowHint);

    connect(ui->closeBtn,&QToolButton::clicked,this,&UpgradeContractDialogCTC::close);
    connect(ui->cancelBtn,&QToolButton::clicked,this,&UpgradeContractDialogCTC::close);
    connect(ui->okBtn,&QToolButton::clicked,this,&UpgradeContractDialogCTC::UpgradeContract);

    connect(ChainIDE::getInstance(),&ChainIDE::jsonDataUpdated,this,&UpgradeContractDialogCTC::jsonDataUpdated);


    connect(ui->callAddress,static_cast<void (QComboBox::*)(const QString &)>(&QComboBox::activated),this,
            &UpgradeContractDialogCTC::refreshContractAddress);

    //测试升级
    connect(ui->callAddress,static_cast<void (QComboBox::*)(const QString &)>(&QComboBox::activated),this,
            &UpgradeContractDialogCTC::testUpgradeContract);
    connect(ui->contractAddress,static_cast<void (QComboBox::*)(const QString &)>(&QComboBox::activated),this,
            &UpgradeContractDialogCTC::testUpgradeContract);
    connect(ui->contractName,&QLineEdit::textEdited,this,&UpgradeContractDialogCTC::testUpgradeContract);
    connect(ui->contractDes,&QLineEdit::textEdited,this,&UpgradeContractDialogCTC::testUpgradeContract);

    //读取所有账户信息
    connect(DataManagerCTC::getInstance(),&DataManagerCTC::queryAccountFinish,this,&UpgradeContractDialogCTC::InitAccountAddress);
    connect(DataManagerCTC::getInstance(),&DataManagerCTC::queryContractFinish,this,&UpgradeContractDialogCTC::refreshContractAddress);
    DataManagerCTC::getInstance()->queryAccount();
}

void UpgradeContractDialogCTC::InitAccountAddress()
{
    DataManagerStruct::AccountCTC::AccountDataPtr data = DataManagerCTC::getInstance()->getAccount();
    for(auto it = data->getAccount().begin();it != data->getAccount().end();++it)
    {
        ui->callAddress->addItem((*it)->getAccountName());
    }

    //查询合约
    DataManagerCTC::getInstance()->queryContract();
}

void UpgradeContractDialogCTC::refreshContractAddress()
{
    ui->contractAddress->clear();
    DataManagerStruct::AddressContractDataPtr data = DataManagerCTC::getInstance()->getContract();

    DataManagerStruct::AddressContractPtr contract = data->getAddressContract(ui->callAddress->currentText());
    if(!contract) return;
    std::for_each(contract->GetContracts().begin(),contract->GetContracts().end(),[this](DataManagerStruct::ContractInfoPtr info){
        if(info->GetContractName().isEmpty())
        {
            this->ui->contractAddress->addItem(info->GetContractAddr());
        }
    });
}

double UpgradeContractDialogCTC::parseTestUpgrade(const QString &data) const
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
