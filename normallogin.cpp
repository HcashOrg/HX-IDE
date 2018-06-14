#include <QPainter>
#include <QKeyEvent>
#include <QDebug>
#include <windows.h>
#include <QDateTime>

#include "normallogin.h"
#include "ui_normallogin.h"
#include "hxchain.h"

NormalLogin::NormalLogin(int chainType, QWidget *parent) :
    QWidget(parent),
    type(chainType),
    ui(new Ui::NormalLogin)
{
    ui->setupUi(this);

    setWindowFlags(  this->windowFlags() & ~Qt::WindowMaximizeButtonHint);
    setFixedSize(960,580);

    connect( HXChain::getInstance(),SIGNAL(jsonDataUpdated(QString)),this,SLOT(pwdConfirmed(QString)));

    chances = 5;

    setAutoFillBackground(true);
    QPalette palette;
    palette.setBrush(QPalette::Background, QBrush(QPixmap(":/pic2/bg.png")));
    setPalette(palette);
    ui->logoLabel->setPixmap(QPixmap(":/pic2/logo173x56.png"));
    ui->pwdLineEdit->setFocus();
    ui->pwdLineEdit->setContextMenuPolicy(Qt::NoContextMenu);
    ui->pwdLineEdit->setAttribute(Qt::WA_InputMethodEnabled, false);


//    ui->enterBtn->setIcon(QPixmap(":/pic2/arrow.png"));
    ui->enterBtn->setStyleSheet("QToolButton{background-image:url(:/pic2/arrow.png);background-repeat: repeat-xy;background-position: center;background-attachment: fixed;background-clip: padding;border-style: flat;}"
                                "QToolButton:hover{background-image:url(:/pic2/arrow_hover.png);background-repeat: repeat-xy;background-position: center;background-attachment: fixed;background-clip: padding;border-style: flat;}");

    QFont font = ui->loginLabel->font();
    font.setPixelSize(12);
    ui->loginLabel->setFont(font);

    font = ui->tipLabel->font();
    font.setPixelSize(12);
    ui->tipLabel->setFont(font);


    timerForLockTime = new QTimer(this);
    connect(timerForLockTime, SIGNAL(timeout()), this, SLOT(checkLock()));
    timerForLockTime->start(1000);

    checkLock();

}

NormalLogin::~NormalLogin()
{

    delete ui;

}

void NormalLogin::on_enterBtn_clicked()
{

    if( ui->pwdLineEdit->text().isEmpty() )
    {
        ui->tipLabel->setText( tr("Empty!") );
        return;
    }

    if( ui->pwdLineEdit->text().size() < 8)
    {
//        ui->tipLabel->setText( QString::fromLocal8Bit("密码至少为8位！") );
        ui->tipLabel->setText( tr("At least 8 letters!") );
        ui->pwdLineEdit->clear();
        return;
    }

    HXChain::getInstance()->postRPC(  "id_unlock_normallogin", toJsonFormat("unlock", QJsonArray() << "99999999" << ui->pwdLineEdit->text() ), type);

    emit showShadowWidget();
    ui->pwdLineEdit->setEnabled(false);

}


void NormalLogin::on_pwdLineEdit_returnPressed()
{
    on_enterBtn_clicked();
}



void NormalLogin::on_pwdLineEdit_textChanged(const QString &arg1)
{
    if( !arg1.isEmpty())
    {
        ui->tipLabel->clear();
    }

    if( arg1.indexOf(' ') > -1)
    {
        ui->pwdLineEdit->setText( ui->pwdLineEdit->text().remove(' '));
    }
}

void NormalLogin::pwdConfirmed(QString id)
{
    if( id == "id_unlock_normallogin" )
    {

        emit hideShadowWidget();
        ui->pwdLineEdit->setEnabled(true);
        ui->pwdLineEdit->setFocus();
        QString result = HXChain::getInstance()->jsonDataValue( id);

        if( result == "\"result\":null")
        {
            emit login();
            this->close();
        }
        else
        {
            ui->tipLabel->setText(tr("Wrong password!"));
            ui->pwdLineEdit->clear();

            chances--;
            qDebug() << "chances : " << chances;
            if( chances < 1)
            {
                if( lockTime() == 0)  // 如果没有 locktime文件 则创建
                {
                    qDebug() << "apppath: " << HXChain::getInstance()->appDataPath;
                    QFile file(HXChain::getInstance()->appDataPath + "\\locktime");
                    qDebug()  << "file open : " <<   file.open(QIODevice::WriteOnly);
                    file.close();
                }

                checkLock();

            }

        }

        return;
    }
}

int NormalLogin::lockTime()
{
    QFileInfo fileInfo;
    fileInfo.setFile(HXChain::getInstance()->appDataPath + "\\locktime");

    if( fileInfo.exists())
    {
        QDateTime createdTime = fileInfo.lastModified();
        int deltaTime = QDateTime::currentDateTime().toTime_t() - createdTime.toTime_t();

        if( deltaTime < PWD_LOCK_TIME)
        {
//            ui->tipLabel->setText( tr("Please try after ") + QString::number( PWD_LOCK_TIME - deltaTime) + "s");
            return PWD_LOCK_TIME - deltaTime;
        }
        else
        {
            qDebug() << "remove locktime file :" << QFile::remove(HXChain::getInstance()->appDataPath + "\\locktime");
            return 0;
        }

    }
    else
    {
        return 0;
    }
}

void NormalLogin::checkLock()
{
    int timeLeft = lockTime();
    if( timeLeft > 0)
    {
        ui->pwdLineEdit->setEnabled(false);
        ui->enterBtn->setEnabled(false);

        if( timeLeft > 3600)
        {
            int hours   = timeLeft / 3600;
            int minutes = (timeLeft - 3600 * hours) / 60;
            ui->tipLabel->setText( tr("Please try after ") + QString::number( hours) + tr(" hours ") +
                                   QString::number( minutes) + tr(" minutes ") + tr(".  "));
        }
        else if( timeLeft > 60)
        {
            int minutes = timeLeft / 60;
            ui->tipLabel->setText( tr("Please try after ") + QString::number( minutes) + tr(" minutes ") + tr(".  "));
        }
        else
        {
            ui->tipLabel->setText( tr("Please try after ") + QString::number( timeLeft) + tr(" seconds ") + tr(".  "));
        }

        ui->pwdLineEdit->clear();
        return;
    }
    else
    {
        ui->pwdLineEdit->setEnabled(true);
        ui->enterBtn->setEnabled(true);
        if( ui->tipLabel->text().startsWith( tr("Please try after ")))
        {
            ui->tipLabel->clear();
        }
    }
}
