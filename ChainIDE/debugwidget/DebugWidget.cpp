#include "DebugWidget.h"
#include "ui_DebugWidget.h"

#include <QDebug>
#include "DataTreeItemModel.h"
#include "DebugDataStruct.h"
class DebugWidget::DataPrivate
{
public:
    DataPrivate()
        :treeModel(new DataTreeItemModel())
    {

    }
    DataTreeItemModel *treeModel ;
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

void DebugWidget::ResetData(BaseItemDataPtr data)
{
    _p->treeModel->InitModelData(data);
}

void DebugWidget::ClearData()
{
    _p->treeModel->InitModelData(std::make_shared<BaseItemData>());
}

void DebugWidget::InitWidget()
{
    ui->treeView->setModel(_p->treeModel);
}
