#include "registercontractdialog.h"
#include "ui_registercontractdialog.h"

#include <QDebug>
#include <QFileInfo>
#include <QMessageBox>
#include <QDir>
#include <QPainter>
#include <QMouseEvent>
#include <QDesktopWidget>
#include <QCoreApplication>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QStyledItemDelegate>
#include <QTimer>

#include "ChainIDE.h"
#include "IDEUtil.h"
#include "DataDefine.h"
#include "DataManager.h"

#include "commondialog.h"
#include "ConvenientOp.h"

RegisterContractDialog::RegisterContractDialog(QWidget *parent) :
    MoveableDialog(parent),
    ui(new Ui::RegisterContractDialog)
{
    ui->setupUi(this);

    InitWidget();
}

RegisterContractDialog::~RegisterContractDialog()
{
    delete ui;
}

void RegisterContractDialog::jsonDataUpdated(const QString &id,const QString &data)
{
    if("register-register_contract" == id)
    {
        ConvenientOp::ShowSyncCommonDialog(data);
        close();
    }
}

void RegisterContractDialog::on_okBtn_clicked()
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

void RegisterContractDialog::on_cancelBtn_clicked()
{
    close();
}

void RegisterContractDialog::on_closeBtn_clicked()
{
    close();
}

void RegisterContractDialog::InitWidget()
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
    connect(ChainIDE::getInstance()->getDataManager(),&DataManager::queryAccountFinish,this,&RegisterContractDialog::InitAccountAddress);
    ChainIDE::getInstance()->getDataManager()->queryAccount();

}

void RegisterContractDialog::InitAccountAddress()
{
    DataDefine::AccountDataPtr data = ChainIDE::getInstance()->getDataManager()->getAccount();

    for(auto it = data->getAccount().begin();it != data->getAccount().end();++it)
    {
        ui->address->addItem((*it)->getAccountName());
    }

    connect(ChainIDE::getInstance(),&ChainIDE::jsonDataUpdated,this,&RegisterContractDialog::jsonDataUpdated);
}

