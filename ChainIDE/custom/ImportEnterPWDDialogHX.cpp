#include "ImportEnterPwdDialogHX.h"
#include "ui_ImportEnterPwdDialogHX.h"

#include <QDebug>

ImportEnterPwdDialogHX::ImportEnterPwdDialogHX(QWidget *parent) :
    MoveableDialog(parent),
    ui(new Ui::ImportEnterPwdDialogHX)
{
    ui->setupUi(this);

    setWindowFlags(Qt::FramelessWindowHint | windowFlags());

    ui->pkPwdLineEdit->setMaxLength(14);

    ui->pkPwdLineEdit->setContextMenuPolicy(Qt::NoContextMenu);
    ui->pkPwdLineEdit->setFocus();

    ui->okBtn->setEnabled(false);
}

ImportEnterPwdDialogHX::~ImportEnterPwdDialogHX()
{
    delete ui;
}

bool ImportEnterPwdDialogHX::pop()
{
    exec();

    return yesOrNO;
}

void ImportEnterPwdDialogHX::on_okBtn_clicked()
{
    if(ui->pkPwdLineEdit->text().isEmpty())     return;
    pwd = ui->pkPwdLineEdit->text();
    yesOrNO = true;
    close();
}

void ImportEnterPwdDialogHX::on_cancelBtn_clicked()
{
    yesOrNO = false;
    close();
}

void ImportEnterPwdDialogHX::on_pkPwdLineEdit_textEdited(const QString &arg1)
{
    checkOkBtn();
}

void ImportEnterPwdDialogHX::checkOkBtn()
{
    if( ui->pkPwdLineEdit->text().size() < 8 || ui->pkPwdLineEdit->text().size() > 14)
    {
        ui->okBtn->setEnabled(false);
        return;
    }

    ui->okBtn->setEnabled(true);
}

void ImportEnterPwdDialogHX::on_closeBtn_clicked()
{
    close();
}
