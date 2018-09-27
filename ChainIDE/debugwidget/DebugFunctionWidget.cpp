#include "DebugFunctionWidget.h"
#include "ui_DebugFunctionWidget.h"

#include <QTreeWidgetItem>
#include <QHeaderView>

#include <QFileInfo>

DebugFunctionWidget::DebugFunctionWidget(const QString &file,DataDefine::ApiEventPtr apis,QWidget *parent) :
    MoveableDialog(parent),
    ui(new Ui::DebugFunctionWidget)
{
    ui->setupUi(this);
    InitWidget(file,apis);
}

DebugFunctionWidget::~DebugFunctionWidget()
{
    delete ui;
}

const QString &DebugFunctionWidget::SelectedApi() const
{
    return api;
}

const QStringList &DebugFunctionWidget::ApiParams() const
{
    return params;
}

void DebugFunctionWidget::OnOKClicked()
{
    if(!ui->function->currentData().value<QString>().isEmpty())
    {
        api = ui->function->currentText();
    }
    params = ui->param->text().simplified().split(" ");
    close();
}

void DebugFunctionWidget::InitWidget(const QString &file, DataDefine::ApiEventPtr apis)
{
    setWindowFlags(Qt::FramelessWindowHint);

    ui->file->setText(QFileInfo(file).fileName());

    QTreeWidget *tree = new QTreeWidget(this);
    tree->header()->setVisible(false);
    QTreeWidgetItem *item = new QTreeWidgetItem(QStringList()<<"api");
    item->setFlags(Qt::ItemIsEnabled);
    tree->addTopLevelItem(item);

    QTreeWidgetItem *item1 = new QTreeWidgetItem(QStringList()<<"offline-api");
    item1->setFlags(Qt::ItemIsEnabled);
    tree->addTopLevelItem(item1);

    for(auto it = apis->getAllApiName().begin();it != apis->getAllApiName().end();++it)
    {
        QTreeWidgetItem *itemChild = new QTreeWidgetItem(QStringList()<<*it);
        itemChild->setData(0,Qt::UserRole,QVariant::fromValue<QString>("api"));
        item->addChild(itemChild);
    }
    for(auto it = apis->getAllOfflineApiName().begin();it != apis->getAllOfflineApiName().end();++it)
    {
        QTreeWidgetItem *itemChild = new QTreeWidgetItem(QStringList()<<*it);
        itemChild->setData(0,Qt::UserRole,QVariant::fromValue<QString>("offline-api"));
        item1->addChild(itemChild);
    }

    tree->expandAll();
    ui->function->setModel(tree->model());
    ui->function->setView(tree);


    connect(ui->closeBtn,&QToolButton::clicked,this,&DebugFunctionWidget::close);
    connect(ui->cancelBtn,&QToolButton::clicked,this,&DebugFunctionWidget::close);
    connect(ui->okBtn,&QToolButton::clicked,this,&DebugFunctionWidget::OnOKClicked);
}
