#include "DebugWidget.h"
#include "ui_DebugWidget.h"

class DebugWidget::DataPrivate
{
public:
    DataPrivate()
    {

    }
};

DebugWidget::DebugWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DebugWidget),
    _p(new DataPrivate())
{
    ui->setupUi(this);
    InitWidget();
}

DebugWidget::~DebugWidget()
{
    delete _p;
    _p = nullptr;
    delete ui;
}

void DebugWidget::InitWidget()
{
    ui->treeWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
//    ui->treeWidget->headerItem()->setTextAlignment(0, Qt::AlignCenter);
//    ui->treeWidget->headerItem()->setTextAlignment(1, Qt::AlignCenter);
//    ui->treeWidget->headerItem()->setTextAlignment(2, Qt::AlignCenter);
}
