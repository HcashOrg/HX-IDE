#include "CallContractWidget.h"
#include "ui_CallContractWidget.h"

#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QCoreApplication>
#include <QDir>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>

#include "ChainIDE.h"
#include "DataManager.h"
#include "IDEUtil.h"
#include "ConvenientOp.h"

CallContractWidget::CallContractWidget(QWidget *parent) :
    MoveableDialog(parent),
    ui(new Ui::CallContractWidget)
{
    ui->setupUi(this);
    InitWidget();
}

CallContractWidget::~CallContractWidget()
{
    delete ui;
}

void CallContractWidget::jsonDataUpdated(const QString &id, const QString &data)
{
    if("call_callcontract" == id)
    {
        ConvenientOp::ShowSyncCommonDialog(data);
        close();
    }
    else if(id.startsWith("call-contractinfo_"))
    {
        QString ownerAddr = id.mid(QString("call-contractinfo_").length());
        parseContractInfo(ownerAddr,data);
    }
}

void CallContractWidget::CallContract()
{
    ChainIDE::getInstance()->postRPC("call_callcontract",IDEUtil::toUbcdHttpJsonFormat("callcontract",QJsonArray()<<
                                     ui->callAddress->currentText()<<ui->contractAddress->currentText()<<ui->function->currentText()
                                     <<ui->param->text()<<ui->gaslimit->value()<<ui->gasprice->value()<<ui->fee->value()));
}

void CallContractWidget::contractAddressChanged()
{
    //查询合约对应的api
    ChainIDE::getInstance()->postRPC("call-contractinfo_"+ui->contractAddress->currentText(),
                                     IDEUtil::toUbcdHttpJsonFormat("getcontractinfo",QJsonArray()<<ui->contractAddress->currentText()));
}

void CallContractWidget::InitWidget()
{
    ui->gaslimit->setRange(0,999999);
    ui->gaslimit->setSingleStep(1);
    ui->gasprice->setRange(10,999999);
    ui->fee->setRange(0,999999999999);
    ui->fee->setDecimals(8);
    ui->fee->setSingleStep(0.001);

    setWindowFlags(Qt::FramelessWindowHint);

    //ui->callAddress->setEditable(true);
    //ui->contractAddress->setEditable(true);

    //读取所有账户信息
    connect(ChainIDE::getInstance()->getDataManager(),&DataManager::queryAccountFinish,this,&CallContractWidget::InitAccountAddress);
    ChainIDE::getInstance()->getDataManager()->queryAccount();


    //初始化合约地址
    InitContractAddress();

    connect(ui->closeBtn,&QToolButton::clicked,this,&CallContractWidget::close);
    connect(ui->cancelBtn,&QToolButton::clicked,this,&CallContractWidget::close);
    connect(ui->okBtn,&QToolButton::clicked,this,&CallContractWidget::CallContract);

    connect(ChainIDE::getInstance(),&ChainIDE::jsonDataUpdated,this,&CallContractWidget::jsonDataUpdated);
}

void CallContractWidget::InitAccountAddress()
{
    QTreeWidget *tree = new QTreeWidget(this);
    tree->header()->setVisible(false);

    DataDefine::AccountDataPtr data = ChainIDE::getInstance()->getDataManager()->getAccount();
    int number = 0;
    QString currentText;
    for(auto it = data->getAccount().begin();it != data->getAccount().end();++it)
    {
        QTreeWidgetItem *item = new QTreeWidgetItem(QStringList()<<(*it)->getAccountName());
        item->setFlags(Qt::ItemIsEnabled);
        tree->addTopLevelItem(item);

        for(auto add = (*it)->getAddressInfos().begin();add != (*it)->getAddressInfos().end();++add)
        {
            QTreeWidgetItem *childitem = new QTreeWidgetItem(QStringList()<<(*add)->GetAddress());
            item->addChild(childitem);
            if(0 == number)
            {
                currentText = (*add)->GetAddress();
            }
            ++number;
        }
    }
    tree->expandAll();
    ui->callAddress->setModel(tree->model());
    ui->callAddress->setView(tree);
}

void CallContractWidget::InitContractAddress()
{

    DataDefine::AddressContractDataPtr data = std::make_shared<DataDefine::AddressContractData>();
    ConvenientOp::ReadContractFromFile(QCoreApplication::applicationDirPath()+QDir::separator()+DataDefine::LOCAL_CONTRACT_PATH,data);
    QTreeWidget *tree = new QTreeWidget(this);
    tree->header()->setVisible(false);
    for(auto it = data->getAllData().begin();it != data->getAllData().end();++it)
    {
        QTreeWidgetItem *item = new QTreeWidgetItem(QStringList()<<(*it)->GetOwnerAddr());
        item->setFlags(Qt::ItemIsEnabled);
        tree->addTopLevelItem(item);
        for(auto cont = (*it)->GetContracts().begin();cont != (*it)->GetContracts().end();++cont)
        {
            QTreeWidgetItem *childitem = new QTreeWidgetItem(QStringList()<<(*cont)->GetContractAddr());
            item->addChild(childitem);
        }

    }
    tree->expandAll();
    ui->contractAddress->setModel(tree->model());
    ui->contractAddress->setView(tree);

    connect(ui->contractAddress,static_cast<void (QComboBox::*)(const QString &)>(&QComboBox::activated),this,
            &CallContractWidget::contractAddressChanged);
}

bool CallContractWidget::parseContractInfo(const QString &addr, const QString &data)
{
    ui->function->clear();
    QTreeWidget *tree = new QTreeWidget(this);
    tree->header()->setVisible(false);
    QTreeWidgetItem *item = new QTreeWidgetItem(QStringList()<<"api");
    item->setFlags(Qt::ItemIsEnabled);
    tree->addTopLevelItem(item);

    QTreeWidgetItem *item1 = new QTreeWidgetItem(QStringList()<<"offline-api");
    item1->setFlags(Qt::ItemIsEnabled);
    tree->addTopLevelItem(item1);

    QJsonParseError json_error;
    QJsonDocument parse_doucment = QJsonDocument::fromJson(data.toLatin1(),&json_error);
    if(json_error.error != QJsonParseError::NoError || !parse_doucment.isObject())
    {
         qDebug()<<json_error.errorString();
         return false;
    }
    QJsonArray apisArr = parse_doucment.object().value("apis").toArray();
    foreach (QJsonValue val, apisArr) {
        if(!val.isObject()) continue;
        QTreeWidgetItem *itemChild = new QTreeWidgetItem(QStringList()<<val.toObject().value("name").toString());
        item->addChild(itemChild);
    }
    QJsonArray offapisArr = parse_doucment.object().value("offline_apis").toArray();
    foreach (QJsonValue val, offapisArr) {
        if(!val.isObject()) continue;
        QTreeWidgetItem *itemChild = new QTreeWidgetItem(QStringList()<<val.toObject().value("name").toString());
        item1->addChild(itemChild);
    }


    tree->expandAll();
    ui->function->setModel(tree->model());
    ui->function->setView(tree);
    return true;
}
