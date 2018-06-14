#include "calldialog.h"
#include "ui_calldialog.h"

#include "../hxchain.h"

#include <QDebug>
#include <QMessageBox>
#include <QPainter>
#include <QMouseEvent>
#include <QListView>
#include <QDesktopWidget>

CallDialog::CallDialog(QString contractAddressOrName, QWidget *parent) :
    QDialog(parent),
    costCalculated(false),
    mouse_press(false),
    ui(new Ui::CallDialog)
{
    ui->setupUi(this);

//    setWindowFlags(  Qt::Dialog | Qt::WindowCloseButtonHint);
//    setWindowTitle(QString::fromLocal8Bit("合约调用"));
    setWindowFlags(Qt::FramelessWindowHint);
    move( (QApplication::desktop()->width() - this->width())/2 , (QApplication::desktop()->height() - this->height())/2);

    connect(HXChain::getInstance(),SIGNAL(jsonDataUpdated(QString)),this,SLOT(jsonDataUpdated(QString)));

    if( !contractAddressOrName.isEmpty())
    {
        ui->contractLineEdit->setText(contractAddressOrName);
    }

    ui->okBtn->setEnabled(false);
    QRegExp rx("^([0]|[1-9][0-9]{0,5})(?:\\.\\d{1,5})?$|(^\\t?$)");
    QRegExpValidator *pReg = new QRegExpValidator(rx, this);
    ui->maxCostLineEdit->setValidator(pReg);

    ui->functionComboBox->setView(new QListView());   // 设置下拉框每项高度有问题 光用qss不行

}

CallDialog::~CallDialog()
{
    delete ui;
}

void CallDialog::contractInfoUpdated(QString contract)
{
    if( contract != ui->contractLineEdit->text() )  return;

    QStringList abiList = HXChain::getInstance()->contractInfoMap.value(contract).abiList;
    abiList.removeAll("init");
    abiList.removeAll("on_deposit");
    abiList.removeAll("on_upgrade");
    abiList.removeAll("on_destroy");
    ui->functionComboBox->clear();
    ui->functionComboBox->addItems( abiList);
}

void CallDialog::on_estimateBtn_clicked()
{
    if( HXChain::getInstance()->currentAccount.isEmpty())  return;
    if( ui->functionComboBox->currentText().isEmpty())  return;

    HXChain::getInstance()->postRPC(  "id_call_contract_testing" , toJsonFormat("call_contract_testing",
                                                  QJsonArray() << ui->contractLineEdit->text() << HXChain::getInstance()->currentAccount << ui->functionComboBox->currentText() << ui->parameterLineEdit->text()));
//    qDebug() << "111111111111111111 " << toJsonFormat( "id_call_contract_testing" , "call_contract_testing",
//                                                       QStringList() << ui->contractLineEdit->text() << HXChain::getInstance()->currentAccount << ui->functionComboBox->currentText() << ui->parameterLineEdit->text());

}

void CallDialog::jsonDataUpdated(QString id)
{
    if( id == "id_call_contract_testing")
    {
        QString result = HXChain::getInstance()->jsonDataValue(id);
//qDebug() << "id_call_contract_testing";
//qDebug() << result;

        if( result.startsWith("\"result\":"))
        {
            int pos = result.indexOf("\"amount\":") + 9;
//            QString fee = result.mid(pos, result.indexOf(",", pos) - pos);
//            fee.remove('\"');

            pos = result.indexOf("\"amount\":", pos) + 9;
            QString costForExecute = result.mid(pos, result.indexOf(",", pos) - pos);
            costForExecute.remove('\"');

            double baseCost = ( costForExecute.toDouble()) / 100000.0;
            ui->costLabel->setText( QString::number( baseCost, 'f', 5) + " ALP");

            costCalculated = true;
            validateOkBtn();
       }
        else
        {
            QMessageBox::critical(NULL, "Error", "Call contract failed : " + result, QMessageBox::Ok);
//            close();
        }

        return;
    }

    if( id == "id_call_contract")
    {
        QString result = HXChain::getInstance()->jsonDataValue(id);
        qDebug() << "call_contract :" <<  id << result;

        if( result.startsWith("\"result\":"))
        {
            QMessageBox::information(NULL, "", QString::fromLocal8Bit("调用合约成功!"), QMessageBox::Ok);
            close();
        }
        else
        {
            if( result.mid(0,8) == "\"error\":")
            {
                int pos = result.indexOf("\"message\":\"") + 11;
                QString errorMessage = result.mid(pos, result.indexOf("\"", pos) - pos);

                if( errorMessage == "insufficient funds")
                {
                    QMessageBox::critical(NULL, "", QString::fromLocal8Bit("当前账户余额不足!"), QMessageBox::Ok);

                }
                else
                {
                    QMessageBox::critical(NULL, "", QString::fromLocal8Bit("调用合约失败!"), QMessageBox::Ok);
                }
            }
        }

    }
}


void CallDialog::on_okBtn_clicked()
{
    HXChain::getInstance()->postRPC(  "id_call_contract" , toJsonFormat("call_contract",
                                                  QJsonArray() << ui->contractLineEdit->text() << HXChain::getInstance()->currentAccount << ui->functionComboBox->currentText() << ui->parameterLineEdit->text() << "ALP" << ui->maxCostLineEdit->text()));

}

void CallDialog::on_parameterLineEdit_textChanged(const QString &arg1)
{
    ui->okBtn->setEnabled(false);
    ui->costLabel->clear();
    costCalculated = false;
}

void CallDialog::on_maxCostLineEdit_textChanged(const QString &arg1)
{
    validateOkBtn();
}

void CallDialog::on_functionComboBox_currentIndexChanged(const QString &arg1)
{
    ui->okBtn->setEnabled(false);
    ui->costLabel->clear();
    costCalculated = false;
}

void CallDialog::validateOkBtn()
{
    if( !costCalculated)  return;
    if( ui->costLabel->text().isEmpty())  return;
    QString cost = ui->costLabel->text();
    cost.remove(" ALP");
    if( cost.toDouble() > ui->maxCostLineEdit->text().toDouble() )
    {
        ui->okBtn->setEnabled(false);
    }
    else
    {
        ui->okBtn->setEnabled(true);
    }
}

void CallDialog::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setPen(QPen(QColor(68,217,199),Qt::SolidLine));
    painter.setBrush(QBrush(QColor(68,217,199),Qt::SolidPattern));
    painter.drawRect(0,0,this->width(),40);
}

void CallDialog::on_contractLineEdit_textChanged(const QString &arg1)
{
    ui->functionComboBox->clear();
    ui->costLabel->clear();
    ui->okBtn->setEnabled(false);

    HXChain::getInstance()->postRPC(  "id_get_contract_info_" + ui->contractLineEdit->text(), toJsonFormat("get_contract_info",
                                                  QJsonArray() << ui->contractLineEdit->text()));

}

void CallDialog::on_closeBtn_clicked()
{
    close();
}

void CallDialog::mousePressEvent(QMouseEvent *event)
{

    if(event->button() == Qt::LeftButton)
     {
          mouse_press = true;
          //鼠标相对于窗体的位置（或者使用event->globalPos() - this->pos()）
          move_point = event->pos();
     }

}

void CallDialog::mouseMoveEvent(QMouseEvent *event)
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

void CallDialog::mouseReleaseEvent(QMouseEvent *)
{
    mouse_press = false;
}

void CallDialog::on_cancelBtn_clicked()
{
    close();
}
