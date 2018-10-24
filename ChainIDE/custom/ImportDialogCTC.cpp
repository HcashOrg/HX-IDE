#include "ImportDialogCTC.h"
#include "ui_ImportDialogCTC.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFileDialog>
#include <QRegExpValidator>
#include <QDebug>

#include "ChainIDE.h"
#include "ConvenientOp.h"
#include "datamanager/DataManagerCTC.h"
#include "ImportEnterPWDDialog.h"
#include "IDEUtil.h"
#include "AES/aesencryptor.h"

ImportDialogCTC::ImportDialogCTC(QWidget *parent) :
    MoveableDialog(parent),
    ui(new Ui::ImportDialogCTC)
{
    ui->setupUi(this);

    setWindowFlags(Qt::FramelessWindowHint);

    ui->name->setPlaceholderText( tr("Beginning with letter,letters or numbers"));
    ui->name->setAttribute(Qt::WA_InputMethodEnabled, false);

    QRegExp regx("[a-z][a-z0-9]+$");
    QValidator *validator = new QRegExpValidator(regx, this);

    //ui->privatekey->setValidator( validator );
    ui->name->setValidator( validator );

    ui->name->setFocus();
    connect(ChainIDE::getInstance(),&ChainIDE::jsonDataUpdated,this,&ImportDialogCTC::jsonDataUpdated);
}

ImportDialogCTC::~ImportDialogCTC()
{
    delete ui;
}

void ImportDialogCTC::on_okBtn_clicked()
{
    if(ui->name->text().isEmpty())   return;
    if(ui->privatekey->text().isEmpty())    return;

    QString priKey = "";

    // 如果输入框中是 私钥文件的地址
    if( ui->privatekey->text().endsWith(".key") )
    {
        QFile file( ui->privatekey->text());
        if( file.open(QIODevice::ReadOnly))
        {
            QByteArray rd = file.readAll();
            file.close();
            priKey = QByteArray::fromBase64( rd);
        }
        else
        {
            ConvenientOp::ShowSyncCommonDialog( tr("Wrong file path."));
            return;
        }

    }
    else if( ui->privatekey->text().endsWith(".hpk") )      // 如果是加密后的私钥文件
    {
        ImportEnterPwdDialog importEnterPwdDialog;
        if(importEnterPwdDialog.pop())
        {
            QFile file( ui->privatekey->text());
            if( file.open(QIODevice::ReadOnly))
            {
                QByteArray rd = file.readAll();
                file.close();
                QString str(rd);

                QString pwd = importEnterPwdDialog.pwd;
                unsigned char key2[16] = {0};
                memcpy(key2,pwd.toUtf8().data(),pwd.toUtf8().size());
                AesEncryptor aes(key2);
                QString output = QString::fromStdString( aes.DecryptString( str.toStdString()) );


                if( output.startsWith("privateKey="))
                {
                    priKey = output.mid( QString("privateKey=").size());
                }
                else
                {
                    ConvenientOp::ShowSyncCommonDialog( tr("Wrong password!"));
                    return;
                }

            }
            else
            {
                ConvenientOp::ShowSyncCommonDialog( tr("Wrong file path."));
                return;
            }
        }
        else
        {
            return;
        }

    }
    else // 如果输入框中是 私钥
    {
        priKey = ui->privatekey->text().simplified();
    }

    ChainIDE::getInstance()->postRPC( "id_wallet_import_private_key",IDEUtil::toJsonFormat( "wallet_import_private_key", QJsonArray() << priKey<<ui->name->text().simplified()<<true,true));

}

void ImportDialogCTC::on_cancelBtn_clicked()
{
    close();
}

void ImportDialogCTC::on_closeBtn_clicked()
{
    close();
}

void ImportDialogCTC::jsonDataUpdated(const QString &id, const QString &data)
{
    if("id_wallet_import_private_key" == id)
    {
        QJsonParseError json_error;
        QJsonDocument parse_doucment = QJsonDocument::fromJson(data.toUtf8(),&json_error);
        if(json_error.error != QJsonParseError::NoError || !parse_doucment.isObject())
        {
            qDebug()<<json_error.errorString();
            ConvenientOp::ShowSyncCommonDialog(json_error.errorString());
        }

        if(parse_doucment.object().value("result").toBool())
        {
            DataManagerCTC::getInstance()->queryAccount();
        }
        else
        {
            ConvenientOp::ShowSyncCommonDialog(data);
            close();
        }
    }

}

void ImportDialogCTC::on_pathBtn_clicked()
{
    QString file = QFileDialog::getOpenFileName(this, tr("Choose your private key file."),"","(*.key *.hpk)");

    ui->privatekey->setText(file);
}
