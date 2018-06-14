#include "registercontractdialog.h"
#include "ui_registercontractdialog.h"

#include "../hxchain.h"

#include <QDebug>
#include <QFileInfo>
#include <QMessageBox>
#include <QDir>
#include <QPainter>
#include <QMouseEvent>
#include <QDesktopWidget>

RegisterContractDialog::RegisterContractDialog(QString path, QWidget *parent) :
    QDialog(parent),
    filePath(path),
    mouse_press(false),
    ui(new Ui::RegisterContractDialog)
{
    ui->setupUi(this);

//    setWindowFlags(  Qt::Dialog | Qt::WindowCloseButtonHint);
//    setWindowTitle(QString::fromLocal8Bit("合约注册"));
    setWindowFlags(Qt::FramelessWindowHint);
    move( (QApplication::desktop()->width() - this->width())/2 , (QApplication::desktop()->height() - this->height())/2);

    connect(HXChain::getInstance(),SIGNAL(jsonDataUpdated(QString)),this,SLOT(jsonDataUpdated(QString)));

    QFileInfo info(filePath);
    ui->pathLabel->setText( info.fileName());

    HXChain::getInstance()->postRPC(  "id_register_contract_testing_" + filePath, toJsonFormat("register_contract_testing",
                                                  QJsonArray() << HXChain::getInstance()->currentAccount << filePath));

    ui->initCostLineEdit->setAttribute(Qt::WA_InputMethodEnabled, false);

    QRegExp rx("^([0]|[1-9][0-9]{0,5})(?:\\.\\d{1,5})?$|(^\\t?$)");
    QRegExpValidator *pReg = new QRegExpValidator(rx, this);
    ui->initCostLineEdit->setValidator(pReg);

    ui->okBtn->setEnabled(false);

}

RegisterContractDialog::~RegisterContractDialog()
{
    delete ui;
}

