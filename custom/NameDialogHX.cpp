#include "NameDialogHX.h"
#include "ui_NameDialogHX.h"

#include <QDesktopWidget>

#include "ChainIDE.h"
#include "datamanager/DataManagerHX.h"

NameDialogHX::NameDialogHX(QWidget *parent) :
    MoveableDialog(parent),
    ui(new Ui::NameDialogHX)
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

NameDialogHX::~NameDialogHX()
{
    delete ui;
}

void NameDialogHX::on_okBtn_clicked()
{
    yesOrNO = true;
    close();
}

void NameDialogHX::on_cancelBtn_clicked()
{
    yesOrNO = false;
    close();
}

QString NameDialogHX::pop()
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

void NameDialogHX::on_nameLineEdit_textChanged(const QString &arg1)
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
    if( DataManagerHX::getInstance()->getAccount()->getAccountByName(addrName) )
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

void NameDialogHX::on_nameLineEdit_returnPressed()
{
    if( !ui->okBtn->isEnabled())  return;

    on_okBtn_clicked();
}

void NameDialogHX::on_closeBtn_clicked()
{
    close();
}
