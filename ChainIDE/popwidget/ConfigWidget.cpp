#include "ConfigWidget.h"
#include "ui_ConfigWidget.h"

#include "DataDefine.h"
#include "ChainIDE.h"
#include "ConvenientOp.h"
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
    ChainIDE::getInstance()->setCurrentLanguage(static_cast<DataDefine::Language>(ui->language->currentData().toInt()));
    ChainIDE::getInstance()->setCurrentTheme(static_cast<DataDefine::ThemeStyle>(ui->theme->currentData().toInt()));
    ChainIDE::getInstance()->setChainClass(static_cast<DataDefine::BlockChainClass>(ui->chainclass->currentData().toInt()));
    ChainIDE::getInstance()->setStartChainTypes(static_cast<DataDefine::ChainTypes>(ui->starttype->currentData().toUInt()));
    isOk = true;

    if(ChainIDE::getInstance()->getCurrentTheme() != ui->theme->currentData().toInt() ||
       ChainIDE::getInstance()->getChainClass() != ui->chainclass->currentData().toInt() ||
       ChainIDE::getInstance()->getStartChainTypes() != ui->starttype->currentData().toInt())
    {
        ConvenientOp::ShowSyncCommonDialog(tr("the change will take effect after restart!"));
    }
    close();
}

void ConfigWidget::InitWidget()
{
    setWindowFlags(Qt::FramelessWindowHint | windowFlags());
    isOk = false;
    //初始化language
    ui->language->clear();
    ui->language->addItem(tr("  English"),static_cast<int>(DataDefine::English));
    ui->language->addItem(tr("  Chinese"),static_cast<int>(DataDefine::SimChinese));
    if(ChainIDE::getInstance()->getCurrentLanguage() ==DataDefine::English)
    {
        ui->language->setCurrentIndex(0);
    }
    else if(ChainIDE::getInstance()->getCurrentLanguage() ==DataDefine::SimChinese)
    {
        ui->language->setCurrentIndex(1);
    }

    //初始化主题
    ui->theme->clear();
    ui->theme->addItem(tr("  Black"),static_cast<int>(DataDefine::Black_Theme));
    ui->theme->addItem(tr("  White"),static_cast<int>(DataDefine::White_Theme));
    if(ChainIDE::getInstance()->getCurrentTheme() ==DataDefine::Black_Theme)
    {
        ui->theme->setCurrentIndex(0);
    }
    else if(ChainIDE::getInstance()->getCurrentTheme() ==DataDefine::White_Theme)
    {
        ui->theme->setCurrentIndex(1);
    }

    //初始化链类型
    ui->chainclass->clear();
    ui->chainclass->addItem(tr("  UB"),static_cast<int>(DataDefine::UB));
    ui->chainclass->addItem(tr("  HX"),static_cast<int>(DataDefine::HX));
    ui->chainclass->addItem(tr("  CTC"),static_cast<int>(DataDefine::CTC));
    if(ChainIDE::getInstance()->getChainClass() ==DataDefine::UB)
    {
        ui->chainclass->setCurrentIndex(0);
    }
    else if(ChainIDE::getInstance()->getChainClass() ==DataDefine::HX)
    {
        ui->chainclass->setCurrentIndex(1);
    }
    else if(ChainIDE::getInstance()->getChainClass() ==DataDefine::CTC)
    {
        ui->chainclass->setCurrentIndex(2);
    }
    //初始化启动类型
    ui->starttype->clear();
    ui->starttype->addItem(tr("  All"),static_cast<int>(DataDefine::TEST|DataDefine::FORMAL));
    ui->starttype->addItem(tr("  Test"),static_cast<int>(DataDefine::TEST));
    ui->starttype->addItem(tr("  Formal"),static_cast<int>(DataDefine::FORMAL));
    ui->starttype->addItem(tr("  None"),static_cast<int>(DataDefine::NONE));
    if(ChainIDE::getInstance()->getStartChainTypes() ==DataDefine::TEST)
    {
        ui->starttype->setCurrentIndex(1);
    }
    else if(ChainIDE::getInstance()->getStartChainTypes() ==DataDefine::FORMAL)
    {
        ui->starttype->setCurrentIndex(2);
    }
    else if(ChainIDE::getInstance()->getStartChainTypes() ==DataDefine::NONE)
    {
        ui->starttype->setCurrentIndex(3);
    }
    else if((ChainIDE::getInstance()->getStartChainTypes() & DataDefine::TEST) &&(ChainIDE::getInstance()->getStartChainTypes() & DataDefine::FORMAL))
    {
        ui->starttype->setCurrentIndex(0);
    }

    connect(ui->cancelBtn,&QToolButton::clicked,this,&ConfigWidget::close);
    connect(ui->closeBtn,&QToolButton::clicked,this,&ConfigWidget::close);
    connect(ui->okBtn,&QToolButton::clicked,this,&ConfigWidget::ConfirmSlots);
}
