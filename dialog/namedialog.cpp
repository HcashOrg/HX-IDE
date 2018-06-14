#include "namedialog.h"
#include "ui_namedialog.h"
#include "../hxchain.h"
#include "../rpcthread.h"

#include <QMovie>
#include <QPainter>
#include <QMouseEvent>
#include <QDesktopWidget>

NameDialog::NameDialog(QWidget *parent) :
    QDialog(parent),
    mouse_press(false),
    ui(new Ui::NameDialog)
{
    ui->setupUi(this);

//    setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint);
//    setWindowTitle(QString::fromLocal8Bit("账户命名"));
    setWindowFlags(Qt::FramelessWindowHint);
    move( (QApplication::desktop()->width() - this->width())/2 , (QApplication::desktop()->height() - this->height())/2);

    connect( HXChain::getInstance(), SIGNAL(jsonDataUpdated(QString)), this, SLOT(jsonDataUpdated(QString)));

    yesOrNO = false;

    ui->nameLineEdit->setPlaceholderText( tr("Beginning with letter,letters or numbers"));
    ui->nameLineEdit->setAttribute(Qt::WA_InputMethodEnabled, false);

//    ui->okBtn->setStyleSheet("QToolButton{background-color:#469cfc;color:#ffffff;border:0px solid rgb(64,153,255);border-radius:3px;}"
//                             "QToolButton:hover{background-color:#62a9f8;}"
//                             "QToolButton:disabled{background-color:#cecece;}");
//    ui->okBtn->setText(tr("Ok"));
    ui->okBtn->setEnabled(false);

//    ui->cancelBtn->setStyleSheet("QToolButton{background-color:#ffffff;color:#282828;border:1px solid #62a9f8;border-radius:3px;}"
//                                 "QToolButton:hover{color:#62a9f8;}");
//    ui->cancelBtn->setText(tr("Cancel"));

    QRegExp regx("[a-z][a-z0-9]+$");
    QValidator *validator = new QRegExpValidator(regx, this);
    ui->nameLineEdit->setValidator( validator );

    ui->nameLineEdit->setFocus();

    gif = new QMovie(":pic2/loading2.gif");
    gif->setScaledSize( QSize(18,18));
    ui->gifLabel->setMovie(gif);
    gif->start();
    ui->gifLabel->hide();

}

NameDialog::~NameDialog()
{
    delete ui;
//    HXChain::getInstance()->removeCurrentDialogVector(this);
}

void NameDialog::on_okBtn_clicked()
{
    yesOrNO = true;
    close();
}

void NameDialog::on_cancelBtn_clicked()
{
    yesOrNO = false;
    close();
}

QString NameDialog::pop()
{
    ui->nameLineEdit->grabKeyboard();


    exec();

    if( yesOrNO == true)
    {
        return ui->nameLineEdit->text();
    }
    else
    {
        return "";
    }
}

bool isExistInWallet(QString name)
{
    QStringList accountList = HXChain::getInstance()->accountInfoMap.keys();
    return accountList.contains(name);
}

void NameDialog::on_nameLineEdit_textChanged(const QString &arg1)
{
    if( arg1.isEmpty())
    {
        ui->okBtn->setEnabled(false);
        ui->addressNameTipLabel1->setPixmap(QPixmap(""));
        ui->addressNameTipLabel2->setText("" );
        return;
    }

    QString addrName = arg1;

    if( 63 < addrName.size() )
    {

        ui->okBtn->setEnabled(false);
        ui->addressNameTipLabel1->setPixmap(QPixmap(":/pic2/wrong.png"));
        ui->addressNameTipLabel2->setText("<body><font style=\"font-size:12px\" color=#FF8880>" + tr("More than 63 characters!") + "</font></body>" );

        return;
    }


    //检查地址名是否在钱包内已经存在
    if( isExistInWallet(addrName) )
    {
        ui->okBtn->setEnabled(false);
        ui->addressNameTipLabel1->setPixmap(QPixmap(":/pic2/wrong.png"));
        ui->addressNameTipLabel2->setText("<body><font style=\"font-size:12px\" color=#FF8880>" + tr( "This name has been used") + "</font></body>" );
        return;
    }

    //检查地址名是否已经在链上注册了 blockchain_get_account(accountName)
    HXChain::getInstance()->postRPC(  "id_blockchain_get_account_" + addrName, toJsonFormat("blockchain_get_account", QJsonArray() << addrName ));
    ui->gifLabel->show();
}

void NameDialog::on_nameLineEdit_returnPressed()
{
    if( !ui->okBtn->isEnabled())  return;

    on_okBtn_clicked();
}

void NameDialog::jsonDataUpdated(QString id)
{
    if( id.mid(0,26) == "id_blockchain_get_account_")
    {
        // 如果跟当前输入框中的内容不一样，则是过时的rpc返回，不用处理
        if( id.mid(26) != ui->nameLineEdit->text())  return;
        QString result = HXChain::getInstance()->jsonDataValue(id);
        ui->gifLabel->hide();

        if( result == "\"result\":null")
        {
            ui->okBtn->setEnabled(true);
            ui->addressNameTipLabel1->setPixmap(QPixmap(":/pic2/correct.png"));
            ui->addressNameTipLabel2->setText("<body><font style=\"font-size:12px\" color=#67B667>" + tr( "You can use this name") + "</font></body>" );
        }
        else
        {
            ui->okBtn->setEnabled(false);
            ui->addressNameTipLabel1->setPixmap(QPixmap(":/pic2/wrong.png"));
            ui->addressNameTipLabel2->setText("<body><font style=\"font-size:12px\" color=#FF8880>" + tr( "This name has been used") + "</font></body>" );
        }

        return;
    }

}

void NameDialog::on_closeBtn_clicked()
{
    close();
}

void NameDialog::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setPen(QPen(QColor(68,217,199),Qt::SolidLine));
    painter.setBrush(QBrush(QColor(68,217,199),Qt::SolidPattern));
    painter.drawRect(0,0,this->width(),40);
}

void NameDialog::mousePressEvent(QMouseEvent *event)
{

    if(event->button() == Qt::LeftButton)
     {
          mouse_press = true;
          //鼠标相对于窗体的位置（或者使用event->globalPos() - this->pos()）
          move_point = event->pos();
     }

}

void NameDialog::mouseMoveEvent(QMouseEvent *event)
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

void NameDialog::mouseReleaseEvent(QMouseEvent *)
{
    mouse_press = false;
}
