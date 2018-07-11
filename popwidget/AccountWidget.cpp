#include "AccountWidget.h"
#include "ui_AccountWidget.h"

#include <QMenu>
#include <QClipboard>
#include "ChainIDE.h"
#include "DataManager.h"
#include "namedialog.h"
#include "importdialog.h"
#include "IDEUtil.h"

Q_DECLARE_METATYPE(DataDefine::AccountInfoPtr)
class AccountWidget::DataPrivate
{
public:
    DataPrivate()
        :contextMenu(new QMenu())
    {

    }
public:
    QMenu *contextMenu;//右键菜单
    QList<QTreeWidgetItem*> findItems;
};

AccountWidget::AccountWidget(QWidget *parent) :
    MoveableDialog(parent),
    ui(new Ui::AccountWidget),
    _p(new DataPrivate())
{
    ui->setupUi(this);
    InitWidget();
}

AccountWidget::~AccountWidget()
{
    delete _p;
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
<<<<<<< HEAD
        ChainIDE::getInstance()->postRPC("createnewaccount",IDEUtil::toJsonFormat("wallet_create_account",QJsonArray()<<name));
=======
        ChainIDE::getInstance()->postRPC("createnewaccount",IDEUtil::toJsonFormat("getnewaddress",QJsonArray()<<name));
>>>>>>> b41c4ef8ab0b45651d7ad4ccd9b34bd3691512f8
    }
}

void AccountWidget::on_importKey_clicked()
{
    ImportDialog dia;
    dia.exec();
}

void AccountWidget::on_closeBtn_clicked()
{
    close();
}

void AccountWidget::CopyAddr()
{
    if(QTreeWidgetItem *item = ui->treeWidget->currentItem())
    {
        QApplication::clipboard()->setText(item->data(0,Qt::UserRole).value<DataDefine::AccountInfoPtr>()->GetAccountAddress());
    }
}

bool AccountWidget::eventFilter(QObject *o, QEvent *e)
{
    if(o == ui->treeWidget && e->type() == QEvent::ContextMenu)
    {
        if(QTreeWidgetItem *item = ui->treeWidget->currentItem())
        {
            if(!item->parent() && ui->treeWidget->currentColumn() == 0 && ui->treeWidget->itemAt(ui->treeWidget->viewport()->mapFromGlobal(QCursor::pos())) == item)
            {
                _p->contextMenu->exec(QCursor::pos());
            }
        }
    }
    return QWidget::eventFilter(o,e);
}

void AccountWidget::InitWidget()
{
    //
    setWindowFlags(Qt::FramelessWindowHint | windowFlags());
    //初始化右键菜单
    InitContextMenu();

    ui->treeWidget->installEventFilter(this);

    //查找所有账户信息
    connect(ChainIDE::getInstance(),&ChainIDE::jsonDataUpdated,this,&AccountWidget::jsonDataUpdated);
    connect(ui->lineEdit,&QLineEdit::textEdited,this, &AccountWidget::QuerySlots);
    connect(ui->lineEdit,&QLineEdit::returnPressed,this,&AccountWidget::QuerySlots);

    connect(ChainIDE::getInstance()->getDataManager(),&DataManager::queryAccountFinish,this,&AccountWidget::InitTree);
    ChainIDE::getInstance()->getDataManager()->queryAccount();
}

void AccountWidget::InitTree()
{
    ui->treeWidget->clear();
    DataDefine::AccountDataPtr data = ChainIDE::getInstance()->getDataManager()->getAccount();
    ui->treeWidget->setColumnCount(3);
    for(auto it = data->getAccount().begin();it != data->getAccount().end();++it)
    {
        QString name = (*it)->getAccountName()+"("+ (*it)->GetAccountAddress()+")";
        QTreeWidgetItem *item = new QTreeWidgetItem(QStringList()<<name<<"");
        item->setData(0,Qt::UserRole,QVariant::fromValue<DataDefine::AccountInfoPtr>(*it));
        item->setTextAlignment(0,Qt::AlignCenter);
        item->setTextAlignment(1,Qt::AlignCenter);
        item->setTextAlignment(2,Qt::AlignCenter);
        ui->treeWidget->addTopLevelItem(item);

        for(auto add = (*it)->getAssetInfos().begin();add != (*it)->getAssetInfos().end();++add)
        {
            QTreeWidgetItem *childitem = new QTreeWidgetItem(QStringList()<<(*add)->GetAssetType()<<QString::number((*add)->GetBalance()/pow(10,(*add)->GetPrecision()),'f',(*add)->GetPrecision())<<tr("转账"));
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

void AccountWidget::InitContextMenu()
{
    QAction *copyAction = new QAction(tr("复制地址"),this);
    connect(copyAction,&QAction::triggered,this,&AccountWidget::CopyAddr);
    _p->contextMenu->addAction(copyAction);
}

void AccountWidget::QuerySlots()
{
    QString queryString = ui->lineEdit->text();
    if(queryString.isEmpty())
    {
        ui->treeWidget->clearSelection();
        _p->findItems.clear();
        return;
    }

    QList<QTreeWidgetItem*> findItems;
    QTreeWidgetItemIterator it(ui->treeWidget);
    while(*it)
    {
        if(-1 != (*it)->text(0).indexOf(queryString,0,Qt::CaseInsensitive) && !(*it)->parent())
        {
            findItems.push_back(*it);
        }
        ++it;
    }
    if(findItems.empty())
    {
        ui->treeWidget->clearSelection();
        _p->findItems.clear();
        return;
    }

    if(!isEqualList(findItems,_p->findItems))
    {//说明不是上次搜索的结果，取第一个
        ui->treeWidget->clearSelection();
        _p->findItems = findItems;
        ui->treeWidget->setCurrentItem(_p->findItems.front());
    }
    else
    {//说明是上次搜索的结果
        selectNext(_p->findItems);
    }
}

void AccountWidget::selectNext(const QList<QTreeWidgetItem *> &listItem)
{
    if(listItem.empty()) return;
    int index = listItem.indexOf(ui->treeWidget->currentItem());
    if(-1 == index || listItem.count() - 1 == index)
    {
        ui->treeWidget->clearSelection();
        ui->treeWidget->setCurrentItem(listItem.front());
    }
    else if(listItem.at(index + 1))
    {
        ui->treeWidget->clearSelection();
        ui->treeWidget->setCurrentItem(listItem.at(index+1));
    }
}

bool AccountWidget::isEqualList(const QList<QTreeWidgetItem *> &list1, const QList<QTreeWidgetItem *> &list2)
{
    if(list1.count() == list2.count())
    {
        foreach (QTreeWidgetItem *item, list1) {
            if(!list2.contains(item))
            {
                return false;
            }
        }
    }
    else
    {
        return false;
    }
    return true;
}
