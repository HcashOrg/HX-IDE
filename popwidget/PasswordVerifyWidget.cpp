#include "PasswordVerifyWidget.h"
#include "ui_PasswordVerifyWidget.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QJsonValue>

#include "ChainIDE.h"
#include "IDEUtil.h"
#include "DataManager.h"

PasswordVerifyWidget::PasswordVerifyWidget(QWidget *parent) :
    MoveableDialog(parent),
    ui(new Ui::PasswordVerifyWidget)
{
    ui->setupUi(this);
    InitWidget();
}

PasswordVerifyWidget::~PasswordVerifyWidget()
{
    delete ui;
}

bool PasswordVerifyWidget::pop()
{
    exec();
    return confirm;
}

void PasswordVerifyWidget::ConfirmSlots()
{
    confirm = true;
    close();
}

void PasswordVerifyWidget::passwordChangeSlots(const QString &address)
{
    ui->okBtn->setEnabled(false);
    if( ui->lineEdit->text().size() < 8)
    {
        return;
    }

    ChainIDE::getInstance()->postRPC( "verify_unlock-lockpage", IDEUtil::toJsonFormat( "unlock", QJsonArray() << ui->lineEdit->text() ));
}

void PasswordVerifyWidget::jsonDataUpdated(const QString &id, const QString &data)
{
    if( id == "verify_unlock-lockpage")
    {
        qDebug()<<data;
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

void PasswordVerifyWidget::passwordReturnPressed()
{
    if(ui->okBtn->isEnabled())
    {
        ConfirmSlots();
    }
}

void PasswordVerifyWidget::InitWidget()
{
    ChainIDE::getInstance()->getDataManager()->dealNewState();
    setWindowFlags(windowFlags()| Qt::FramelessWindowHint);
    confirm = false;
    ui->okBtn->setEnabled(false);
    ui->tip->setVisible(false);
    connect(ui->closeBtn,&QToolButton::clicked,this,&PasswordVerifyWidget::close);
    connect(ui->cancelBtn,&QToolButton::clicked,this,&PasswordVerifyWidget::close);
    connect(ui->okBtn,&QToolButton::clicked,this,&PasswordVerifyWidget::ConfirmSlots);
    connect(ui->lineEdit,&QLineEdit::textEdited,this,&PasswordVerifyWidget::passwordChangeSlots);
    connect(ui->lineEdit,&QLineEdit::returnPressed,this,&PasswordVerifyWidget::passwordReturnPressed);
    connect(ChainIDE::getInstance(),&ChainIDE::jsonDataUpdated,this,&PasswordVerifyWidget::jsonDataUpdated);
}
