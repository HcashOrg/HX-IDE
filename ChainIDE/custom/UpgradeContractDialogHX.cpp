#include "UpgradeContractDialogHX.h"
#include "ui_UpgradeContractDialogHX.h"

#include <math.h>
#include <QDebug>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>

#include "datamanager/DataManagerHX.h"
#include "ConvenientOp.h"
#include "ChainIDE.h"
#include "IDEUtil.h"

UpgradeContractDialogHX::UpgradeContractDialogHX(QWidget *parent) :
    MoveableDialog(parent),
    ui(new Ui::UpgradeContractDialogHX)
{
    ui->setupUi(this);
    InitWidget();
}

UpgradeContractDialogHX::~UpgradeContractDialogHX()
{
    delete ui;
}

void UpgradeContractDialogHX::jsonDataUpdated(const QString &id, const QString &data)
{
    if("upgrade_upgrade_contract" == id)
    {
        ConvenientOp::ShowSyncCommonDialog(data);
        close();
    }
    else if("upgrade_upgrade_contract_test" == id)
    {
        double fee = parseTestUpgrade(data);
        ui->gaslimit->setToolTip(tr("approximatefee:%1").arg(QString::number(fee,'f',5)));
        ui->gasprice->setToolTip(tr("approximatefee:%1").arg(QString::number(fee,'f',5)));
    }

}

void UpgradeContractDialogHX::UpgradeContract()
{

    //对信息不足的操作不予执行
    if(ui->callAddress->currentText().isEmpty() || ui->contractAddress->currentText().isEmpty() || ui->contractName->text().isEmpty())
    {
        return;
    }
    ChainIDE::getInstance()->postRPC("upgrade_upgrade_contract",IDEUtil::toJsonFormat("upgrade_contract",QJsonArray()<<
                                     ui->callAddress->currentText()<<ui->gasprice->text()<<ui->gaslimit->text()
                                     <<ui->contractAddress->currentText()<<ui->contractName->text()
                                     <<ui->contractDes->text()));

}

void UpgradeContractDialogHX::testUpgradeContract()
{
    if(ui->contractName->text().isEmpty())
    {
        return;
    }
    ChainIDE::getInstance()->postRPC("upgrade_upgrade_contract_test",IDEUtil::toJsonFormat("upgrade_contract_testing",QJsonArray()<<
                                     ui->callAddress->currentText()<<ui->contractAddress->currentText()
                                     <<ui->contractName->text()<<ui->contractDes->text()
                                     ));
}

void UpgradeContractDialogHX::InitWidget()
{
    ui->gaslimit->setRange(0,999999);
    ui->gaslimit->setSingleStep(1);
    ui->gasprice->setRange(10,999999);

    setWindowFlags(Qt::FramelessWindowHint);

    connect(ui->closeBtn,&QToolButton::clicked,this,&UpgradeContractDialogHX::close);
    connect(ui->cancelBtn,&QToolButton::clicked,this,&UpgradeContractDialogHX::close);
    connect(ui->okBtn,&QToolButton::clicked,this,&UpgradeContractDialogHX::UpgradeContract);

    connect(ChainIDE::getInstance(),&ChainIDE::jsonDataUpdated,this,&UpgradeContractDialogHX::jsonDataUpdated);


    connect(ui->callAddress,static_cast<void (QComboBox::*)(const QString &)>(&QComboBox::activated),this,
            &UpgradeContractDialogHX::refreshContractAddress);


    //测试升级
    connect(ui->callAddress,static_cast<void (QComboBox::*)(const QString &)>(&QComboBox::activated),this,
            &UpgradeContractDialogHX::testUpgradeContract);
    connect(ui->contractAddress,static_cast<void (QComboBox::*)(const QString &)>(&QComboBox::activated),this,
            &UpgradeContractDialogHX::testUpgradeContract);
    connect(ui->contractName,&QLineEdit::textEdited,this,&UpgradeContractDialogHX::testUpgradeContract);
    connect(ui->contractDes,&QLineEdit::textEdited,this,&UpgradeContractDialogHX::testUpgradeContract);

    //读取所有账户信息
    connect(DataManagerHX::getInstance(),&DataManagerHX::queryAccountFinish,this,&UpgradeContractDialogHX::InitAccountAddress);
    connect(DataManagerHX::getInstance(),&DataManagerHX::queryContractFinish,this,&UpgradeContractDialogHX::refreshContractAddress);
    DataManagerHX::getInstance()->queryAccount();

}

void UpgradeContractDialogHX::InitAccountAddress()
{
    DataManagerStruct::AccountHX::AccountDataPtr data = DataManagerHX::getInstance()->getAccount();
    for(auto it = data->getAccount().begin();it != data->getAccount().end();++it)
    {
        ui->callAddress->addItem((*it)->getAccountName());
    }

    //查询合约
    DataManagerHX::getInstance()->queryContract();
}

void UpgradeContractDialogHX::refreshContractAddress()
{
    ui->contractAddress->clear();
    DataManagerStruct::AddressContractDataPtr data = DataManagerHX::getInstance()->getContract();

    DataManagerStruct::AddressContractPtr contract = data->getAddressContract(ui->callAddress->currentText());
    if(!contract) return;
    std::for_each(contract->GetContracts().begin(),contract->GetContracts().end(),[this](DataManagerStruct::ContractInfoPtr info){
        if(info->GetContractName().isEmpty())
        {
            this->ui->contractAddress->addItem(info->GetContractAddr());
        }

    });
}

double UpgradeContractDialogHX::parseTestUpgrade(const QString &data) const
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
