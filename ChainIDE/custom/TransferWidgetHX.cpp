#include "TransferWidgetHX.h"
#include "ui_TransferWidgetHX.h"

#include "ChainIDE.h"
#include "datamanager/DataManagerHX.h"
#include "IDEUtil.h"

#include "ConvenientOp.h"

Q_DECLARE_METATYPE(DataDefine::AccountHX::AccountInfoPtr)
Q_DECLARE_METATYPE(DataDefine::AccountHX::AssetInfoPtr)

TransferWidgetHX::TransferWidgetHX(QWidget *parent) :
    MoveableDialog(parent),
    ui(new Ui::TransferWidgetHX)
{
    ui->setupUi(this);
    InitWidget();
}

TransferWidgetHX::~TransferWidgetHX()
{
    delete ui;
}

void TransferWidgetHX::transferSlot()
{

    ChainIDE::getInstance()->postRPC( "transfer-to",
                                     IDEUtil::toJsonFormat( "transfer_to_address",
                                                   QJsonArray() << ui->comboBox_account->currentText()<< ui->lineEdit->text()
                                                   << ui->doubleSpinBox->text() << ui->comboBox_asset->currentText()
                                                   << "" << true ));

}


void TransferWidgetHX::comboBoxAccountChangeSlot()
{
    ui->comboBox_asset->clear();
    DataDefine::AccountHX::AccountInfoPtr info = ui->comboBox_account->currentData().value<DataDefine::AccountHX::AccountInfoPtr>();
    for(auto it = info->getAssetInfos().begin();it != info->getAssetInfos().end();++it){
        ui->comboBox_asset->addItem((*it)->GetAssetType(),QVariant::fromValue<DataDefine::AccountHX::AssetInfoPtr>(*it));
    }
    if(ui->comboBox_asset->count() >= 1)
    {
        ui->comboBox_asset->setCurrentIndex(0);
    }
}

void TransferWidgetHX::comboBoxAssetChangeSlot()
{
    //设置上限
    if(DataDefine::AccountHX::AssetInfoPtr asset = ui->comboBox_asset->currentData().value<DataDefine::AccountHX::AssetInfoPtr>()){
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

void TransferWidgetHX::jsonDataUpdated(const QString &id, const QString &data)
{
    if("transfer-to" == id)
    {
        ConvenientOp::ShowSyncCommonDialog(data);
        close();
    }
}

void TransferWidgetHX::InitWidget()
{
    //
    setWindowFlags(Qt::FramelessWindowHint | windowFlags());
    ui->doubleSpinBox->setDecimals(8);
    ui->doubleSpinBox->setSingleStep(0.001);
    ui->doubleSpinBox->setMaximum(1e18);

    connect(DataManagerHX::getInstance(),&DataManagerHX::queryAccountFinish,this,&TransferWidgetHX::InitComboBox);

    connect(ui->closeBtn,&QToolButton::clicked,this,&QWidget::close);
    connect(ui->cancelBtn,&QToolButton::clicked,this,&QWidget::close);
    connect(ui->okBtn,&QToolButton::clicked,this,&TransferWidgetHX::transferSlot);
    connect(ChainIDE::getInstance(),&ChainIDE::jsonDataUpdated,this,&TransferWidgetHX::jsonDataUpdated);
    connect(ui->comboBox_account,static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),this,&TransferWidgetHX::comboBoxAccountChangeSlot);
    connect(ui->comboBox_asset,static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),this,&TransferWidgetHX::comboBoxAssetChangeSlot);
    //初始化
    DataManagerHX::getInstance()->queryAccount();
}

void TransferWidgetHX::InitComboBox()
{
    DataDefine::AccountHX::AccountDataPtr accounts = DataManagerHX::getInstance()->getAccount();
    for(auto it = accounts->getAccount().begin();it != accounts->getAccount().end();++it)
    {
        ui->comboBox_account->addItem((*it)->getAccountName(),QVariant::fromValue<DataDefine::AccountHX::AccountInfoPtr>(*it));
    }

    if(ui->comboBox_account->count() >= 1)
    {
        ui->comboBox_account->setCurrentIndex(0);
    }
}
