#include "removescriptdialog.h"
#include "ui_removescriptdialog.h"

#include "../hxchain.h"

#include <QMessageBox>
#include <QDebug>
#include <QPainter>
#include <QMouseEvent>
#include <QDesktopWidget>

RemoveScriptDialog::RemoveScriptDialog(QString scriptId, QWidget *parent) :
    QDialog(parent),
    mouse_press(false),
    ui(new Ui::RemoveScriptDialog)
{
    ui->setupUi(this);

//    setWindowFlags(  Qt::Dialog | Qt::WindowCloseButtonHint);
//    setWindowTitle( QString::fromLocal8Bit("注销脚本"));
    setWindowFlags(Qt::FramelessWindowHint);
    move( (QApplication::desktop()->width() - this->width())/2 , (QApplication::desktop()->height() - this->height())/2);

    connect(HXChain::getInstance(),SIGNAL(jsonDataUpdated(QString)),this,SLOT(jsonDataUpdated(QString)));

    ui->scriptLabel->setText(scriptId);
}

RemoveScriptDialog::~RemoveScriptDialog()
{
    delete ui;
}

void RemoveScriptDialog::on_okBtn_clicked()
{
    QString pwd = ui->pwdLineEdit->text();
    pwd = pwd.simplified();
    if( pwd.isEmpty())
    {
        QMessageBox::warning(NULL, "", QString::fromLocal8Bit("请输入钱包密码!"), QMessageBox::Ok);
        return;
    }

    if( QMessageBox::Yes != QMessageBox::information(NULL, "", QString::fromLocal8Bit("确定注销该脚本?"), QMessageBox::Yes | QMessageBox::No))
    {
        return;
    }

    HXChain::getInstance()->postRPC(  "id_wallet_check_passphrase_withdrawDialog",toJsonFormat( "wallet_check_passphrase",
                                                  QJsonArray() << pwd));

}


void RemoveScriptDialog::jsonDataUpdated(QString id)
{
    if( id == "id_wallet_check_passphrase_withdrawDialog")
    {
        QString result = HXChain::getInstance()->jsonDataValue(id);

        if( result == "\"result\":true")
        {
            HXChain::getInstance()->postRPC(  "id_remove_script_" + ui->scriptLabel->text(), toJsonFormat("remove_script",
                                                          QJsonArray() << ui->scriptLabel->text()));
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

                QMessageBox::critical(NULL, "", "Remove script fail : " + errorMessage, QMessageBox::Ok);
            }
        }

        return;
    }

    if( id == "id_remove_script_" + ui->scriptLabel->text())
    {
        QString result = HXChain::getInstance()->jsonDataValue(id);

        QString scriptId = id.mid(17);
        if( result == "\"result\":null")
        {
            QString key = HXChain::getInstance()->configRemoveScriptId(scriptId);

            if( key.isEmpty())
            {
                qDebug() << "no scriptId in config file: " << scriptId;
                return;
            }

            QString path = HXChain::getInstance()->restorePathFormat(key);
           qDebug() << "remove file " + path + " : " << QFile::remove(path);
            emit scriptRemoved(ui->scriptLabel->text());
            HXChain::getInstance()->scriptInfoMap.remove(ui->scriptLabel->text());

            QMessageBox::information(NULL, "", QString::fromLocal8Bit("注销脚本成功!"), QMessageBox::Ok);
            close();
        }
        else
        {
            int pos = result.indexOf("\"message\":\"") + 11;
            QString errorMessage = result.mid(pos, result.indexOf("\"", pos) - pos);

            QMessageBox::critical(NULL, "", QString::fromLocal8Bit("注销脚本失败: ") + errorMessage, QMessageBox::Ok);
        }

        return;
    }
}

void RemoveScriptDialog::on_pwdLineEdit_returnPressed()
{
    on_okBtn_clicked();
}

void RemoveScriptDialog::on_closeBtn_clicked()
{
    close();
}

void RemoveScriptDialog::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setPen(QPen(QColor(68,217,199),Qt::SolidLine));
    painter.setBrush(QBrush(QColor(68,217,199),Qt::SolidPattern));
    painter.drawRect(0,0,this->width(),40);
}

void RemoveScriptDialog::mousePressEvent(QMouseEvent *event)
{

    if(event->button() == Qt::LeftButton)
     {
          mouse_press = true;
          //鼠标相对于窗体的位置（或者使用event->globalPos() - this->pos()）
          move_point = event->pos();
     }

}

void RemoveScriptDialog::mouseMoveEvent(QMouseEvent *event)
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

void RemoveScriptDialog::mouseReleaseEvent(QMouseEvent *)
{
    mouse_press = false;
}

void RemoveScriptDialog::on_cancelBtn_clicked()
{
    close();
}
