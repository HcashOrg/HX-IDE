#include "upgradedialog.h"
#include "ui_upgradedialog.h"

#include "../hxchain.h"

#include <QDebug>
#include <QMessageBox>
#include <QPainter>
#include <QMouseEvent>
#include <QDesktopWidget>

UpgradeDialog::UpgradeDialog( QString contractAddress, QWidget *parent) :
    QDialog(parent),
    address(contractAddress),
    costCalculated(false),
    mouse_press(false),
    ui(new Ui::UpgradeDialog)
{
    ui->setupUi(this);

//    setWindowFlags(  Qt::Dialog | Qt::WindowCloseButtonHint);
//    setWindowTitle( QString::fromLocal8Bit("合约升级"));
    setWindowFlags(Qt::FramelessWindowHint);
    move( (QApplication::desktop()->width() - this->width())/2 , (QApplication::desktop()->height() - this->height())/2);

    connect(HXChain::getInstance(),SIGNAL(jsonDataUpdated(QString)),this,SLOT(jsonDataUpdated(QString)));

    ui->addressLabel->setText(address);

    QString owner = HXChain::getInstance()->contractInfoMap.value(address).owner;
    QString ownerName = HXChain::getInstance()->getAccountNameByOwnerKey(owner);
    ui->ownerAccountLabel->setText(ownerName);

    QRegExp rx2("^([0]|[1-9][0-9]{0,5})(?:\\.\\d{1,5})?$|(^\\t?$)");
    QRegExpValidator *pReg2 = new QRegExpValidator(rx2, this);
    ui->onUpgradeAmountLineEdit->setValidator(pReg2);

    ui->okBtn->setEnabled(false);
}

UpgradeDialog::~UpgradeDialog()
{
    delete ui;
}

void UpgradeDialog::jsonDataUpdated(QString id)
{
    if( id == "id_upgrade_contract_testing_" + address)
    {
        QString result = HXChain::getInstance()->jsonDataValue(id);

        if( result.mid(0,9) == "\"result\":")
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

                QMessageBox::critical(NULL, "", "Upgrade contract testing failed : " + errorMessage, QMessageBox::Ok);

            }
        }
        return;
    }

    if( id == "id_upgrade_contract_" + address )
    {
        QString result = HXChain::getInstance()->jsonDataValue(id);

        if( result.mid(0,9) == "\"result\":")
        {
            HXChain::getInstance()->specialOperationMap.insert(address,"upgrading");
            QMessageBox::information(NULL, "", QString::fromLocal8Bit("升级合约成功!"), QMessageBox::Ok);
            close();
        }
        else
        {
            if( result.mid(0,8) == "\"error\":")
            {
                int pos = result.indexOf("\"message\":\"") + 11;
                QString errorMessage = result.mid(pos, result.indexOf("\"", pos) - pos);

                QMessageBox::critical(NULL, "", QString::fromLocal8Bit("升级合约失败 : ") + errorMessage, QMessageBox::Ok);

            }
        }

    }
}

void UpgradeDialog::on_okBtn_clicked()
{
    HXChain::getInstance()->postRPC(  "id_upgrade_contract_" + address, toJsonFormat("upgrade_contract",
            QJsonArray() << address << ui->ownerAccountLabel->text() << ui->nameLineEdit->text() << ui->descriptionLineEdit->text() << "ALP" << QString::number( ui->onUpgradeAmountLineEdit->text().toDouble()) ));

}

void UpgradeDialog::on_cancelBtn_clicked()
{
    close();
}

void UpgradeDialog::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setPen(QPen(QColor(68,217,199),Qt::SolidLine));
    painter.setBrush(QBrush(QColor(68,217,199),Qt::SolidPattern));
    painter.drawRect(0,0,this->width(),40);
}

void UpgradeDialog::mousePressEvent(QMouseEvent *event)
{

    if(event->button() == Qt::LeftButton)
     {
          mouse_press = true;
          //鼠标相对于窗体的位置（或者使用event->globalPos() - this->pos()）
          move_point = event->pos();
     }

}

void UpgradeDialog::mouseMoveEvent(QMouseEvent *event)
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

void UpgradeDialog::mouseReleaseEvent(QMouseEvent *)
{
    mouse_press = false;
}

void UpgradeDialog::on_closeBtn_clicked()
{
    close();
}

void UpgradeDialog::on_estimateBtn_clicked()
{
    HXChain::getInstance()->postRPC(  "id_upgrade_contract_testing_" + address , toJsonFormat("upgrade_contract_testing",
                                                  QJsonArray() << address << ui->ownerAccountLabel->text() << ui->nameLineEdit->text() << ui->descriptionLineEdit->text() ));
}

void UpgradeDialog::validateOkBtn()
{
    if( !costCalculated)  return;
    if( ui->costLabel->text().isEmpty())  return;
    QString cost = ui->costLabel->text();
    cost.remove(" ALP");
    if( cost.toDouble() > ui->onUpgradeAmountLineEdit->text().toDouble() )
    {
        ui->okBtn->setEnabled(false);
    }
    else
    {
        ui->okBtn->setEnabled(true);
    }
}

void UpgradeDialog::on_onUpgradeAmountLineEdit_textChanged(const QString &arg1)
{
     validateOkBtn();
}
