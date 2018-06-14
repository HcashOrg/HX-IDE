#include <QDebug>
#include <QPainter>
#include <QKeyEvent>
#include <windows.h>

#include "firstlogin.h"
#include "ui_firstlogin.h"
#include "hxchain.h"
#include "rpcthread.h"

FirstLogin::FirstLogin( int chainType, QWidget *parent) :
    QWidget(parent),
    type(chainType),
    ui(new Ui::FirstLogin)
{
    ui->setupUi(this);

    setWindowFlags(  this->windowFlags() & ~Qt::WindowMaximizeButtonHint);
    setFixedSize(960,580);

    connect(HXChain::getInstance(),SIGNAL(jsonDataUpdated(QString)),this,SLOT(jsonDataUpdated(QString)));

    setAutoFillBackground(true);
    QPalette palette;
    palette.setBrush(QPalette::Background, QBrush(QPixmap(":/pic2/bg2.png")));
    setPalette(palette);

    ui->pwdLineEdit->setFocus();
    ui->pwdLineEdit->installEventFilter(this);
    ui->pwdLineEdit->setContextMenuPolicy(Qt::NoContextMenu);
    ui->pwdLineEdit2->setContextMenuPolicy(Qt::NoContextMenu);
    ui->pwdLineEdit->setAttribute(Qt::WA_InputMethodEnabled, false);
    ui->pwdLineEdit2->setAttribute(Qt::WA_InputMethodEnabled, false);


    QFont font = ui->pwdLabel->font();
    font.setPixelSize(12);
    ui->pwdLabel->setFont(font);
    ui->pwdLabel2->setFont(font);

    font = ui->tipLabel->font();
    font.setPixelSize(12);
    ui->tipLabel->setFont(font);

    ui->correctLabel->setPixmap( QPixmap(":/pic2/correct2.png"));
    ui->correctLabel2->setPixmap( QPixmap(":/pic2/correct2.png"));
    ui->correctLabel->hide();
    ui->correctLabel2->hide();

    ui->tipLabel2->setPixmap(QPixmap(":/pic2/pwdTip.png"));

}

FirstLogin::~FirstLogin()
{

    delete ui;

}

void FirstLogin::on_createBtn_clicked()
{

    if( ui->pwdLineEdit->text().isEmpty() || ui->pwdLineEdit2->text().isEmpty() )
    {
        ui->tipLabel->setText(tr("Empty!"));
        return;
    }

    if( ui->pwdLineEdit->text() == ui->pwdLineEdit2->text())
    {
        if( ui->pwdLineEdit->text().size() < 8)
        {
            ui->tipLabel->setText(tr("Too short!"));
            return;
        }

        emit showShadowWidget();
        ui->pwdLineEdit->setEnabled(false);
        ui->pwdLineEdit2->setEnabled(false);

//        RpcThread* rpcThread = new RpcThread;
//        connect(rpcThread,SIGNAL(finished()),rpcThread,SLOT(deleteLater()));
//        rpcThread->setLogin("a","b");
//        rpcThread->setWriteData( toJsonFormat( "id_create", "create", QStringList() << "wallet" << ui->pwdLineEdit->text() ));
//        rpcThread->start();

        HXChain::getInstance()->postRPC(  "id_create", toJsonFormat("create", QJsonArray() << "wallet" << ui->pwdLineEdit->text() ), type);
    }
    else
    {
        ui->tipLabel->setText(tr("not consistent!"));
        return;
    }

}


void FirstLogin::on_pwdLineEdit2_returnPressed()
{
    on_createBtn_clicked();
}

bool FirstLogin::eventFilter(QObject *watched, QEvent *e)
{
    if(watched == ui->pwdLineEdit)
    {
        if(e->type() == QEvent::FocusIn)
        {
            ui->tipLabel2->show();
        }
        else if( e->type() == QEvent::FocusOut)
        {
            ui->tipLabel2->hide();
        }
    }

    return QWidget::eventFilter(watched,e);

}

void FirstLogin::on_pwdLineEdit_textChanged(const QString &arg1)
{
    if( arg1.indexOf(' ') > -1)
    {
        ui->pwdLineEdit->setText( ui->pwdLineEdit->text().remove(' '));
        return;
    }

    ui->tipLabel->clear();

    if( arg1.size() > 7 )
    {
        ui->correctLabel->show();

        if( ui->pwdLineEdit->text() == ui->pwdLineEdit2->text() )
        {
            ui->correctLabel2->show();
        }
        else
        {
            ui->correctLabel2->hide();
        }
    }
    else
    {
        ui->correctLabel->hide();
        ui->correctLabel2->hide();
    }

}

void FirstLogin::on_pwdLineEdit2_textChanged(const QString &arg1)
{
    if( arg1.indexOf(' ') > -1)
    {
        ui->pwdLineEdit2->setText( ui->pwdLineEdit2->text().remove(' '));
        return;
    }

    ui->tipLabel->clear();

    if( arg1.size() > 7 && ( ui->pwdLineEdit->text() == ui->pwdLineEdit2->text() ) )
    {
        ui->correctLabel2->show();
    }
    else
    {
        ui->correctLabel2->hide();
    }
}

void FirstLogin::jsonDataUpdated(QString id)
{
    if( id == "id_create")
    {
        QString result = HXChain::getInstance()->jsonDataValue(id);
        qDebug() << "id_create" << result;
        if( result == "\"result\":null" )
        {
            HXChain::getInstance()->postRPC(  "id_unlock_firstlogin", toJsonFormat("unlock", QJsonArray() << "99999999" << ui->pwdLineEdit->text() ), type);
qDebug() << "id_unlock_firstlogin";
            return;
        }
        else
        {
            emit hideShadowWidget();
            return;
        }

    }

    if( id == "id_unlock_firstlogin")
    {
        emit hideShadowWidget();
        ui->pwdLineEdit->setEnabled(true);
        ui->pwdLineEdit2->setEnabled(true);

        qDebug() << "id_unlock_firstlogin" << HXChain::getInstance()->jsonDataValue(id);
        if( HXChain::getInstance()->jsonDataValue(id) == "\"result\":null")
        {
            emit login();
            this->close();
        }
        return;
    }

}
