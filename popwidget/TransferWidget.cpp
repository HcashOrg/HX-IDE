#include "TransferWidget.h"
#include "ui_TransferWidget.h"

#include "ChainIDE.h"
#include "DataManager.h"
#include "IDEUtil.h"

#include "ConvenientOp.h"

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
    if(!ui->checkBox->isChecked())
    {
        ChainIDE::getInstance()->postRPC("transfer-to",IDEUtil::toUbcdHttpJsonFormat("sendtoaddress",QJsonArray()<<
                                         ui->lineEdit->text()<<ui->doubleSpinBox->value()));
    }
    else
    {
        ChainIDE::getInstance()->postRPC("transfer-to",IDEUtil::toUbcdHttpJsonFormat("sendfrom",QJsonArray()<<
                                         ui->comboBox->currentText()<<ui->lineEdit->text()<<ui->doubleSpinBox->value()));
    }
}

void TransferWidget::checkBoxChangeSlot()
{
    ui->comboBox->setEnabled(ui->checkBox->isChecked());
    ui->label_balance->setEnabled(ui->checkBox->isChecked());
}

void TransferWidget::comboBoxChangeSlot()
{
    ui->label_balance->setText(QString::number(ui->comboBox->currentData().toDouble(),'f',8));
    ui->doubleSpinBox->setRange(0,ui->comboBox->currentData().toDouble());
}

void TransferWidget::addressChangeSlot()
{
    ChainIDE::getInstance()->getDataManager()->checkAddress(ui->lineEdit->text());
}

void TransferWidget::addressCheckSlot(bool isvalid)
{
    qDebug()<<isvalid;
    QString enable = DataDefine::Black_Theme == ChainIDE::getInstance()->getCurrentTheme() ? "QLineEdit{color:white;}":"QLineEdit{color:black;}";
    QString disable = "QLineEdit{color:red;}";
    ui->lineEdit->setStyleSheet(isvalid?enable:disable);
}

void TransferWidget::jsonDataUpdated(const QString &id, const QString &data)
{
    if("transfer-to" == id)
    {
        ConvenientOp::ShowSyncCommonDialog(data);
    }
}

void TransferWidget::InitWidget()
{
    //
    setWindowFlags(Qt::FramelessWindowHint | windowFlags());
    ui->checkBox->setChecked(true);
    ui->doubleSpinBox->setDecimals(8);
    ui->doubleSpinBox->setSingleStep(0.001);

    connect(ChainIDE::getInstance()->getDataManager(),&DataManager::addressCheckFinish,this,&TransferWidget::addressCheckSlot);
    connect(ChainIDE::getInstance()->getDataManager(),&DataManager::queryAccountFinish,this,&TransferWidget::InitComboBox);
    ChainIDE::getInstance()->getDataManager()->queryAccount();

    connect(ui->closeBtn,&QToolButton::clicked,this,&QWidget::close);
    connect(ui->cancelBtn,&QToolButton::clicked,this,&QWidget::close);
    connect(ui->okBtn,&QToolButton::clicked,this,&TransferWidget::transferSlot);
    connect(ui->checkBox,&QCheckBox::clicked,this,&TransferWidget::checkBoxChangeSlot);
    connect(ui->lineEdit,&QLineEdit::textEdited,this,&TransferWidget::addressChangeSlot);
    connect(ChainIDE::getInstance(),&ChainIDE::jsonDataUpdated,this,&TransferWidget::jsonDataUpdated);

}

void TransferWidget::InitComboBox()
{
    DataDefine::AccountDataPtr accounts = ChainIDE::getInstance()->getDataManager()->getAccount();
    for(auto it = accounts->getAccount().begin();it != accounts->getAccount().end();++it)
    {
        ui->comboBox->addItem((*it)->getAccountName(),(*it)->getTotalBalance());
    }
    connect(ui->comboBox,static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),this,&TransferWidget::comboBoxChangeSlot);

    if(ui->comboBox->count() >= 1)
    {
        ui->comboBox->setCurrentIndex(0);
    }
}
