#include "AccountWidget.h"
#include "ui_AccountWidget.h"

#include "ChainIDE.h"
#include "DataManager.h"
#include "namedialog.h"
#include "IDEUtil.h"

AccountWidget::AccountWidget(QWidget *parent) :
    MoveableDialog(parent),
    ui(new Ui::AccountWidget)
{
    ui->setupUi(this);
    InitWidget();
}

AccountWidget::~AccountWidget()
{
    delete ui;
}

void AccountWidget::jsonDataUpdated(const QString &id, const QString &data)
{
    if("createnewaccount" == id)
    {
        ChainIDE::getInstance()->getDataManager()->queryAccount();
    }
}

void AccountWidget::on_newAccount_clicked()
{
    NameDialog dia;
    QString name = dia.pop();
    if(!name.isEmpty())
    {
        ChainIDE::getInstance()->postRPC("createnewaccount",IDEUtil::toUbcdHttpJsonFormat("getnewaddress",QJsonArray()<<name));
    }
}

void AccountWidget::on_closeBtn_clicked()
{
    close();
}

void AccountWidget::InitWidget()
{
    //
    setWindowFlags(Qt::FramelessWindowHint | windowFlags());


    //查找所有账户信息
    connect(ChainIDE::getInstance(),&ChainIDE::jsonDataUpdated,this,&AccountWidget::jsonDataUpdated);

    connect(ChainIDE::getInstance()->getDataManager(),&DataManager::queryAccountFinish,this,&AccountWidget::InitTree);
    ChainIDE::getInstance()->getDataManager()->queryAccount();

    //InitTree();

}

void AccountWidget::InitTree()
{
    ui->treeWidget->clear();
    DataDefine::AccountDataPtr data = ChainIDE::getInstance()->getDataManager()->getAccount();
    ui->treeWidget->setColumnCount(3);
    for(auto it = data->getAccount().begin();it != data->getAccount().end();++it)
    {
        QTreeWidgetItem *item = new QTreeWidgetItem(QStringList()<<(*it)->getAccountName()<<QString::number((*it)->getTotalBalance(),'f',8)<<"");
        item->setTextAlignment(0,Qt::AlignCenter);
        item->setTextAlignment(1,Qt::AlignCenter);
        item->setTextAlignment(2,Qt::AlignCenter);
        ui->treeWidget->addTopLevelItem(item);

        for(auto add = (*it)->getAddressInfos().begin();add != (*it)->getAddressInfos().end();++add)
        {
            QTreeWidgetItem *childitem = new QTreeWidgetItem(QStringList()<<(*add)->GetAddress()<<QString::number((*add)->GetBalance(),'f',8)<<tr("转账"));
            childitem->setTextAlignment(0,Qt::AlignCenter);
            childitem->setTextAlignment(1,Qt::AlignCenter);
            childitem->setTextAlignment(2,Qt::AlignCenter);
            item->addChild(childitem);
        }
    }
    ui->treeWidget->setHeaderLabels(QStringList()<<tr("账户/地址")<<tr("余额")<<tr("操作"));
    ui->treeWidget->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->treeWidget->header()->setDefaultAlignment(Qt::AlignCenter);
    ui->treeWidget->expandAll();
}
