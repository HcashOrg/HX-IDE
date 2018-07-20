#include "ContractWidgetUB.h"
#include "ui_ContractWidgetUB.h"

#include <QCoreApplication>
#include <QDir>

#include "ChainIDE.h"
#include "datamanager/DataManagerUB.h"
#include "ConvenientOp.h"

ContractWidgetUB::ContractWidgetUB(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ContractWidgetUB)
{
    ui->setupUi(this);

    InitWidget();
}

ContractWidgetUB::~ContractWidgetUB()
{
    delete ui;
}

void ContractWidgetUB::RefreshTree()
{
    ui->treeWidget->clear();
    DataDefine::AddressContractDataPtr data = std::make_shared<DataDefine::AddressContractData>();
    QString contractPath = ChainIDE::getInstance()->getCurrentChainType() == DataDefine::TEST? DataDefine::LOCAL_CONTRACT_TEST_PATH : DataDefine::LOCAL_CONTRACT_FORMAL_PATH;
    ConvenientOp::ReadContractFromFile(QCoreApplication::applicationDirPath()+QDir::separator()+contractPath,data);
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

void ContractWidgetUB::ContractClicked(QTreeWidgetItem *item, QTreeWidgetItem *itempre)
{
    if(item)
    {
        ui->functionWidget->RefreshContractAddr(item->text(0));
    }
}

void ContractWidgetUB::InitWidget()
{
    ui->treeWidget->header()->setVisible(false);
    ui->splitter->setSizes(QList<int>()<<0.66*this->height()<<0.34*this->height());


    if(ChainIDE::getInstance()->getChainClass() == DataDefine::UB)
    {
        connect(ui->treeWidget,&QTreeWidget::currentItemChanged,this,&ContractWidgetUB::ContractClicked);
    }
}

void ContractWidgetUB::retranslator()
{
    ui->retranslateUi(this);
    ui->functionWidget->retranslator();
}
