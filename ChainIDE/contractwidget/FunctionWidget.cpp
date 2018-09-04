#include "FunctionWidget.h"
#include "ui_FunctionWidget.h"

#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QDebug>

#include <mutex>


#include "ChainIDE.h"
#include "datamanager/DataManagerHX.h"
#include "datamanager/DataManagerUB.h"

static std::mutex datamutex;
FunctionWidget::FunctionWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FunctionWidget)
{
    ui->setupUi(this);
    InitWidget();
}

FunctionWidget::~FunctionWidget()
{
    delete ui;
}

void FunctionWidget::RefreshContractAddr(const QString &addr)
{
    ui->treeWidget_offline->clear();
    ui->treeWidget_online->clear();
    ui->tabWidget->setCurrentIndex(0);

    DataManagerStruct::ContractInfoPtr info = nullptr;
    if(ChainIDE::getInstance()->getChainClass() == DataDefine::HX)
    {
        info = DataManagerHX::getInstance()->getContract()->getContractInfo(addr);
    }
    else if(ChainIDE::getInstance()->getChainClass() == DataDefine::UB)
    {
        info = DataManagerUB::getInstance()->getContract()->getContractInfo(addr);
    }

    if(!info) return;

    foreach(QString api, info->GetInterface()->getAllApiName())
    {
        qDebug()<<api;
        QTreeWidgetItem *itemChild = new QTreeWidgetItem(QStringList()<<api);
        ui->treeWidget_online->addTopLevelItem(itemChild);
    }
    foreach(QString offapi, info->GetInterface()->getAllOfflineApiName())
    {
        QTreeWidgetItem *itemChild = new QTreeWidgetItem(QStringList()<<offapi);
        ui->treeWidget_offline->addTopLevelItem(itemChild);
    }
}

void FunctionWidget::Clear()
{
    ui->treeWidget_offline->clear();
    ui->treeWidget_online->clear();
    ui->tabWidget->setCurrentIndex(0);
}

void FunctionWidget::InitWidget()
{
    ui->treeWidget_offline->header()->setVisible(false);
    ui->treeWidget_online->header()->setVisible(false);
}

void FunctionWidget::retranslator()
{
    ui->retranslateUi(this);
}
