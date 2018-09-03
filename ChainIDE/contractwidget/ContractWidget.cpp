#include "ContractWidget.h"
#include "ui_ContractWidget.h"

#include <QCoreApplication>
#include <QGuiApplication>
#include <QClipboard>
#include <QDir>
#include <QMenu>

#include "ChainIDE.h"
#include "datamanager/DataManagerHX.h"
#include "datamanager/DataManagerUB.h"
#include "ConvenientOp.h"

class ContractWidget::DataPrivate
{
public:
    DataPrivate()
        :contextMenu(new QMenu())
    {

    }
public:
    QMenu *contextMenu;//右键菜单
};

Q_DECLARE_METATYPE(DataManagerStruct::ContractInfoPtr)
Q_DECLARE_METATYPE(DataManagerStruct::AddressContractPtr)

ContractWidget::ContractWidget(QWidget *parent) :
    QWidget(parent),
    _p(new DataPrivate()),
    ui(new Ui::ContractWidget)
{
    ui->setupUi(this);
    InitWidget();
}

ContractWidget::~ContractWidget()
{
    delete _p;
    _p = nullptr;
    delete ui;
}

void ContractWidget::RefreshTree()
{
    if(ChainIDE::getInstance()->getChainClass() == DataDefine::HX)
    {
        DataManagerHX::getInstance()->queryAccount();
    }
    else if(ChainIDE::getInstance()->getChainClass() == DataDefine::UB)
    {
        DataManagerUB::getInstance()->queryContract();
    }
}

void ContractWidget::ContractClicked(QTreeWidgetItem *item, int column)
{
    if(item && item->parent())
    {
        ui->functionWidget->RefreshContractAddr(item->text(0));
    }
}

void ContractWidget::CopyAddr()
{
    if(QTreeWidgetItem *item = ui->treeWidget->currentItem())
    {
        QApplication::clipboard()->setText(item->data(0,Qt::UserRole).value<DataManagerStruct::ContractInfoPtr>()->GetContractAddr());
    }
}

void ContractWidget::InitWidget()
{
    //初始化右键菜单
    ui->treeWidget->installEventFilter(this);
    InitContextMenu();

    ui->treeWidget->header()->setVisible(false);
    ui->treeWidget->header()->setStretchLastSection(true);
    ui->treeWidget->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    ui->splitter->setSizes(QList<int>()<<0.66*this->height()<<0.34*this->height());

    connect(ui->treeWidget,&QTreeWidget::itemClicked,this,&ContractWidget::ContractClicked);

    if(ChainIDE::getInstance()->getChainClass() == DataDefine::HX && (ChainIDE::getInstance()->getStartChainTypes() | DataDefine::NONE))
    {
        connect(DataManagerHX::getInstance(),&DataManagerHX::queryAccountFinish,DataManagerHX::getInstance(),&DataManagerHX::queryContract);
        connect(DataManagerHX::getInstance(),&DataManagerHX::queryContractFinish,this,&ContractWidget::InitTree);
    }
    else if(ChainIDE::getInstance()->getChainClass() == DataDefine::UB && (ChainIDE::getInstance()->getStartChainTypes() | DataDefine::NONE))
    {
        connect(DataManagerUB::getInstance(),&DataManagerUB::queryContractFinish,this,&ContractWidget::InitTree);
    }
}

void ContractWidget::InitTree()
{
    ui->treeWidget->clear();

    DataManagerStruct::AddressContractDataPtr data = nullptr;
    if(ChainIDE::getInstance()->getChainClass() == DataDefine::HX)
    {
        data = DataManagerHX::getInstance()->getContract();
    }
    else if(ChainIDE::getInstance()->getChainClass() == DataDefine::UB)
    {
        data = DataManagerUB::getInstance()->getContract();
    }
    if(!data) return ;
    for(auto it = data->getAllData().begin();it != data->getAllData().end();++it)
    {
        QTreeWidgetItem *item = new QTreeWidgetItem(QStringList()<<(*it)->GetOwnerAddr()<<tr("合约描述"));
        item->setFlags(Qt::ItemIsEnabled);
        item->setData(0,Qt::UserRole,QVariant::fromValue<DataManagerStruct::AddressContractPtr>(*it));
        item->setTextAlignment(0,Qt::AlignCenter);
        ui->treeWidget->addTopLevelItem(item);
        for(auto cont = (*it)->GetContracts().begin();cont != (*it)->GetContracts().end();++cont)
        {
            QTreeWidgetItem *childitem = new QTreeWidgetItem(QStringList()<<((*cont)->GetContractName().isEmpty()?(*cont)->GetContractAddr():(*cont)->GetContractName())<<(*cont)->GetContractDes());
            childitem->setData(0,Qt::UserRole,QVariant::fromValue<DataManagerStruct::ContractInfoPtr>(*cont));
            childitem->setToolTip(0,(*cont)->GetContractAddr());
            childitem->setTextAlignment(0,Qt::AlignCenter);
            item->addChild(childitem);
        }
    }
    ui->treeWidget->expandAll();
}

void ContractWidget::InitContextMenu()
{
    QAction *copyAction = new QAction(tr("复制地址"),this);
    connect(copyAction,&QAction::triggered,this,&ContractWidget::CopyAddr);
    _p->contextMenu->addAction(copyAction);
}

bool ContractWidget::eventFilter(QObject *watched, QEvent *event)
{
    if(watched == ui->treeWidget && event->type() == QEvent::ContextMenu)
    {
        if(QTreeWidgetItem *item = ui->treeWidget->currentItem())
        {
            if(item->parent() && ui->treeWidget->itemAt(ui->treeWidget->viewport()->mapFromGlobal(QCursor::pos())) == item)
            {
                _p->contextMenu->exec(QCursor::pos());
            }
        }
    }
    return QWidget::eventFilter(watched,event);

}

void ContractWidget::retranslator()
{
    ui->retranslateUi(this);
    ui->functionWidget->retranslator();
}
