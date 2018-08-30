#include "ContractWidgetHX.h"
#include "ui_ContractWidgetHX.h"

#include <QCoreApplication>
#include <QDir>

#include "ChainIDE.h"
#include "datamanager/DataManagerHX.h"
#include "ConvenientOp.h"

ContractWidgetHX::ContractWidgetHX(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ContractWidgetHX)
{
    ui->setupUi(this);
    InitWidget();
}

ContractWidgetHX::~ContractWidgetHX()
{
    delete ui;
}

void ContractWidgetHX::RefreshTree()
{
    if(ChainIDE::getInstance()->getChainClass() == DataDefine::HX)
    {
        DataManagerHX::getInstance()->queryAccount();
    }
}

void ContractWidgetHX::ContractClicked(QTreeWidgetItem *item, QTreeWidgetItem *itempre)
{
    if(item)
    {
        ui->functionWidget->RefreshContractAddr(item->text(0));
    }
}

void ContractWidgetHX::InitWidget()
{
    ui->treeWidget->header()->setVisible(false);
    ui->splitter->setSizes(QList<int>()<<0.66*this->height()<<0.34*this->height());

    connect(ui->treeWidget,&QTreeWidget::currentItemChanged,this,&ContractWidgetHX::ContractClicked);

    if(ChainIDE::getInstance()->getChainClass() == DataDefine::HX && (ChainIDE::getInstance()->getStartChainTypes() | DataDefine::NONE))
    {
        connect(DataManagerHX::getInstance(),&DataManagerHX::queryAccountFinish,DataManagerHX::getInstance(),&DataManagerHX::queryContract);
        connect(DataManagerHX::getInstance(),&DataManagerHX::queryContractFinish,this,&ContractWidgetHX::InitTree);
    }
}

void ContractWidgetHX::InitTree()
{
    ui->treeWidget->clear();
    DataManagerStruct::AddressContractDataPtr data = DataManagerHX::getInstance()->getContract();

    for(auto it = data->getAllData().begin();it != data->getAllData().end();++it)
    {
        QTreeWidgetItem *item = new QTreeWidgetItem(QStringList()<<(*it)->GetOwnerAddr());
        item->setFlags(Qt::ItemIsEnabled);
        item->setTextAlignment(0,Qt::AlignCenter);
        ui->treeWidget->addTopLevelItem(item);
        for(auto cont = (*it)->GetContracts().begin();cont != (*it)->GetContracts().end();++cont)
        {
            QTreeWidgetItem *childitem = new QTreeWidgetItem(QStringList()<<(*cont)->GetContractAddr());
            childitem->setTextAlignment(0,Qt::AlignCenter);
            item->addChild(childitem);
        }
    }
    ui->treeWidget->expandAll();
}

void ContractWidgetHX::retranslator()
{
    ui->retranslateUi(this);
    ui->functionWidget->retranslator();
}
