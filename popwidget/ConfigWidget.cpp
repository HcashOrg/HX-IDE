#include "ConfigWidget.h"
#include "ui_ConfigWidget.h"

#include "DataDefine.h"
#include "ChainIDE.h"
ConfigWidget::ConfigWidget(QWidget *parent) :
    MoveableDialog(parent),
    ui(new Ui::ConfigWidget)
{
    ui->setupUi(this);
    InitWidget();
}

ConfigWidget::~ConfigWidget()
{
    delete ui;
}

bool ConfigWidget::pop()
{
    exec();
    return isOk;
}

void ConfigWidget::ConfirmSlots()
{
    qDebug()<<static_cast<DataDefine::Language>(ui->comboBox->currentData().toInt());
    ChainIDE::getInstance()->setCurrentLanguage(static_cast<DataDefine::Language>(ui->comboBox->currentData().toInt()));
    isOk = true;
    close();
}

void ConfigWidget::InitWidget()
{
    setWindowFlags(Qt::FramelessWindowHint | windowFlags());
    isOk = false;
    //初始化comboBox
    ui->comboBox->clear();
    ui->comboBox->addItem(tr("  English"),static_cast<int>(DataDefine::English));
    ui->comboBox->addItem(tr("  Chinese"),static_cast<int>(DataDefine::SimChinese));
    if(ChainIDE::getInstance()->getCurrentLanguage() ==DataDefine::English)
    {
        ui->comboBox->setCurrentIndex(0);
    }
    else if(ChainIDE::getInstance()->getCurrentLanguage() ==DataDefine::SimChinese)
    {
        ui->comboBox->setCurrentIndex(1);
    }

    connect(ui->cancelBtn,&QToolButton::clicked,this,&ConfigWidget::close);
    connect(ui->closeBtn,&QToolButton::clicked,this,&ConfigWidget::close);
    connect(ui->okBtn,&QToolButton::clicked,this,&ConfigWidget::ConfirmSlots);
}
