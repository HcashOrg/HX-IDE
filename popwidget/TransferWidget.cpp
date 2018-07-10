#include "TransferWidget.h"
#include "ui_TransferWidget.h"

#include "ChainIDE.h"
#include "DataManager.h"
#include "IDEUtil.h"

#include "ConvenientOp.h"

Q_DECLARE_METATYPE(DataDefine::AccountInfoPtr)
Q_DECLARE_METATYPE(DataDefine::AssetInfoPtr)

TransferWidget::TransferWidget(QWidget *parent) :
    MoveableDialog(parent),
    ui(new Ui::TransferWidget)
{
    ui->setupUi(this);
    InitWidget();
}

TransferWidget::~TransferWidget()
{
    delete ui;
}

void TransferWidget::transferSlot()
{

    ChainIDE::getInstance()->postRPC( "transfer-to",
                                     IDEUtil::toJsonFormat( "transfer_to_address",
                                                   QJsonArray() << ui->comboBox_account->currentText()<< ui->lineEdit->text()
                                                   << ui->doubleSpinBox->text() << ui->comboBox_asset->currentText()
                                                   << "" << true ));

}


void TransferWidget::comboBoxAccountChangeSlot()
{
    ui->comboBox_asset->clear();
    DataDefine::AccountInfoPtr info = ui->comboBox_account->currentData().value<DataDefine::AccountInfoPtr>();
    for(auto it = info->getAssetInfos().begin();it != info->getAssetInfos().end();++it){
        ui->comboBox_asset->addItem((*it)->GetAssetType(),QVariant::fromValue<DataDefine::AssetInfoPtr>(*it));
    }
    if(ui->comboBox_asset->count() >= 1)
    {
        ui->comboBox_asset->setCurrentIndex(0);
    }
}

void TransferWidget::comboBoxAssetChangeSlot()
{
    //设置上限
    if(DataDefine::AssetInfoPtr asset = ui->comboBox_asset->currentData().value<DataDefine::AssetInfoPtr>()){
        double number = asset->GetBalance()/pow(10,asset->GetPrecision());
        ui->label_balance->setText(QString::number(number,'f',asset->GetPrecision()));
    }
    else{
        ui->label_balance->setText("/");
    }
}

void TransferWidget::jsonDataUpdated(const QString &id, const QString &data)
{
    if("transfer-to" == id)
    {
        ConvenientOp::ShowSyncCommonDialog(data);
        close();
    }
}

void TransferWidget::InitWidget()
{
    //
    setWindowFlags(Qt::FramelessWindowHint | windowFlags());
    ui->doubleSpinBox->setDecimals(8);
    ui->doubleSpinBox->setSingleStep(0.001);

    connect(ChainIDE::getInstance()->getDataManager(),&DataManager::queryAccountFinish,this,&TransferWidget::InitComboBox);
    ChainIDE::getInstance()->getDataManager()->queryAccount();

    connect(ui->closeBtn,&QToolButton::clicked,this,&QWidget::close);
    connect(ui->cancelBtn,&QToolButton::clicked,this,&QWidget::close);
    connect(ui->okBtn,&QToolButton::clicked,this,&TransferWidget::transferSlot);
    connect(ChainIDE::getInstance(),&ChainIDE::jsonDataUpdated,this,&TransferWidget::jsonDataUpdated);
    connect(ui->comboBox_account,static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),this,&TransferWidget::comboBoxAccountChangeSlot);
    connect(ui->comboBox_asset,static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),this,&TransferWidget::comboBoxAssetChangeSlot);

}

void TransferWidget::InitComboBox()
{
    DataDefine::AccountDataPtr accounts = ChainIDE::getInstance()->getDataManager()->getAccount();
    for(auto it = accounts->getAccount().begin();it != accounts->getAccount().end();++it)
    {
        ui->comboBox_account->addItem((*it)->getAccountName(),QVariant::fromValue<DataDefine::AccountInfoPtr>(*it));
    }

    if(ui->comboBox_account->count() >= 1)
    {
        ui->comboBox_account->setCurrentIndex(0);
    }
}
