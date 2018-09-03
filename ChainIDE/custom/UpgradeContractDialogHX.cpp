#include "UpgradeContractDialogHX.h"
#include "ui_UpgradeContractDialogHX.h"

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

}

void UpgradeContractDialogHX::UpgradeContract()
{

    ChainIDE::getInstance()->postRPC("upgrade_upgrade_contract",IDEUtil::toJsonFormat("upgrade_contract",QJsonArray()<<
                                     ui->callAddress->currentText()<<ui->gasprice->text()<<ui->gaslimit->text()
                                     <<ui->contractAddress->currentText()<<ui->contractName->text()
                                     <<ui->contractDes->text()));

}

void UpgradeContractDialogHX::InitWidget()
{
    ui->gaslimit->setRange(0,999999);
    ui->gaslimit->setSingleStep(1);
    ui->gasprice->setRange(10,999999);

    setWindowFlags(Qt::FramelessWindowHint);

    //读取所有账户信息
    connect(DataManagerHX::getInstance(),&DataManagerHX::queryAccountFinish,this,&UpgradeContractDialogHX::InitAccountAddress);
    connect(DataManagerHX::getInstance(),&DataManagerHX::queryContractFinish,this,&UpgradeContractDialogHX::refreshContractAddress);
    DataManagerHX::getInstance()->queryAccount();

    connect(ui->closeBtn,&QToolButton::clicked,this,&UpgradeContractDialogHX::close);
    connect(ui->cancelBtn,&QToolButton::clicked,this,&UpgradeContractDialogHX::close);
    connect(ui->okBtn,&QToolButton::clicked,this,&UpgradeContractDialogHX::UpgradeContract);

    connect(ChainIDE::getInstance(),&ChainIDE::jsonDataUpdated,this,&UpgradeContractDialogHX::jsonDataUpdated);


    connect(ui->callAddress,static_cast<void (QComboBox::*)(const QString &)>(&QComboBox::activated),this,
            &UpgradeContractDialogHX::refreshContractAddress);
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
        this->ui->contractAddress->addItem(info->GetContractName().isEmpty()?info->GetContractAddr():info->GetContractName());
    });
}
