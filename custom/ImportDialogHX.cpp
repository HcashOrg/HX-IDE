#include "ImportDialogHX.h"
#include "ui_ImportDialogHX.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDesktopWidget>

#include "ChainIDE.h"
#include "ConvenientOp.h"
#include "datamanager/DataManagerHX.h"
#include "IDEUtil.h"

ImportDialogHX::ImportDialogHX(QWidget *parent) :
    MoveableDialog(parent),
    ui(new Ui::ImportDialogHX)
{
    ui->setupUi(this);

    setWindowFlags(Qt::FramelessWindowHint);
    move( (QApplication::desktop()->width() - this->width())/2 , (QApplication::desktop()->height() - this->height())/2);

    ui->name->setPlaceholderText( tr("Beginning with letter,letters or numbers"));
    ui->name->setAttribute(Qt::WA_InputMethodEnabled, false);

    QRegExp regx("[a-z][a-z0-9]+$");
    QValidator *validator = new QRegExpValidator(regx, this);

    ui->privatekey->setValidator( validator );
    ui->name->setValidator( validator );

    ui->name->setFocus();
    connect(ChainIDE::getInstance(),&ChainIDE::jsonDataUpdated,this,&ImportDialogHX::jsonDataUpdated);

}

ImportDialogHX::~ImportDialogHX()
{
    delete ui;
}

void ImportDialogHX::on_okBtn_clicked()
{

    ChainIDE::getInstance()->postRPC( "import-import_key",
                                     IDEUtil::toJsonFormat( "import_key", QJsonArray() <<ui->name->text() << ui->privatekey->text()));

}

void ImportDialogHX::on_privatekey_returnPressed()
{
    on_okBtn_clicked();
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
            close();
            return;
        }

        if(parse_doucment.object().value("result").toBool())
        {
            DataManagerHX::getInstance()->queryAccount();
            close();
        }
        else
        {
            ConvenientOp::ShowSyncCommonDialog(data);
            close();
        }
    }
}

void ImportDialogHX::on_cancelBtn_clicked()
{
    close();
}
