#include "ContractWidget.h"
#include "ui_ContractWidget.h"

#include <QCoreApplication>
#include <QDir>

#include "ChainIDE.h"
#include "DataManager.h"
#include "ConvenientOp.h"

ContractWidget::ContractWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ContractWidget)
{
    ui->setupUi(this);
    InitWidget();
}

ContractWidget::~ContractWidget()
{
    delete ui;
}

void ContractWidget::RefreshTree()
{
    ui->treeWidget->clear();
    DataDefine::AddressContractDataPtr data = std::make_shared<DataDefine::AddressContractData>();
    ConvenientOp::ReadContractFromFile(QCoreApplication::applicationDirPath()+QDir::separator()+DataDefine::LOCAL_CONTRACT_PATH,data);

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

void ContractWidget::ContractClicked(QTreeWidgetItem *item, QTreeWidgetItem *itempre)
{
    if(item)
    {
        ui->functionWidget->RefreshContractAddr(item->text(0));
    }
}

void ContractWidget::InitWidget()
{
    ui->treeWidget->header()->setVisible(false);
    ui->splitter->setSizes(QList<int>()<<0.66*this->height()<<0.34*this->height());

    connect(ui->treeWidget,&QTreeWidget::currentItemChanged,this,&ContractWidget::ContractClicked);


}

void ContractWidget::retranslator()
{
    ui->retranslateUi(this);
    ui->functionWidget->retranslator();
}
