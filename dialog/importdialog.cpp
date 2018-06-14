#include "importdialog.h"
#include "ui_importdialog.h"
#include "../hxchain.h"
#include "namedialog.h"

#include <QDir>
#include <QFileDialog>
#include <QDebug>
#include <QFile>
#include <QDesktopWidget>
#include <QMouseEvent>
#include <QPainter>
#include <QMessageBox>

ImportDialog::ImportDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ImportDialog)
{
    ui->setupUi(this);

    setWindowFlags(Qt::FramelessWindowHint);
    move( (QApplication::desktop()->width() - this->width())/2 , (QApplication::desktop()->height() - this->height())/2);

    connect( HXChain::getInstance(), SIGNAL(jsonDataUpdated(QString)), this, SLOT(jsonDataUpdated(QString)));

    ui->okBtn->setEnabled(false);

}

ImportDialog::~ImportDialog()
{
    delete ui;
}

void ImportDialog::mousePressEvent(QMouseEvent *event)
{

    if(event->button() == Qt::LeftButton)
     {
          mouse_press = true;
          //鼠标相对于窗体的位置（或者使用event->globalPos() - this->pos()）
          move_point = event->pos();
     }

}

void ImportDialog::mouseMoveEvent(QMouseEvent *event)
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

void ImportDialog::mouseReleaseEvent(QMouseEvent *)
{
    mouse_press = false;
}



void ImportDialog::on_pathBtn_clicked()
{
    QString file = QFileDialog::getOpenFileName(this, tr("Choose your private key file."),"","(*.gkey)");
    file.replace("/","\\");
    ui->privateKeyLineEdit->setText(file);
}

void ImportDialog::on_okBtn_clicked()
{
    // 如果输入框中是 私钥文件的地址
    if( ui->privateKeyLineEdit->text().contains(".gkey") )
    {
        QFile file( ui->privateKeyLineEdit->text());
        if( file.open(QIODevice::ReadOnly))
        {
            QByteArray ba = QByteArray::fromBase64( file.readAll());
            QString str(ba);

            HXChain::getInstance()->postRPC(  "id_wallet_import_private_key",toJsonFormat( "wallet_import_private_key", QJsonArray() << str));

            ui->okBtn->setEnabled(false);

            file.close();
            return;
        }
        else
        {
            QMessageBox::critical(NULL, "Error", "Open file " + ui->privateKeyLineEdit->text() + " error : "  + file.errorString(), QMessageBox::Ok);

            return;
        }

    }
    else // 如果输入框中是 私钥
    {
        HXChain::getInstance()->postRPC(  "id_wallet_import_private_key", toJsonFormat("wallet_import_private_key", QJsonArray() << ui->privateKeyLineEdit->text() ));

        ui->okBtn->setEnabled(false);

    }

}

QString toThousandFigure( int);

void ImportDialog::jsonDataUpdated(QString id)
{
    if( id == "id_wallet_import_private_key")
    {

        QString result = HXChain::getInstance()->jsonDataValue(id);
        if( result.mid(0,9) == "\"result\":")
        {
            ui->okBtn->setEnabled(true);

//            HXChain::getInstance()->needToScan = true;  // 导入成功，需要scan

            QString name = result.mid(10, result.indexOf("\"", 10) - 10);
            if( HXChain::getInstance()->accountInfoMap.keys().contains(name))        //  如果已存在该账号
            {
                QMessageBox::warning(NULL, "", name + QString::fromLocal8Bit("已存在!"), QMessageBox::Ok);

            }
            else
            {

                emit accountImported();

                QMessageBox::information(NULL, "", name + QString::fromLocal8Bit("已导入!"), QMessageBox::Ok);

                HXChain::getInstance()->postRPC(  "id_scan", toJsonFormat("scan", QJsonArray() << "0" ));
                HXChain::getInstance()->postRPC(  "id_wallet_scan_transaction", toJsonFormat("wallet_scan_transaction", QJsonArray() << "ALL" << "true" ));

                close();
            }

        }
        else if( result.mid(0,8) == "\"error\":")
        {
            qDebug() << "import error: " << result;

            if( !result.contains("Unknown key! You must specify an account name!") )
            {
                // 如果不是未注册账户 是错误的私钥格式
                QMessageBox::critical(NULL, "Error", QString::fromLocal8Bit("错误的私钥格式!"), QMessageBox::Ok);

                ui->okBtn->setEnabled(true);
                return;
            }

            NameDialog nameDialog;
            QString name = nameDialog.pop();

            if( name.isEmpty())    // 如果取消 命名
            {
                ui->okBtn->setEnabled(true);
                return;
            }

            if( ui->privateKeyLineEdit->text().contains(".gkey") )  // 如果是路径
            {
                QFile file( ui->privateKeyLineEdit->text());
                if( file.open(QIODevice::ReadOnly))
                {
                    QByteArray ba = QByteArray::fromBase64( file.readAll());
                    QString str(ba);

                    HXChain::getInstance()->postRPC(  "id_wallet_import_private_key_" + name,toJsonFormat( "wallet_import_private_key", QJsonArray() << str << name << "true"));

                    file.close();
                    return;
                }
                else
                {
                    QMessageBox::critical(NULL, "Error", QString::fromLocal8Bit("错误的文件路径!"), QMessageBox::Ok);


                    ui->okBtn->setEnabled(true);

                    return;
                }

            }
            else // 如果输入框中是 私钥
            {
                HXChain::getInstance()->postRPC(  "id_wallet_import_private_key_" + name,toJsonFormat( "wallet_import_private_key", QJsonArray() << ui->privateKeyLineEdit->text() << name << "true" ));

            }
        }

        return;
    }

    if( id.mid(0,29) == "id_wallet_import_private_key_")
    {
        ui->okBtn->setEnabled(true);

        QString result = HXChain::getInstance()->jsonDataValue(id);

        if( result.mid(0,9) == "\"result\":")
        {
//            HXChain::getInstance()->needToScan = true;  // 导入成功，需要scan

            QString name = result.mid(10, result.indexOf("\"", 10) - 10);


            emit accountImported();

            QMessageBox::information(NULL, "", name + QString::fromLocal8Bit("已导入!"), QMessageBox::Ok);

            HXChain::getInstance()->postRPC(  "id_scan", toJsonFormat("scan", QJsonArray() << "0" ));
            HXChain::getInstance()->postRPC(  "id_wallet_scan_transaction", toJsonFormat("wallet_scan_transaction", QJsonArray() << "ALL" << "true" ));


            close();
        }
        else if( result.mid(0,8) == "\"error\":")
        {
            QMessageBox::critical(NULL, "Error", QString::fromLocal8Bit("错误的私钥格式!"), QMessageBox::Ok);
        }

        return;
    }

}


void ImportDialog::on_privateKeyLineEdit_textChanged(const QString &arg1)
{
    QString str = arg1.simplified();
    if( str.isEmpty())
    {
        ui->okBtn->setEnabled(false);
    }
    else
    {
        ui->okBtn->setEnabled(true);
    }
}

void ImportDialog::on_privateKeyLineEdit_returnPressed()
{
    if( ui->okBtn->isEnabled())
    {
        on_okBtn_clicked();
    }
}

void ImportDialog::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setPen(QPen(QColor(68,217,199),Qt::SolidLine));
    painter.setBrush(QBrush(QColor(68,217,199),Qt::SolidPattern));
    painter.drawRect(0,0,this->width(),40);
}

void ImportDialog::on_closeBtn_clicked()
{
    close();
}

void ImportDialog::on_cancelBtn_clicked()
{
    close();
}
