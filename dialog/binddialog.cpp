#include "binddialog.h"
#include "ui_binddialog.h"

#include "../hxchain.h"

#include <QDebug>
#include <QMessageBox>
#include <QPainter>
#include <QMouseEvent>
#include <QListView>
#include <QDesktopWidget>

BindDialog::BindDialog(QString id, QString contractAddress, QString event, QWidget *parent) :
    QDialog(parent),
    scriptId(id),
    mouse_press(false),
    ui(new Ui::BindDialog)
{
    ui->setupUi(this);

//    setWindowFlags(  Qt::Dialog | Qt::WindowCloseButtonHint);
//    setWindowTitle(QString::fromLocal8Bit("绑定脚本"));
    setWindowFlags(Qt::FramelessWindowHint);
    move( (QApplication::desktop()->width() - this->width())/2 , (QApplication::desktop()->height() - this->height())/2);

    connect(HXChain::getInstance(),SIGNAL(jsonDataUpdated(QString)),this,SLOT(jsonDataUpdated(QString)));

    ui->eventComboBox->setView(new QListView());   // 设置下拉框每项高度有问题 光用qss不行
    ui->scriptComboBox->setView(new QListView());

    QStringList scripts = HXChain::getInstance()->scriptInfoMap.keys();
    ui->scriptComboBox->addItems(scripts);
    ui->scriptComboBox->setCurrentText(scriptId);

    ui->contractAddressLineEdit->setText(contractAddress);
    ui->eventComboBox->setCurrentText(event);
}

BindDialog::~BindDialog()
{
    delete ui;
}

void BindDialog::jsonDataUpdated(QString id)
{
    if( id == "id_get_contract_info_" + ui->contractAddressLineEdit->text().simplified() )
    {
        QString result = HXChain::getInstance()->jsonDataValue(id);

        if( result.startsWith("\"error\":"))
        {
//            QMessageBox::critical(NULL, "", QString::fromLocal8Bit("合约不存在!"), QMessageBox::Ok);
            return;
        }

        int pos = result.indexOf("\"id\":\"") + 6;

        pos = result.indexOf("\"description\":\"") + 15;
        QString description = result.mid(pos, result.indexOf("\"", pos) - pos);
        ui->descriptionLabel->setText(description);

//        pos = result.indexOf("\"owner\":\"") + 9;
//        QString owner = result.mid(pos, result.indexOf("\"", pos) - pos);
//        ui->ownerLabel->setText(owner);

        pos = result.indexOf("\"owner_address\":\"") + 17;
        QString ownerAddress = result.mid(pos, result.indexOf("\"", pos) - pos);

        pos = result.indexOf("\"owner_name\":\"") + 14;
        QString ownerName = result.mid(pos, result.indexOf("\"", pos) - pos);
        if( !ownerName.isEmpty())
        {   // 如果有ownername 则显示ownername  否则显示ownerAddress
            ui->ownerLabel->setText(ownerName);
        }
        else
        {
            ui->ownerLabel->setText(ownerAddress);
        }

        pos = result.indexOf("\"events\":[") + 10;
        QString events = result.mid(pos, result.indexOf("]", pos) - pos);
        if(!events.isEmpty())
        {
            events.remove("\"");
            QStringList eventList = events.split(",");
            ui->eventComboBox->addItems(eventList);
        }

        return;
    }

    if( id.startsWith("id_add_event_handler_") )
    {
        QString result = HXChain::getInstance()->jsonDataValue(id);

        if( result.startsWith("\"result\":"))
        {
            QMessageBox::information(NULL, "", QString::fromLocal8Bit("绑定成功!"), QMessageBox::Ok);
            close();
        }
        else
        {
            if( result.mid(0,8) == "\"error\":")
            {
                int pos = result.indexOf("\"message\":\"") + 11;
                QString errorMessage = result.mid(pos, result.indexOf("\"", pos) - pos);

                QMessageBox::critical(NULL, "", "Bind script fail : " + errorMessage, QMessageBox::Ok);
            }
        }

        return;
    }
}

void BindDialog::on_okBtn_clicked()
{
    QString contract =  ui->contractAddressLineEdit->text().simplified();
    if( contract.isEmpty())   return;
    if( ui->scriptComboBox->currentText().isEmpty())    return;

    if( contract.startsWith("CON") && contract.count() > 32)
    {
        HXChain::getInstance()->postRPC(  "id_add_event_handler_" + ui->contractAddressLineEdit->text(), toJsonFormat("add_event_handler", QJsonArray() << ui->contractAddressLineEdit->text() << ui->eventComboBox->currentText() << ui->scriptComboBox->currentText() << ui->indexSpinBox->text()  ));
    }
    else
    {
        // 如果是合约名   add_event_handler 不支持使用合约名  转换为合约地址
        QString address;
        QStringList keys = HXChain::getInstance()->contractInfoMap.keys();
        foreach ( QString key, keys)
        {
            if( HXChain::getInstance()->contractInfoMap.value(key).name == contract)
            {
                address =  HXChain::getInstance()->contractInfoMap.value(key).address;
                break;
            }
        }

        if( !address.isEmpty())
        {
            HXChain::getInstance()->postRPC(  "id_add_event_handler_" + ui->contractAddressLineEdit->text(), toJsonFormat("add_event_handler", QJsonArray() << address << ui->eventComboBox->currentText() << ui->scriptComboBox->currentText() << ui->indexSpinBox->text()  ));
        }



    }





}

void BindDialog::on_contractAddressLineEdit_textChanged(const QString &arg1)
{ 
    ui->eventComboBox->clear();
    ui->ownerLabel->clear();
    ui->descriptionLabel->clear();


    if( ui->contractAddressLineEdit->text().simplified().isEmpty())  return;

    HXChain::getInstance()->postRPC(  "id_get_contract_info_" + ui->contractAddressLineEdit->text(), toJsonFormat("get_contract_info", QJsonArray() << ui->contractAddressLineEdit->text()  ));

}

void BindDialog::on_closeBtn_clicked()
{
    close();
}

void BindDialog::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setPen(QPen(QColor(68,217,199),Qt::SolidLine));
    painter.setBrush(QBrush(QColor(68,217,199),Qt::SolidPattern));
    painter.drawRect(0,0,this->width(),40);
}


void BindDialog::mousePressEvent(QMouseEvent *event)
{

    if(event->button() == Qt::LeftButton)
     {
          mouse_press = true;
          //鼠标相对于窗体的位置（或者使用event->globalPos() - this->pos()）
          move_point = event->pos();
     }

}

void BindDialog::mouseMoveEvent(QMouseEvent *event)
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

void BindDialog::mouseReleaseEvent(QMouseEvent *)
{
    mouse_press = false;
}

void BindDialog::on_bindBtn_2_clicked()
{
    close();
}
