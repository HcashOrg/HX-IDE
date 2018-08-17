#include "RegisterContractDialogHX.h"
#include "ui_RegisterContractDialogHX.h"

#include <QDebug>
#include <QFileInfo>
#include <QMessageBox>
#include <QDir>
#include <QPainter>
#include <QMouseEvent>
#include <QCoreApplication>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QStyledItemDelegate>

#include "ChainIDE.h"
#include "IDEUtil.h"
#include "DataDefine.h"
#include "datamanager/DataManagerHX.h"

#include "popwidget/commondialog.h"
#include "ConvenientOp.h"

RegisterContractDialogHX::RegisterContractDialogHX(QWidget *parent) :
    MoveableDialog(parent),
    ui(new Ui::RegisterContractDialogHX)
{
    ui->setupUi(this);

    InitWidget();
}

RegisterContractDialogHX::~RegisterContractDialogHX()
{
    delete ui;
}

void RegisterContractDialogHX::jsonDataUpdated(const QString &id,const QString &data)
{
    if("register-register_contract" == id)
    {
        ConvenientOp::ShowSyncCommonDialog(data);
        close();
    }
}

void RegisterContractDialogHX::on_okBtn_clicked()
{
    //打开合约文件
    QString contract = ui->contractFile->currentData().toString();

    //获取注册地址
    QString registerName = ui->address->currentText();

    QString price = QString::number(ui->gasprice->value()/pow(10,5));
    ChainIDE::getInstance()->postRPC("register-register_contract",IDEUtil::toJsonFormat("register_contract",
                                     QJsonArray()<<registerName<<price<<ui->gaslimit->text()<<
                                     contract));

}

void RegisterContractDialogHX::on_cancelBtn_clicked()
{
    close();
}

void RegisterContractDialogHX::on_closeBtn_clicked()
{
    close();
}

void RegisterContractDialogHX::InitWidget()
{
    contractAddress = "";
    ui->gaslimit->setRange(0,999999);
    ui->gaslimit->setSingleStep(1);
    ui->gasprice->setRange(10,999999);

    QStyledItemDelegate* itemDelegate = new QStyledItemDelegate();
    ui->contractFile->setItemDelegate(itemDelegate);

    setWindowFlags(Qt::FramelessWindowHint);

    //初始化combobox,读取所有.gpc文件
    QStringList fileList;
    IDEUtil::GetAllFile(QCoreApplication::applicationDirPath()+QDir::separator()+DataDefine::CONTRACT_DIR,
                        fileList,QStringList()<<DataDefine::CONTRACT_SUFFIX);
    foreach (QString file, fileList) {
        ui->contractFile->addItem(file.mid(QString(QCoreApplication::applicationDirPath()+QDir::separator()+DataDefine::CONTRACT_DIR).length()),
                                  QVariant::fromValue(file));
    }

    //读取所有账户信息
    connect(DataManagerHX::getInstance(),&DataManagerHX::queryAccountFinish,this,&RegisterContractDialogHX::InitAccountAddress);
    DataManagerHX::getInstance()->queryAccount();

}

void RegisterContractDialogHX::InitAccountAddress()
{
    DataManagerStruct::AccountHX::AccountDataPtr data = DataManagerHX::getInstance()->getAccount();

    for(auto it = data->getAccount().begin();it != data->getAccount().end();++it)
    {
        ui->address->addItem((*it)->getAccountName());
    }

    connect(ChainIDE::getInstance(),&ChainIDE::jsonDataUpdated,this,&RegisterContractDialogHX::jsonDataUpdated);
}

