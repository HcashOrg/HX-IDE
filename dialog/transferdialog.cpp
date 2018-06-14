#include "transferdialog.h"
#include "ui_transferdialog.h"

#include "../hxchain.h"

#include <QDebug>
#include <QMessageBox>
#include <QPainter>
#include <QMouseEvent>
#include <QDesktopWidget>

TransferDialog::TransferDialog(QString contractAddress, QWidget *parent) :
    QDialog(parent),
    costCalculated(false),
    mouse_press(false),
    ui(new Ui::TransferDialog)
{
    ui->setupUi(this);

//    setWindowFlags(  Qt::Dialog | Qt::WindowCloseButtonHint);
//    setWindowTitle( QString::fromLocal8Bit("合约充值"));
    setWindowFlags(Qt::FramelessWindowHint);
    move( (QApplication::desktop()->width() - this->width())/2 , (QApplication::desktop()->height() - this->height())/2);

    connect(HXChain::getInstance(),SIGNAL(jsonDataUpdated(QString)),this,SLOT(jsonDataUpdated(QString)));


    if( !contractAddress.isEmpty())
    {
        ui->addressOrNameLineEdit->setText(contractAddress);
    }


    if( !HXChain::getInstance()->currentAccount.isEmpty())
    {
        ui->currentAccountLabel->setText( HXChain::getInstance()->currentAccount);
        ui->balanceLabel->setText( HXChain::getInstance()->accountInfoMap.value( HXChain::getInstance()->currentAccount).balance );
    }


    QRegExp rx("^([0]|[1-9][0-9]{0,5})(?:\\.\\d{1,5})?$|(^\\t?$)");
    QRegExpValidator *pReg = new QRegExpValidator(rx, this);
    ui->amountLineEdit->setValidator(pReg);

    QRegExp rx2("^([0]|[1-9][0-9]{0,5})(?:\\.\\d{1,5})?$|(^\\t?$)");
    QRegExpValidator *pReg2 = new QRegExpValidator(rx2, this);
    ui->onDepositAmountLineEdit->setValidator(pReg2);

    ui->okBtn->setEnabled(false);
}

TransferDialog::~TransferDialog()
{
    delete ui;
}

void TransferDialog::updateAccountBalance()
{
    if( !HXChain::getInstance()->currentAccount.isEmpty())
    {
        ui->currentAccountLabel->setText( HXChain::getInstance()->currentAccount);
        ui->balanceLabel->setText( HXChain::getInstance()->accountInfoMap.value( HXChain::getInstance()->currentAccount).balance );
    }
}

void TransferDialog::contractInfoUpdated(QString addressOrName)
{
    if( addressOrName != ui->addressOrNameLineEdit->text() )  return;

    if( addressOrName.startsWith("CON"))
    {
        // 如果是地址  名称或地址项显示合约名称
        ui->nameOrAddressLabel->setText( HXChain::getInstance()->contractInfoMap.value(addressOrName).name);
    }
    else
    {
        // 如果是名称  名称或地址项显示合约地址
        ui->nameOrAddressLabel->setText( HXChain::getInstance()->contractInfoMap.value(addressOrName).address);
    }

    ui->descriptionLabel->setText( HXChain::getInstance()->contractInfoMap.value(addressOrName).description);
}

void TransferDialog::jsonDataUpdated(QString id)
{
    if( id == "id_wallet_transfer_to_contract_" + ui->addressOrNameLineEdit->text())
    {
        QString result = HXChain::getInstance()->jsonDataValue(id);

        if( result.mid(0,9) == "\"result\":")
        {
            HXChain::getInstance()->postRPC(  "id_wallet_account_balance", toJsonFormat("wallet_account_balance", QJsonArray()));
            QMessageBox::information(NULL, "", "Transfer to contract succeed.", QMessageBox::Ok);
            close();
        }
        else
        {
            if( result.mid(0,8) == "\"error\":")
            {
                int pos = result.indexOf("\"message\":\"") + 11;
                QString errorMessage = result.mid(pos, result.indexOf("\"", pos) - pos);

                QMessageBox::critical(NULL, "", "Transfer to contract fail : " + errorMessage, QMessageBox::Ok);

            }
        }

        return;
    }

    if( id == "id_wallet_transfer_to_contract_testing_" + ui->addressOrNameLineEdit->text())
    {
        QString result = HXChain::getInstance()->jsonDataValue(id);

        if( result.mid(0,9) == "\"result\":")
        {
            int pos = result.indexOf("\"amount\":") + 9;
            pos = result.indexOf("\"amount\":", pos) + 9;
            pos = result.indexOf("\"amount\":", pos) + 9;   // 第三个amount 是执行花费
            QString costForExecute = result.mid(pos, result.indexOf(",", pos) - pos);
            costForExecute.remove('\"');

            double baseCost = ( costForExecute.toDouble()) / 100000.0;
            ui->costLabel->setText( QString::number( baseCost, 'f', 5) + " ALP");

            costCalculated = true;
            validateOkBtn();
        }
        else
        {
            if( result.mid(0,8) == "\"error\":")
            {
                int pos = result.indexOf("\"message\":\"") + 11;
                QString errorMessage = result.mid(pos, result.indexOf("\"", pos) - pos);

                QMessageBox::critical(NULL, "", "Transfer to contract testing fail : " + errorMessage, QMessageBox::Ok);

            }
        }

        return;
    }
}

