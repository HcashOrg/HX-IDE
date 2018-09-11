#include "RegisterContractDialogCTC.h"
#include "ui_RegisterContractDialogCTC.h"

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
#include "datamanager/DataManagerCTC.h"

#include "popwidget/commondialog.h"
#include "ConvenientOp.h"

RegisterContractDialogCTC::RegisterContractDialogCTC(QWidget *parent) :
    MoveableDialog(parent),
    ui(new Ui::RegisterContractDialogCTC)
{
    ui->setupUi(this);
    InitWidget();
}

RegisterContractDialogCTC::~RegisterContractDialogCTC()
{
    delete ui;
}

void RegisterContractDialogCTC::jsonDataUpdated(const QString &id, const QString &data)
{
    if("register-register_contract" == id)
    {
        ConvenientOp::ShowSyncCommonDialog(data);
        close();
    }
    else if("register-register_contract_test" == id)
    {
        //解析结果，填入测试费用
        ui->feelimit->setValue(parseTestRegisterFee(data));
        ui->feelimit->setToolTip(tr("建议手续费:%1").arg(ui->feelimit->text()));
    }
}

void RegisterContractDialogCTC::on_okBtn_clicked()
{
    //打开合约文件
    QString contract = ui->contractFile->currentData().toString();

    //获取注册地址
    QString registerName = ui->address->currentText();

    ChainIDE::getInstance()->postRPC("register-register_contract",IDEUtil::toJsonFormat("contract_register",
                                     QJsonArray()<<registerName<<contract<<"CTC"<<ui->feelimit->text()
                                     ,true));
}

void RegisterContractDialogCTC::on_cancelBtn_clicked()
{
    close();
}

void RegisterContractDialogCTC::on_closeBtn_clicked()
{
    close();
}

void RegisterContractDialogCTC::testRegister()
{
    //打开合约文件
    QString contract = ui->contractFile->currentData().toString();

    //获取注册地址
    QString registerName = ui->address->currentText();

    ChainIDE::getInstance()->postRPC("register-register_contract_test",IDEUtil::toJsonFormat("contract_register_testing",
                                     QJsonArray()<<registerName<<contract
                                     ,true));

}

void RegisterContractDialogCTC::InitWidget()
{
    ui->feelimit->setDecimals(8);
    ui->feelimit->setRange(0,999999999);

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

    connect(ui->contractFile,static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),this,&RegisterContractDialogCTC::testRegister);
    connect(ui->address,static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),this,&RegisterContractDialogCTC::testRegister);

    //读取所有账户信息
    connect(DataManagerCTC::getInstance(),&DataManagerCTC::queryAccountFinish,this,&RegisterContractDialogCTC::InitAccountAddress);
    DataManagerCTC::getInstance()->queryAccount();

}

void RegisterContractDialogCTC::InitAccountAddress()
{
    DataManagerStruct::AccountCTC::AccountDataPtr data = DataManagerCTC::getInstance()->getAccount();

    for(auto it = data->getAccount().begin();it != data->getAccount().end();++it)
    {
        ui->address->addItem((*it)->getAccountName());
    }

    connect(ChainIDE::getInstance(),&ChainIDE::jsonDataUpdated,this,&RegisterContractDialogCTC::jsonDataUpdated);

}

double RegisterContractDialogCTC::parseTestRegisterFee(const QString &data) const
{
    double resultVal = 0;
    QJsonParseError json_error;
    QJsonDocument parse_doucment = QJsonDocument::fromJson(data.toLatin1(),&json_error);
    if(json_error.error != QJsonParseError::NoError || !parse_doucment.isObject())
    {
        qDebug()<<json_error.errorString();
        return 0;
    }
    QJsonArray resultArray = parse_doucment.object().value("result").toArray();
    foreach (QJsonValue addr, resultArray) {
        if(addr.toObject().value("amount").isString())
        {
            resultVal += addr.toObject().value("amount").toString().toULongLong()/pow(10,8);
        }
        else if(addr.toObject().value("amount").isDouble())
        {
            resultVal += addr.toObject().value("amount").toDouble()/pow(10,8);
        }

    }
    return resultVal;
}
