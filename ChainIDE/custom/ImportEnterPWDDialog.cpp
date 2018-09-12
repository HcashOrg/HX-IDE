#include "ImportEnterPwdDialog.h"
#include "ui_ImportEnterPwdDialog.h"

#include <QDebug>

ImportEnterPwdDialog::ImportEnterPwdDialog(QWidget *parent) :
    MoveableDialog(parent),
    ui(new Ui::ImportEnterPwdDialog)
{
    ui->setupUi(this);

    setWindowFlags(Qt::FramelessWindowHint | windowFlags());

    ui->pkPwdLineEdit->setMaxLength(14);

    ui->pkPwdLineEdit->setContextMenuPolicy(Qt::NoContextMenu);
    ui->pkPwdLineEdit->setFocus();

    ui->okBtn->setEnabled(false);
}

ImportEnterPwdDialog::~ImportEnterPwdDialog()
{
    delete ui;
}

bool ImportEnterPwdDialog::pop()
{
    exec();

    return yesOrNO;
}

void ImportEnterPwdDialog::on_okBtn_clicked()
{
    if(ui->pkPwdLineEdit->text().isEmpty())     return;
    pwd = ui->pkPwdLineEdit->text();
    yesOrNO = true;
    close();
}

void ImportEnterPwdDialog::on_cancelBtn_clicked()
{
    yesOrNO = false;
    close();
}

void ImportEnterPwdDialog::on_pkPwdLineEdit_textEdited(const QString &arg1)
{
    checkOkBtn();
}

void ImportEnterPwdDialog::checkOkBtn()
{
    if( ui->pkPwdLineEdit->text().size() < 8 || ui->pkPwdLineEdit->text().size() > 14)
    {
        ui->okBtn->setEnabled(false);
        return;
    }

    ui->okBtn->setEnabled(true);
}

void ImportEnterPwdDialog::on_closeBtn_clicked()
{
    close();
}
