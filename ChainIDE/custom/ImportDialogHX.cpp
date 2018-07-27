#include "ImportDialogHX.h"
#include "ui_ImportDialogHX.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFileDialog>
#include <QRegExpValidator>

#include "ChainIDE.h"
#include "ConvenientOp.h"
#include "datamanager/DataManagerHX.h"
#include "KeyDataUtil.h"
#include "ImportEnterPWDDialogHX.h"
#include "IDEUtil.h"

ImportDialogHX::ImportDialogHX(QWidget *parent) :
    MoveableDialog(parent),
    ui(new Ui::ImportDialogHX)
{
    ui->setupUi(this);

    setWindowFlags(Qt::FramelessWindowHint);

    ui->name->setPlaceholderText( tr("Beginning with letter,letters or numbers"));
    ui->name->setAttribute(Qt::WA_InputMethodEnabled, false);

    QRegExp regx("[a-z][a-z0-9]+$");
    QValidator *validator = new QRegExpValidator(regx, this);

    //ui->privatekey->setValidator( validator );
    ui->name->setValidator( validator );
    ui->privatekey->setEnabled(false);

    ui->name->setFocus();
    connect(ChainIDE::getInstance(),&ChainIDE::jsonDataUpdated,this,&ImportDialogHX::jsonDataUpdated);

}

ImportDialogHX::~ImportDialogHX()
{
    delete ui;
}

void ImportDialogHX::on_okBtn_clicked()
{
    if(ui->name->text().isEmpty())   return;
    if(ui->privatekey->text().isEmpty())    return;

    KeyDataPtr data = std::make_shared<KeyDataInfo>();
    if(KeyDataUtil::ReadaPrivateKeyFromPath(ui->privatekey->text(),data))
    {
        //解密
        if(ui->privatekey->text().endsWith(".lpk"))
        {
            data->DecryptBase64();
        }
        else if(ui->privatekey->text().endsWith(".elpk"))
        {
            ImportEnterPwdDialogHX importEnterPwdDialog;
            if(importEnterPwdDialog.pop())
            {
                data->DecryptAES(importEnterPwdDialog.pwd);
            }
        }
        else
        {
            return;
        }
    }
    else
    {
        ConvenientOp::ShowSyncCommonDialog( tr("Wrong file path."));
        return;
    }

    //验证该地址是否已经存在
    if(DataManagerHX::getInstance()->getAccount()->getAccountByAddr(data->HXAddr))
    {
        ConvenientOp::ShowSyncCommonDialog( tr("HX Address:%1  Already Exists!").arg(data->HXAddr));
        close();
        return;
    }

    //开始导入私钥
    for(auto it = data->info_key.begin();it != data->info_key.end();++it)
    {
        if((*it).first == "HX")
        {
            //导入hx私钥
            ChainIDE::getInstance()->postRPC( "import-import_key",
                                             IDEUtil::toJsonFormat( "import_key", QJsonArray() << ui->name->text() << (*it).second));
        }
        else
        {
            //导入跨连私钥
            ChainIDE::getInstance()->postRPC( "import-import_crosschain_key",
                                             IDEUtil::toJsonFormat( "import_crosschain_key", QJsonArray() << (*it).second <<(*it).first));
        }
    }
    ChainIDE::getInstance()->postRPC( "import-finish_import_key", IDEUtil::toJsonFormat( "finish_import_key", QJsonArray()));

}

void ImportDialogHX::on_closeBtn_clicked()
{
    close();
}

void ImportDialogHX::jsonDataUpdated(const QString &id, const QString &data)
{
    if("import-import_key" == id)
    {
        QJsonParseError json_error;
        QJsonDocument parse_doucment = QJsonDocument::fromJson(data.toLatin1(),&json_error);
        if(json_error.error != QJsonParseError::NoError || !parse_doucment.isObject())
        {
            qDebug()<<json_error.errorString();
            ConvenientOp::ShowSyncCommonDialog(json_error.errorString());
        }

        if(parse_doucment.object().value("result").toBool())
        {/*
            DataManagerHX::getInstance()->queryAccount();*/
        }
        else
        {
            ConvenientOp::ShowSyncCommonDialog(data);
        }
    }
    else if("import-finish_import_key" == id)
    {
        DataManagerHX::getInstance()->queryAccount();
        close();
    }
}

void ImportDialogHX::on_cancelBtn_clicked()
{
    close();
}

void ImportDialogHX::on_pathBtn_clicked()
{
    QString file = QFileDialog::getOpenFileName(this, tr("Choose your private key file."),"","(*.lpk *.elpk)");
    ui->privatekey->setText(file);
}
