#include "RegisterContractDialogHX.h"
#include "ui_RegisterContractDialogHX.h"

#include <math.h>
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
    else if("register-register_contract_test" == id)
    {
        double fee = parseTestRegisterFee(data);
        ui->gaslimit->setToolTip(tr("approximatefee:%1").arg(QString::number(fee,'f',5)));
        ui->gasprice->setToolTip(tr("approximatefee:%1").arg(QString::number(fee,'f',5)));
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

void RegisterContractDialogHX::testRegister()
{
    //打开合约文件
    QString contract = ui->contractFile->currentData().toString();

    //获取注册地址
    QString registerName = ui->address->currentText();

    ChainIDE::getInstance()->postRPC("register-register_contract_test",IDEUtil::toJsonFormat("register_contract_testing",
                                     QJsonArray()<<registerName<<contract));
}

void RegisterContractDialogHX::InitWidget()
{
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

    connect(ui->contractFile,static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),this,&RegisterContractDialogHX::testRegister);
    connect(ui->address,static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),this,&RegisterContractDialogHX::testRegister);

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

double RegisterContractDialogHX::parseTestRegisterFee(const QString &data) const
{
    double resultVal = 0;
    QJsonParseError json_error;
    QJsonDocument parse_doucment = QJsonDocument::fromJson(data.toUtf8(),&json_error);
    if(json_error.error != QJsonParseError::NoError || !parse_doucment.isObject())
    {
        qDebug()<<json_error.errorString();
        return 0;
    }
    QJsonArray resultArray = parse_doucment.object().value("result").toArray();
    foreach (QJsonValue addr, resultArray) {
        if(addr.isObject())
        {
            if(addr.toObject().value("amount").isString())
            {
                resultVal += addr.toObject().value("amount").toString().toULongLong()/pow(10,5);
            }
            else if(addr.toObject().value("amount").isDouble())
            {
                resultVal += addr.toObject().value("amount").toDouble()/pow(10,5);
            }
        }
        else if(addr.isDouble())
        {
            ui->gaslimit->setValue(addr.toInt());
        }
    }
    return resultVal;
}

