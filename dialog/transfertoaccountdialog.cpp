#include "transfertoaccountdialog.h"
#include "ui_transfertoaccountdialog.h"

#include "../hxchain.h"

#include <QDebug>
#include <QPainter>
#include <QMouseEvent>
#include <QListView>
#include <QDesktopWidget>
#include <QMessageBox>
#include <QTextCodec>

TransferToAccountDialog::TransferToAccountDialog(QWidget *parent) :
    QDialog(parent),
    mouse_press(false),
    ui(new Ui::TransferToAccountDialog)
{
    ui->setupUi(this);

    setWindowFlags(Qt::FramelessWindowHint);
    move( (QApplication::desktop()->width() - this->width())/2 , (QApplication::desktop()->height() - this->height())/2);

    connect(HXChain::getInstance(),SIGNAL(jsonDataUpdated(QString)),this,SLOT(jsonDataUpdated(QString)));

    inited = false;

    QStringList accountList = HXChain::getInstance()->accountInfoMap.keys();

    if( accountList.size() > 0)
    {
        ui->accountComboBox->addItems( accountList);

        if( HXChain::getInstance()->currentAccount.isEmpty())
        {
            // 默认是 map中第一个账户
//            HXChain::getInstance()->currentAccount = accountList.at(0);
            HXChain::getInstance()->setCurrentAccount(accountList.at(0));
        }

        ui->accountComboBox->setCurrentText( HXChain::getInstance()->currentAccount);
        inited = true;

        on_accountComboBox_currentIndexChanged( HXChain::getInstance()->currentAccount);
    }

    ui->accountComboBox->setView(new QListView());   // 设置下拉框每项高度有问题 光用qss不行

    QRegExp rx("^([0]|[1-9][0-9]{0,5})(?:\\.\\d{1,5})?$|(^\\t?$)");
    QRegExpValidator *pReg = new QRegExpValidator(rx, this);
    ui->amountLineEdit->setValidator(pReg);

    ui->okBtn->setEnabled(false);

    ui->tipLabel->hide();
}

TransferToAccountDialog::~TransferToAccountDialog()
{
    delete ui;
}

void TransferToAccountDialog::mousePressEvent(QMouseEvent *event)
{

    if(event->button() == Qt::LeftButton)
     {
          mouse_press = true;
          //鼠标相对于窗体的位置（或者使用event->globalPos() - this->pos()）
          move_point = event->pos();
     }

}

void TransferToAccountDialog::mouseMoveEvent(QMouseEvent *event)
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

void TransferToAccountDialog::mouseReleaseEvent(QMouseEvent *)
{
    mouse_press = false;
}

void TransferToAccountDialog::updateAccountBalance()
{
    if( !ui->accountComboBox->currentText().isEmpty())
    {
        ui->balanceLabel->setText( HXChain::getInstance()->accountInfoMap.value( ui->accountComboBox->currentText()).balance );
        validateOkBtn();
    }
}

void TransferToAccountDialog::jsonDataUpdated(QString id)
{
    if( id == "id_wallet_transfer_to_address" || id == "id_wallet_transfer_to_public_account")
    {
        QString result = HXChain::getInstance()->jsonDataValue(id);
        qDebug() << result;

        if( result.mid(0,9) == "\"result\":")
        {
            HXChain::getInstance()->postRPC(  "id_wallet_account_balance", toJsonFormat("wallet_account_balance", QJsonArray()));
            QMessageBox::information(NULL, "", QString::fromLocal8Bit("转账成功!"), QMessageBox::Ok);
        }
        else
        {
            if( result.mid(0,8) == "\"error\":")
            {
                int pos = result.indexOf("\"message\":\"") + 11;
                QString errorMessage = result.mid(pos, result.indexOf("\"", pos) - pos);

                if( errorMessage == "Out of Range")
                {
                    QMessageBox::critical(NULL, "", QString::fromLocal8Bit("转账失败: ") + QString::fromLocal8Bit("错误的地址!"), QMessageBox::Ok);
                }
                else if( errorMessage == "Assert Exception")
                {
                    QMessageBox::critical(NULL, "", QString::fromLocal8Bit("转账失败: ") + QString::fromLocal8Bit("错误的地址!"), QMessageBox::Ok);
                }
                else
                {
                    QMessageBox::critical(NULL, "", QString::fromLocal8Bit("转账失败: ") + errorMessage, QMessageBox::Ok);
                }

            }
        }

        return;
    }
}

