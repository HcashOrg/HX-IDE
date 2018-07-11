#include "NameDialogUB.h"
#include "ui_NameDialogUB.h"

#include <QMovie>
#include <QPainter>
#include <QMouseEvent>
#include <QDesktopWidget>

#include "ChainIDE.h"
#include "datamanager/DataManagerUB.h"

NameDialogUB::NameDialogUB(QWidget *parent) :
    MoveableDialog(parent),
    ui(new Ui::NameDialogUB)
{
    ui->setupUi(this);

    setWindowFlags(Qt::FramelessWindowHint);
    move( (QApplication::desktop()->width() - this->width())/2 , (QApplication::desktop()->height() - this->height())/2);

    yesOrNO = false;

    ui->nameLineEdit->setPlaceholderText( tr("Beginning with letter,letters or numbers"));
    ui->nameLineEdit->setAttribute(Qt::WA_InputMethodEnabled, false);

    ui->okBtn->setEnabled(false);

    QRegExp regx("[a-z][a-z0-9]+$");
    QValidator *validator = new QRegExpValidator(regx, this);
    ui->nameLineEdit->setValidator( validator );

    ui->nameLineEdit->setFocus();
}

NameDialogUB::~NameDialogUB()
{
    delete ui;
}

void NameDialogUB::on_okBtn_clicked()
{
    yesOrNO = true;
    close();
}

void NameDialogUB::on_cancelBtn_clicked()
{
    yesOrNO = false;
    close();
}

QString NameDialogUB::pop()
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

void NameDialogUB::on_nameLineEdit_textChanged(const QString &arg1)
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
        ui->addressNameTipLabel1->setPixmap(QPixmap(":/pic/wrong.png"));
        ui->addressNameTipLabel2->setText("<body><font style=\"font-size:12px\" color=#FF8880>" + tr("More than 63 characters!") + "</font></body>" );

        return;
    }

    //检查地址名是否在钱包内已经存在
    if( DataManagerUB::getInstance()->getAccount()->getAccountByName(addrName) )
    {
        ui->okBtn->setEnabled(false);
        ui->addressNameTipLabel1->setPixmap(QPixmap(":/pic/wrong.png"));
        ui->addressNameTipLabel2->setText("<body><font style=\"font-size:12px\" color=#FF8880>" + tr( "This name has been used") + "</font></body>" );
        return;
    }
    else
    {
        ui->okBtn->setEnabled(true);
        ui->addressNameTipLabel1->setPixmap(QPixmap(":/pic/correct.png"));
        ui->addressNameTipLabel2->setText("<body><font style=\"font-size:12px\" color=#67B667>" + tr( "You can use this name") + "</font></body>" );
        return;

    }
}

void NameDialogUB::on_nameLineEdit_returnPressed()
{
    if( !ui->okBtn->isEnabled())  return;

    on_okBtn_clicked();
}

void NameDialogUB::on_closeBtn_clicked()
{
    close();
}
