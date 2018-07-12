#include "PasswordVerifyWidgetHX.h"
#include "ui_PasswordVerifyWidgetHX.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QJsonValue>

#include "ChainIDE.h"
#include "IDEUtil.h"
#include "datamanager/DataManagerHX.h"

PasswordVerifyWidgetHX::PasswordVerifyWidgetHX(QWidget *parent) :
    MoveableDialog(parent),
    ui(new Ui::PasswordVerifyWidgetHX)
{
    ui->setupUi(this);
    InitWidget();
}

PasswordVerifyWidgetHX::~PasswordVerifyWidgetHX()
{
    delete ui;
}

bool PasswordVerifyWidgetHX::pop()
{
    exec();
    return confirm;
}

void PasswordVerifyWidgetHX::ConfirmSlots()
{
    confirm = true;
    close();
}

void PasswordVerifyWidgetHX::passwordChangeSlots(const QString &address)
{
    ui->okBtn->setEnabled(false);
    if( ui->lineEdit->text().size() < 8)
    {
        return;
    }

    ChainIDE::getInstance()->postRPC( "verify_unlock-lockpage", IDEUtil::toJsonFormat( "unlock", QJsonArray() << ui->lineEdit->text() ));
}

void PasswordVerifyWidgetHX::jsonDataUpdated(const QString &id, const QString &data)
{
    if( id == "verify_unlock-lockpage")
    {
        QJsonParseError json_error;
        QJsonDocument parse_doucment = QJsonDocument::fromJson(data.toLatin1(),&json_error);
        if(json_error.error != QJsonParseError::NoError || !parse_doucment.isObject())
        {
             qDebug()<<json_error.errorString();
             return ;
        }
        if( parse_doucment.object().value("result").isNull())
        {
            ui->tip->setVisible(false);
            ui->okBtn->setEnabled(true);
        }
        else
        {
            ui->tip->setVisible(true);
            ui->okBtn->setEnabled(false);
        }
    }
}

void PasswordVerifyWidgetHX::passwordReturnPressed()
{
    if(ui->okBtn->isEnabled())
    {
        ConfirmSlots();
    }
}

void PasswordVerifyWidgetHX::InitWidget()
{
    DataManagerHX::getInstance()->dealNewState();
    setWindowFlags(windowFlags()| Qt::FramelessWindowHint);
    confirm = false;
    ui->okBtn->setEnabled(false);
    ui->tip->setVisible(false);
    connect(ui->closeBtn,&QToolButton::clicked,this,&PasswordVerifyWidgetHX::close);
    connect(ui->cancelBtn,&QToolButton::clicked,this,&PasswordVerifyWidgetHX::close);
    connect(ui->okBtn,&QToolButton::clicked,this,&PasswordVerifyWidgetHX::ConfirmSlots);
    connect(ui->lineEdit,&QLineEdit::textEdited,this,&PasswordVerifyWidgetHX::passwordChangeSlots);
    connect(ui->lineEdit,&QLineEdit::returnPressed,this,&PasswordVerifyWidgetHX::passwordReturnPressed);
    connect(ChainIDE::getInstance(),&ChainIDE::jsonDataUpdated,this,&PasswordVerifyWidgetHX::jsonDataUpdated);
}