void RegisterContractDialog::jsonDataUpdated(QString id)
{
    if( id == "id_register_contract_testing_" + filePath)
    {
        QString result = HXChain::getInstance()->jsonDataValue(id);

        if( result.startsWith("\"result\":"))
        {
            int pos = result.indexOf("\"amount\":") + 9;
            QString fee = result.mid(pos, result.indexOf(",", pos) - pos);
            fee.remove('\"');

            pos = result.indexOf("\"amount\":", pos) + 9;
            QString deposit = result.mid(pos, result.indexOf(",", pos) - pos);
            deposit.remove('\"');

            pos = result.indexOf("\"amount\":", pos) + 9;
            QString costForLength = result.mid(pos, result.indexOf(",", pos) - pos);
            costForLength.remove('\"');

            pos = result.indexOf("\"amount\":", pos) + 9;
            QString costForInit = result.mid(pos, result.indexOf(",", pos) - pos);
            costForInit.remove('\"');

            double baseCost = (fee.toDouble() + deposit.toDouble() + costForLength.toDouble()) / 100000.0;
            ui->baseCostLabel->setText( QString::number( baseCost, 'f', 5) + " ALP");

            double initCost = costForInit.toDouble() / 100000.0;
            ui->expectedCostLabel->setText( QString::number( initCost, 'f', 5) + " ALP");

        }
        else
        {
            QMessageBox::critical(NULL, "Error", "Test registering contract failed : " + result, QMessageBox::Ok);
            close();
        }

        return;
    }

    if( id == "id_register_contract_" + filePath)
    {
        QString result = HXChain::getInstance()->jsonDataValue(id);

        if( result.startsWith("\"result\":"))
        {
            QString address = result.mid(9);
            address.remove('\"');

            // 已注册的合约记录在config中  复制源码文件到 registered文件夹下的 test或formal文件夹下 文件名后添加_reg
            QString luaPath = filePath;
            luaPath = luaPath.left( luaPath.count() - 4);
            luaPath.append(".glua");

            // 如果有.glua文件 拷贝的是.glua文件 否则是.gpc文件
            QFileInfo luaInfo(luaPath);
            if( luaInfo.exists())
            {
                filePath = luaPath;
            }

            QString registeredPath;
            QDir dir("registered");
            QFileInfo info(filePath);

//            if( !info.exists())
//            {
//                // 如果合约源码文件不存在
//                QMessageBox::information(NULL, "Success", QString::fromLocal8Bit("合约注册成功！未找到合约源码文件."), QMessageBox::Ok);
//                close();
//                return;
//            }

            QString name = info.fileName();
            QString suffix;
            if( name.endsWith(".glua"))
            {
                name = name.left( name.count() - 5);
                suffix = ".glua";
            }
            else if( name.endsWith(".gpc"))
            {
                name = name.left( name.count() - 4);
                suffix = ".gpc";
            }

            QString remark;
            if( HXChain::getInstance()->isInSandBox)
            {
                remark = "_sandbox";
            }
            else
            {
                remark = "";
            }

            if( HXChain::getInstance()->currenChain() == 1)
            {
                int num = 1;
                registeredPath = dir.absolutePath() + "/test/" + name + "_reg" + QString::number(num) + remark + suffix;
                QFileInfo info(registeredPath);
                while ( info.exists())
                {
                    num++;
                    registeredPath = dir.absolutePath() + "/test/" + name + "_reg" + QString::number(num) + remark + suffix;
                    info.setFile(registeredPath);
                }
            }
            else if( HXChain::getInstance()->currenChain() == 2)
            {
                int num = 1;
                registeredPath = dir.absolutePath() + "/formal/" + name + "_reg" + QString::number(num) + remark + suffix;
                QFileInfo info(registeredPath);
                while ( info.exists())
                {
                    num++;
                    registeredPath = dir.absolutePath() + "/formal/" + name + "_reg" + QString::number(num) + remark + suffix;
                    info.setFile(registeredPath);
                }
            }

            if( QFile::copy(filePath, registeredPath) )
            {
                qDebug() << "register copy succeed: " << registeredPath;
            }
            else
            {
                qDebug() << "register copy fail: " << registeredPath;
            }


            HXChain::getInstance()->configSetContractAddress(registeredPath,address);

            ContractInfo cInfo;
            cInfo.address = address;
            cInfo.ownerAddress = HXChain::getInstance()->accountInfoMap.value(HXChain::getInstance()->currentAccount).address;
            HXChain::getInstance()->contractInfoMap.insert(address, cInfo);

            emit contractRegistered(registeredPath);

            QMessageBox::information(NULL, "Success", "Contract address : " + address, QMessageBox::Ok);
            close();
        }
        else
        {
            QMessageBox::critical(NULL, "Error", "Register contract failed : " + result, QMessageBox::Ok);
        }

        return;
    }

}

void RegisterContractDialog::on_okBtn_clicked()
{
    HXChain::getInstance()->postRPC(  "id_register_contract_" + filePath, toJsonFormat("register_contract",
                                                  QJsonArray() << HXChain::getInstance()->currentAccount << filePath << "ALP" << ui->initCostLineEdit->text()));
}

void RegisterContractDialog::on_cancelBtn_clicked()
{
    close();
}

void RegisterContractDialog::on_initCostLineEdit_textChanged(const QString &arg1)
{
    QString cost = ui->expectedCostLabel->text();
    cost.remove(" ALP");
    if( cost.toDouble() > ui->initCostLineEdit->text().toDouble())
    {
        ui->okBtn->setEnabled(false);
    }
    else
    {
        ui->okBtn->setEnabled(true);
    }
}

void RegisterContractDialog::on_closeBtn_clicked()
{
    close();
}

void RegisterContractDialog::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setPen(QPen(QColor(68,217,199),Qt::SolidLine));
    painter.setBrush(QBrush(QColor(68,217,199),Qt::SolidPattern));
    painter.drawRect(0,0,this->width(),40);
}

void RegisterContractDialog::mousePressEvent(QMouseEvent *event)
{

    if(event->button() == Qt::LeftButton)
     {
          mouse_press = true;
          //鼠标相对于窗体的位置（或者使用event->globalPos() - this->pos()）
          move_point = event->pos();
     }

}

void RegisterContractDialog::mouseMoveEvent(QMouseEvent *event)
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

void RegisterContractDialog::mouseReleaseEvent(QMouseEvent *)
{
    mouse_press = false;
}