void TransferDialog::on_okBtn_clicked()
{
    HXChain::getInstance()->postRPC(  "id_wallet_transfer_to_contract_" + ui->addressOrNameLineEdit->text(), toJsonFormat("wallet_transfer_to_contract",
                           QJsonArray() << ui->amountLineEdit->text() << "ALP" << HXChain::getInstance()->currentAccount << ui->addressOrNameLineEdit->text() << ui->onDepositAmountLineEdit->text() ));
}

void TransferDialog::on_cancelBtn_clicked()
{
    close();
}

void TransferDialog::on_estimateBtn_clicked()
{
    if( ui->addressOrNameLineEdit->text().isEmpty())  return;
    if( ui->amountLineEdit->text().isEmpty())   return;

    if( ui->amountLineEdit->text().toDouble() < 0.00001)
    {
        QMessageBox::critical(NULL, "", QString::fromLocal8Bit("金额不能为0!"), QMessageBox::Ok);
        return;
    }

    HXChain::getInstance()->postRPC(  "id_wallet_transfer_to_contract_testing_" + ui->addressOrNameLineEdit->text() , toJsonFormat("wallet_transfer_to_contract_testing",
                           QJsonArray() << ui->amountLineEdit->text() << "ALP" << HXChain::getInstance()->currentAccount << ui->addressOrNameLineEdit->text() << ui->onDepositAmountLineEdit->text() ));

}

void TransferDialog::validateOkBtn()
{
    if( !costCalculated)  return;
    if( ui->costLabel->text().isEmpty())  return;
    QString cost = ui->costLabel->text();
    cost.remove(" ALP");
    if( cost.toDouble() > ui->onDepositAmountLineEdit->text().toDouble() )
    {
        ui->okBtn->setEnabled(false);
    }
    else
    {
        ui->okBtn->setEnabled(true);
    }
}

void TransferDialog::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setPen(QPen(QColor(68,217,199),Qt::SolidLine));
    painter.setBrush(QBrush(QColor(68,217,199),Qt::SolidPattern));
    painter.drawRect(0,0,this->width(),40);
}

void TransferDialog::on_addressOrNameLineEdit_textChanged(const QString &arg1)
{
    ui->okBtn->setEnabled(false);
    ui->costLabel->clear();
    costCalculated = false;
    ui->nameOrAddressLabel->clear();
    ui->descriptionLabel->clear();

    HXChain::getInstance()->postRPC(  "id_get_contract_info_" + ui->addressOrNameLineEdit->text(), toJsonFormat("get_contract_info", QJsonArray() << ui->addressOrNameLineEdit->text() ));

}

void TransferDialog::on_amountLineEdit_textChanged(const QString &arg1)
{
    ui->okBtn->setEnabled(false);
    ui->costLabel->clear();
    costCalculated = false;
}

void TransferDialog::on_closeBtn_clicked()
{
    close();
}

void TransferDialog::mousePressEvent(QMouseEvent *event)
{

    if(event->button() == Qt::LeftButton)
     {
          mouse_press = true;
          //鼠标相对于窗体的位置（或者使用event->globalPos() - this->pos()）
          move_point = event->pos();
     }

}

void TransferDialog::mouseMoveEvent(QMouseEvent *event)
{

    //若鼠标左键被按下
    if(mouse_press)
    {
        //鼠标相对于屏幕的位置
        QPoint move_pos = event->globalPos();

        //移动主窗体位置
        this->move(move_pos - move_point);
    }

}

void TransferDialog::mouseReleaseEvent(QMouseEvent *)
{
    mouse_press = false;
}

void TransferDialog::on_onDepositAmountLineEdit_textChanged(const QString &arg1)
{
    validateOkBtn();
}