void TransferToAccountDialog::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setPen(QPen(QColor(68,217,199),Qt::SolidLine));
    painter.setBrush(QBrush(QColor(68,217,199),Qt::SolidPattern));
    painter.drawRect(0,0,this->width(),40);
}

void TransferToAccountDialog::validateOkBtn()
{
    if( ui->accountComboBox->currentText().isEmpty())
    {
        ui->okBtn->setEnabled(false);
        return;
    }

    QString balance = ui->balanceLabel->text();
    balance.remove(" ALP");
    if( balance.toDouble() < 0.000001 )
    {
        ui->okBtn->setEnabled(false);
        return;
    }

    QString sendTo = ui->sendToLineEdit->text().simplified();
    if( sendTo.isEmpty())
    {
        ui->okBtn->setEnabled(false);
        return;
    }

    QString amount = ui->amountLineEdit->text();
    if( amount.isEmpty() || amount.toDouble() < 0.000001 || amount.toDouble() > balance.toDouble())
    {
        ui->okBtn->setEnabled(false);
        return;
    }

    if( amount.toDouble() + 0.01 > balance.toDouble())
    {
        ui->okBtn->setEnabled(false);
        return;
    }

    QTextCodec* utfCodec = QTextCodec::codecForName("UTF-8");
    QByteArray ba = utfCodec->fromUnicode(ui->remarkLineEdit->text());
    if( ba.size() > 40)
    {
        ui->okBtn->setEnabled(false);
        return;
    }

    ui->okBtn->setEnabled(true);

}

void TransferToAccountDialog::on_accountComboBox_currentIndexChanged(const QString &arg1)
{
    if( !inited)  return;
    ui->addressLabel->setText( HXChain::getInstance()->accountInfoMap.value(arg1).address);
    ui->balanceLabel->setText( HXChain::getInstance()->accountInfoMap.value(arg1).balance);

    validateOkBtn();
}

void TransferToAccountDialog::on_okBtn_clicked()
{
    if( ui->sendToLineEdit->text().isEmpty())   return;
    QString cmd;
    if( ui->sendToLineEdit->text().startsWith("ALP"))
    {
        cmd = "wallet_transfer_to_address";
    }
    else
    {
        cmd = "wallet_transfer_to_public_account";
    }

    HXChain::getInstance()->postRPC(  "id_" + cmd, toJsonFormat(cmd,
            QJsonArray() << ui->amountLineEdit->text() << "ALP" << ui->accountComboBox->currentText()
            << ui->sendToLineEdit->text() << ui->remarkLineEdit->text() << "vote_none" ));

}

void TransferToAccountDialog::on_sendToLineEdit_textChanged(const QString &arg1)
{
    validateOkBtn();
}

void TransferToAccountDialog::on_amountLineEdit_textChanged(const QString &arg1)
{
    validateOkBtn();
}

void TransferToAccountDialog::on_closeBtn_clicked()
{
    close();
}

void TransferToAccountDialog::on_remarkLineEdit_textChanged(const QString &arg1)
{
    QTextCodec* utfCodec = QTextCodec::codecForName("UTF-8");
    QByteArray ba = utfCodec->fromUnicode(ui->remarkLineEdit->text());
    if( ba.size() > 40)
    {
        ui->tipLabel->show();
    }
    else
    {
        ui->tipLabel->hide();
    }

    validateOkBtn();
}

void TransferToAccountDialog::on_cancelBtn_clicked()
{
    close();
}
