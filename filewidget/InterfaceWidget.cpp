#include "InterfaceWidget.h"
#include "ui_InterfaceWidget.h"

class InterfaceWidget::DataPrivate
{
public:
    DataPrivate()
    {

    }
public:
    QString currentFilePath;
};

InterfaceWidget::InterfaceWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::InterfaceWidget),
    _p(new DataPrivate())
{
    ui->setupUi(this);
    InitWidget();
}

InterfaceWidget::~InterfaceWidget()
{
    delete _p;
    delete ui;
}

void InterfaceWidget::ShowInterface(const QString &filePath)
{
    _p->currentFilePath = filePath;
    InitData();
}

void InterfaceWidget::InitData()
{
    ui->treeWidget_event->clear();
    ui->treeWidget_function->clear();
}

void InterfaceWidget::InitWidget()
{
    ui->treeWidget_event->header()->setVisible(false);
    ui->treeWidget_function->header()->setVisible(false);
    ui->tabWidget->setCurrentIndex(0);
}
