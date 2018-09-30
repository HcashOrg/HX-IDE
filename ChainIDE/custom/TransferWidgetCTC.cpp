#include "TransferWidgetCTC.h"
#include "ui_TransferWidgetCTC.h"

#include <math.h>
#include "ChainIDE.h"
#include "datamanager/DataManagerCTC.h"
#include "IDEUtil.h"

#include "ConvenientOp.h"

Q_DECLARE_METATYPE(DataManagerStruct::AccountCTC::AccountInfoPtr)
Q_DECLARE_METATYPE(DataManagerStruct::AccountCTC::AssetInfoPtr)

TransferWidgetCTC::TransferWidgetCTC(QWidget *parent) :
    MoveableDialog(parent),
    ui(new Ui::TransferWidgetCTC)
{
    ui->setupUi(this);
    InitWidget();
}

TransferWidgetCTC::~TransferWidgetCTC()
{
    delete ui;
}

void TransferWidgetCTC::transferSlot()
{
    ChainIDE::getInstance()->postRPC( "transfer-to",
                                     IDEUtil::toJsonFormat( "wallet_transfer_to_address",
                                                   QJsonArray() << ui->doubleSpinBox->text()<< ui->comboBox_asset->currentText()
                                                                << ui->comboBox_account->currentText()<< ui->lineEdit->text()
                                                                <<""<<" ",true));
}

void TransferWidgetCTC::comboBoxAccountChangeSlot()
{
    ui->comboBox_asset->clear();
    DataManagerStruct::AccountCTC::AccountInfoPtr info = ui->comboBox_account->currentData().value<DataManagerStruct::AccountCTC::AccountInfoPtr>();
    for(auto it = info->getAssetInfos().begin();it != info->getAssetInfos().end();++it){
        ui->comboBox_asset->addItem((*it)->GetAssetType(),QVariant::fromValue<DataManagerStruct::AccountCTC::AssetInfoPtr>(*it));
    }
    if(ui->comboBox_asset->count() >= 1)
    {
        ui->comboBox_asset->setCurrentIndex(0);
    }
}

void TransferWidgetCTC::comboBoxAssetChangeSlot()
{
    //设置上限
    if(DataManagerStruct::AccountCTC::AssetInfoPtr asset = ui->comboBox_asset->currentData().value<DataManagerStruct::AccountCTC::AssetInfoPtr>()){
        double number = asset->GetBalance()/pow(10,asset->GetPrecision());
        ui->label_balance->setText(QString::number(number,'f',asset->GetPrecision()));
        ui->doubleSpinBox->setDecimals(asset->GetPrecision());
        ui->doubleSpinBox->setRange(0,number);
    }
    else{
        ui->label_balance->setText("/");
        ui->doubleSpinBox->setRange(0,0);
    }
}

void TransferWidgetCTC::jsonDataUpdated(const QString &id, const QString &data)
{
    if("transfer-to" == id)
    {
        ConvenientOp::ShowSyncCommonDialog(data);
        close();
    }
}

void TransferWidgetCTC::InitWidget()
{
    //
    setWindowFlags(Qt::FramelessWindowHint | windowFlags());
    ui->doubleSpinBox->setDecimals(8);
    ui->doubleSpinBox->setSingleStep(0.001);
    ui->doubleSpinBox->setMaximum(1e18);

    connect(DataManagerCTC::getInstance(),&DataManagerCTC::queryAccountFinish,this,&TransferWidgetCTC::InitComboBox);

    connect(ui->closeBtn,&QToolButton::clicked,this,&QWidget::close);
    connect(ui->cancelBtn,&QToolButton::clicked,this,&QWidget::close);
    connect(ui->okBtn,&QToolButton::clicked,this,&TransferWidgetCTC::transferSlot);
    connect(ChainIDE::getInstance(),&ChainIDE::jsonDataUpdated,this,&TransferWidgetCTC::jsonDataUpdated);
    connect(ui->comboBox_account,static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),this,&TransferWidgetCTC::comboBoxAccountChangeSlot);
    connect(ui->comboBox_asset,static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),this,&TransferWidgetCTC::comboBoxAssetChangeSlot);
    //初始化
    DataManagerCTC::getInstance()->queryAccount();
}

void TransferWidgetCTC::InitComboBox()
{
    DataManagerStruct::AccountCTC::AccountDataPtr accounts = DataManagerCTC::getInstance()->getAccount();
    for(auto it = accounts->getAccount().begin();it != accounts->getAccount().end();++it)
    {
        ui->comboBox_account->addItem((*it)->getAccountName(),QVariant::fromValue<DataManagerStruct::AccountCTC::AccountInfoPtr>(*it));
    }

    if(ui->comboBox_account->count() >= 1)
    {
        ui->comboBox_account->setCurrentIndex(0);
    }
}
