#include "withdrawdialog.h"
#include "ui_withdrawdialog.h"

#include "../hxchain.h"

#include <QDebug>
#include <QMessageBox>
#include <QPainter>
#include <QMouseEvent>
#include <QDesktopWidget>

WithdrawDialog::WithdrawDialog( QString contractAddress, QWidget *parent) :
    QDialog(parent),
    address(contractAddress),
    costCalculated(false),
    mouse_press(false),
    ui(new Ui::WithdrawDialog)
{
    ui->setupUi(this);

//    setWindowFlags(  Qt::Dialog | Qt::WindowCloseButtonHint);
//    setWindowTitle( QString::fromLocal8Bit("合约销毁"));
    setWindowFlags(Qt::FramelessWindowHint);
    move( (QApplication::desktop()->width() - this->width())/2 , (QApplication::desktop()->height() - this->height())/2);

    connect(HXChain::getInstance(),SIGNAL(jsonDataUpdated(QString)),this,SLOT(jsonDataUpdated(QString)));

    ui->addressLabel->setText(address);

    // get_contract_info 获得的ownerName 必须是注册账户  否则为空 所以还是通过公钥（owner）去查询
    QString owner = HXChain::getInstance()->contractInfoMap.value(address).owner;
    ownerName = HXChain::getInstance()->getAccountNameByOwnerKey(owner);
    ui->accountLabel->setText(ownerName);

    if( HXChain::getInstance()->isInSandBox || HXChain::getInstance()->currenChain() == 1)
    {
        ui->pwdLineEdit->hide();
        ui->pwdLabel->hide();
    }

    QRegExp rx2("^([0]|[1-9][0-9]{0,5})(?:\\.\\d{1,5})?$|(^\\t?$)");
    QRegExpValidator *pReg2 = new QRegExpValidator(rx2, this);
    ui->onDestroyAmountLineEdit->setValidator(pReg2);

    ui->okBtn->setEnabled(false);

}

WithdrawDialog::~WithdrawDialog()
{
    delete ui;
}

void WithdrawDialog::on_okBtn_clicked()
{
    if( !HXChain::getInstance()->isInSandBox && HXChain::getInstance()->currenChain() != 1)
    {
        QString pwd = ui->pwdLineEdit->text();
        pwd = pwd.simplified();
        if( pwd.isEmpty())
        {
            QMessageBox::warning(NULL, "", QString::fromLocal8Bit("请输入钱包密码!"), QMessageBox::Ok);
            return;
        }

        if( QMessageBox::Yes != QMessageBox::information(NULL, "", QString::fromLocal8Bit("确定销毁该合约?"), QMessageBox::Yes | QMessageBox::No))
        {
            return;
        }

        HXChain::getInstance()->postRPC(  "id_wallet_check_passphrase_withdrawDialog",toJsonFormat( "wallet_check_passphrase",
                                                      QJsonArray() << pwd));
    }
    else
    {
        if( QMessageBox::Yes != QMessageBox::information(NULL, "", QString::fromLocal8Bit("确定销毁该合约?"), QMessageBox::Yes | QMessageBox::No))
        {
            return;
        }

        HXChain::getInstance()->postRPC(  "id_destroy_contract_" + address,toJsonFormat( "destroy_contract",
                                                      QJsonArray() << address << ownerName << "ALP" << QString::number( ui->onDestroyAmountLineEdit->text().toDouble()) ));
    }

}

void WithdrawDialog::jsonDataUpdated(QString id)
{
    if( id == "id_destroy_contract_testing_" + address)
    {
        QString result = HXChain::getInstance()->jsonDataValue(id);

        if( result.startsWith("\"result\":"))
        {
            int pos = result.indexOf("\"amount\":") + 9;
            pos = result.indexOf("\"amount\":", pos) + 9;   // 第2个amount 是执行花费
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

                QMessageBox::critical(NULL, "", "Destroy contract testing failed : " + errorMessage, QMessageBox::Ok);
            }
        }

        return;
    }

    if( id == "id_wallet_check_passphrase_withdrawDialog")
    {
        QString result = HXChain::getInstance()->jsonDataValue(id);

        if( result == "\"result\":true")
        {
            HXChain::getInstance()->postRPC(  "id_destroy_contract_" + address, toJsonFormat("destroy_contract",
                                                          QJsonArray() << address << ownerName << "ALP" << QString::number( ui->onDestroyAmountLineEdit->text().toDouble())));
        }
        else if( result == "\"result\":false")
        {
            QMessageBox::critical(NULL, "Error", QString::fromLocal8Bit("错误的密码!"), QMessageBox::Ok);
        }
        else
        {
            if( result.mid(0,8) == "\"error\":")
            {
                int pos = result.indexOf("\"message\":\"") + 11;
                QString errorMessage = result.mid(pos, result.indexOf("\"", pos) - pos);

                QMessageBox::critical(NULL, "", "Check_passphrase fail : " + errorMessage, QMessageBox::Ok);
            }
        }

        return;
    }

    if( id == "id_destroy_contract_" + address)
    {
        QString result = HXChain::getInstance()->jsonDataValue(id);

        if( result.startsWith("\"result\":"))
        {
            emit contractDestroyed(address);
            QMessageBox::information(NULL, "", QString::fromLocal8Bit("销毁合约成功!"), QMessageBox::Ok);
            close();
        }
        else
        {
            if( result.mid(0,8) == "\"error\":")
            {
                int pos = result.indexOf("\"message\":\"") + 11;
                QString errorMessage = result.mid(pos, result.indexOf("\"", pos) - pos);

                QMessageBox::critical(NULL, "", QString::fromLocal8Bit("销毁合约失败!") + errorMessage, QMessageBox::Ok);
            }
        }

        return;
    }
}

void WithdrawDialog::on_pwdLineEdit_returnPressed()
{
    on_okBtn_clicked();
}

void WithdrawDialog::on_closeBtn_clicked()
{
    close();
}

void WithdrawDialog::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setPen(QPen(QColor(68,217,199),Qt::SolidLine));
    painter.setBrush(QBrush(QColor(68,217,199),Qt::SolidPattern));
    painter.drawRect(0,0,this->width(),40);
}

void WithdrawDialog::mousePressEvent(QMouseEvent *event)
{

    if(event->button() == Qt::LeftButton)
     {
          mouse_press = true;
          //鼠标相对于窗体的位置（或者使用event->globalPos() - this->pos()）
          move_point = event->pos();
     }

}

void WithdrawDialog::mouseMoveEvent(QMouseEvent *event)
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

void WithdrawDialog::mouseReleaseEvent(QMouseEvent *)
{
    mouse_press = false;
}

void WithdrawDialog::on_estimateBtn_clicked()
{
    HXChain::getInstance()->postRPC(  "id_destroy_contract_testing_" + address , toJsonFormat("destroy_contract_testing",
                                                  QJsonArray() << address << ownerName ));

}

void WithdrawDialog::on_onDestroyAmountLineEdit_textChanged(const QString &arg1)
{
    validateOkBtn();
}

void WithdrawDialog::validateOkBtn()
{
    if( !costCalculated)  return;
    if( ui->costLabel->text().isEmpty())  return;
    QString cost = ui->costLabel->text();
    cost.remove(" ALP");
    if( cost.toDouble() > ui->onDestroyAmountLineEdit->text().toDouble() )
    {
        ui->okBtn->setEnabled(false);
    }
    else
    {
        ui->okBtn->setEnabled(true);
    }
}

void WithdrawDialog::on_cancelBtn_clicked()
{
    close();
}
