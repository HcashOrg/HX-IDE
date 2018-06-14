#include "interfacewidget.h"
#include "ui_interfacewidget.h"

#include "hxchain.h"
#include "control/contextmenu.h"
#include "dialog/binddialog.h"

#include <QLayout>
#include <QDebug>
#include <QTreeWidgetItem>
#include <QContextMenuEvent>
#include <QMessageBox>

InterfaceWidget::InterfaceWidget(QWidget *parent) :
    QWidget(parent),
    address(""),
    ui(new Ui::InterfaceWidget)
{
    ui->setupUi(this);

    connect(HXChain::getInstance(),SIGNAL(jsonDataUpdated(QString)),this,SLOT(jsonDataUpdated(QString)));

    QHBoxLayout* hbLayout = new QHBoxLayout;
    hbLayout->addWidget(ui->interfaceBtn);
    hbLayout->addWidget(ui->eventBtn);
    hbLayout->setMargin(0);
    hbLayout->setSpacing(0);

    QVBoxLayout* vbLayout = new QVBoxLayout;
    vbLayout->addWidget(ui->addressLabel);
    vbLayout->addLayout(hbLayout);
    vbLayout->addWidget(ui->tabWidget);
    vbLayout->setMargin(0);
    vbLayout->setSpacing(0);
    this->setLayout( vbLayout);

    QVBoxLayout* vbLayout2 = new QVBoxLayout;
    vbLayout2->addWidget(ui->interfaceTreeWidget);
    vbLayout2->setMargin(0);
    ui->interfaceTabWidget->setLayout( vbLayout2);

    QVBoxLayout* vbLayout3 = new QVBoxLayout;
    vbLayout3->addWidget(ui->eventTreeWidget);
    vbLayout3->setMargin(0);
    ui->eventTabWidget->setLayout( vbLayout3);

    ui->tabWidget->tabBar()->hide();
    ui->interfaceTreeWidget->setHeaderHidden(true);
    ui->eventTreeWidget->setHeaderHidden(true);
    ui->eventTreeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    on_interfaceBtn_clicked();
}

InterfaceWidget::~InterfaceWidget()
{
    delete ui;
}

void InterfaceWidget::setContract(QString contractAddress)
{
    address = contractAddress;

    if( !address.isEmpty())
    {
        ui->addressLabel->setText(address);
        HXChain::getInstance()->postRPC(  "id_get_contract_info_" + address, toJsonFormat("get_contract_info",
                                                      QJsonArray() << address));
    }
    else
    {
        ui->interfaceTreeWidget->clear();
        ui->eventTreeWidget->clear();
        ui->addressLabel->clear();
    }

}

void InterfaceWidget::setScript(QString scriptId)
{
    ui->addressLabel->setText( scriptId);
    ui->interfaceTreeWidget->clear();
    ui->eventTreeWidget->clear();
}

void InterfaceWidget::contractInfoUpdated(QString contractAddress)
{
    if( contractAddress != address )  return;

    ui->interfaceTreeWidget->clear();
    foreach (QString abi, HXChain::getInstance()->contractInfoMap.value(address).abiList)
    {
        QTreeWidgetItem* item = new QTreeWidgetItem(QStringList() << abi);
        ui->interfaceTreeWidget->addTopLevelItem(item);
    }

    ui->eventTreeWidget->clear();
    foreach (QString event, HXChain::getInstance()->contractInfoMap.value(address).eventList)
    {
        QTreeWidgetItem* item = new QTreeWidgetItem(QStringList() << event);
        ui->eventTreeWidget->addTopLevelItem(item);

        HXChain::getInstance()->postRPC(  "id_list_event_handler_" + address + "_" + event, toJsonFormat("list_event_handler",
                                                      QJsonArray() << address << event ));

    }
}

void InterfaceWidget::unbind()
{
    if( QMessageBox::Yes == QMessageBox::information(NULL, "", QString::fromLocal8Bit("确定解除绑定该脚本?"), QMessageBox::Yes | QMessageBox::No))
    {
        QTreeWidgetItem* currentItem =  ui->eventTreeWidget->currentItem();
        QTreeWidgetItem* eventItem = currentItem->parent();
        QStringList strList = currentItem->text(0).split("   ");
        QString scriptId;
        if( strList.size() > 1)    scriptId = strList.at(1);

        HXChain::getInstance()->postRPC(  "id_delete_event_handler_" + address, toJsonFormat("delete_event_handler",
                                                      QJsonArray() << address << eventItem->text(0) << scriptId));

    }

}

void InterfaceWidget::onEventBindTriggered()
{
    QTreeWidgetItem* currentItem =  ui->eventTreeWidget->currentItem();
    if( currentItem == NULL)    return;

    BindDialog bindDialog("",address,currentItem->text(0));
    bindDialog.exec();

    setContract(address);

}

void InterfaceWidget::jsonDataUpdated(QString id)
{
    if( id.startsWith( "id_list_event_handler_" + address) )
    {
        QString event = id.mid( QString("id_list_event_handler_" + address).count() + 1 );

        QString result = HXChain::getInstance()->jsonDataValue(id);

        if( result.startsWith("\"result\":["))
        {
            if( result == "\"result\":[]")  return;

            QString scripts = result.mid(10);
            scripts.remove("\"");
            scripts.remove("]");

            QStringList scriptList = scripts.split(",");

            QList<QTreeWidgetItem*> itemList = ui->eventTreeWidget->findItems(event,Qt::MatchExactly);
            if( itemList.count() > 0)
            {
                int index = 0;
                foreach (QString scriptId, scriptList)
                {
                    QTreeWidgetItem* item = new QTreeWidgetItem(QStringList() << QString::number(index) + "   " + scriptId);
                    itemList.at(0)->addChild(item);
                    index++;
                }

            }

        }

        return;
    }

    if( id.startsWith("id_delete_event_handler_"))
    {
        QString result = HXChain::getInstance()->jsonDataValue(id);
        setContract(address);

        return;
    }
}

void InterfaceWidget::on_interfaceBtn_clicked()
{
    ui->tabWidget->setCurrentIndex(0);
    ui->interfaceBtn->setStyleSheet( QString::fromLocal8Bit(BTN_SELECTED_STYLESHEET) );
    ui->eventBtn->setStyleSheet( QString::fromLocal8Bit(BTN_UNSELECTED_STYLESHEET) );
}

void InterfaceWidget::on_eventBtn_clicked()
{
    ui->tabWidget->setCurrentIndex(1);
    ui->interfaceBtn->setStyleSheet( QString::fromLocal8Bit(BTN_UNSELECTED_STYLESHEET) );
    ui->eventBtn->setStyleSheet( QString::fromLocal8Bit(BTN_SELECTED_STYLESHEET));
}

void InterfaceWidget::on_eventTreeWidget_customContextMenuRequested(const QPoint &pos)
{
    QTreeWidgetItem* currentItem =  ui->eventTreeWidget->itemAt(pos);

    if( currentItem == NULL)    return;

    if( currentItem->parent() !=  NULL)  // 如果parent不是NULL,则为某一事件下的子项
    {
        ContextMenu menu(ContextMenu::BindedScriptType);
        connect(&menu,SIGNAL(unbindTriggered()),this,SLOT(unbind()));
        menu.exec(QCursor::pos());
        return;
    }
    else        // 否则点击的是事件， 弹出事件绑定脚本的弹框
    {
        ContextMenu menu(ContextMenu::EventBindScriptType);
        connect(&menu,SIGNAL(eventBindTriggered()),this,SLOT(onEventBindTriggered()));
        menu.exec(QCursor::pos());
        return;
    }

}
