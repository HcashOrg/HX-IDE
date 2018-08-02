#include "outputwidget.h"
#include "ui_outputwidget.h"

#include <QDebug>
#include <QTabBar>

class OutputWidget::DataPrivate
{
public:
    DataPrivate()
    {

    }
    ~DataPrivate()
    {
    }

public:
};

OutputWidget::OutputWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::OutputWidget),
    _p(new DataPrivate())
{
    ui->setupUi(this);
    //隐藏左右控制按钮
    ui->tabWidget->tabBar()->setUsesScrollButtons(false);
    ui->tabWidget->setCurrentIndex(0);
}

OutputWidget::~OutputWidget()
{
    delete _p;
    delete ui;
}

void OutputWidget::clearCompileMessage()
{
    ui->compileText->clear();
}

void OutputWidget::clearOutputMessage()
{
    ui->outputText->clear();
}

void OutputWidget::receiveCompileMessage(const QString &text)
{
    ui->tabWidget->setCurrentIndex(0);

    ui->compileText->append(text);
}

void OutputWidget::receiveOutputMessage(const QString &text)
{
    ui->tabWidget->setCurrentIndex(1);

    ui->outputText->append(text);
}

void OutputWidget::retranslator()
{
    ui->retranslateUi(this);
}

