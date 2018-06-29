#include "commondialog.h"
#include "ui_commondialog.h"
#include <QDebug>


CommonDialog::CommonDialog(commonDialogType _type, QWidget *parent) :
    MoveableDialog(parent),
    ui(new Ui::CommonDialog)
{
    ui->setupUi(this);

    setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);


    yesOrNO = false;

    if( _type == OkAndCancel)
    {
    }
    else if( _type == OkOnly)
    {
        ui->cancelBtn->hide();
    }
    else if( _type == YesOrNo)
    {
        ui->okBtn->setText(tr("Yes"));
        ui->cancelBtn->setText(tr("No"));
    }    
}

CommonDialog::~CommonDialog()
{
    delete ui;
}

void CommonDialog::on_okBtn_clicked()
{
    yesOrNO = true;
    close();
}

void CommonDialog::on_cancelBtn_clicked()
{
    yesOrNO = false;
    close();
}

void CommonDialog::on_closeBtn_clicked()
{
    yesOrNO = false;
    close();
}

bool CommonDialog::pop()
{
    exec();
    return yesOrNO;
}

void CommonDialog::setText(QString text)
{
    ui->textBrowser->setText(text);

}
