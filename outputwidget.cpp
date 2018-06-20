#include "outputwidget.h"
#include "ui_outputwidget.h"

#include "ChainIDE.h"
#include <QDebug>
#include <QTextCodec>
#include <QDateTime>
#include <QProcess>
#include <QTextEdit>

class OutputWidget::DataPrivate
{
public:
    DataPrivate()
        :testOut(new QTextEdit())
        ,testCompile(new QTextEdit())
        ,formalOut(new QTextEdit())
        ,formalCompile(new QTextEdit())
    {

    }
    ~DataPrivate()
    {
        delete testOut;
        delete testCompile;
        delete formalOut;
        delete formalCompile;
    }

public:
    QTextEdit *testOut;
    QTextEdit *testCompile;
    QTextEdit *formalOut;
    QTextEdit *formalCompile;
};

OutputWidget::OutputWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::OutputWidget),
    _p(new DataPrivate())
{
    ui->setupUi(this);

    ui->compileStack->addWidget(_p->testCompile);
    _p->testCompile->resize(ui->compileStack->size());
    ui->compileStack->addWidget(_p->formalCompile);
    ui->outputStack->addWidget(_p->testOut);
    ui->outputStack->addWidget(_p->formalOut);
    ui->compileStack->setCurrentWidget(_p->testCompile);
    ui->outputStack->setCurrentWidget(_p->testOut);
    ui->tabWidget->setCurrentIndex(0);
}

OutputWidget::~OutputWidget()
{
    delete _p;
    delete ui;
}

void OutputWidget::receiveCompileMessage(const QString &text, int chainType)
{
    ui->tabWidget->setCurrentIndex(0);
    QTextEdit *curBrowser = 1 == chainType ? _p->testCompile : _p->formalCompile;

    ui->compileStack->setCurrentWidget(curBrowser);
    curBrowser->append(text);
}

void OutputWidget::receiveOutputMessage(const QString &text, int chainType)
{
    ui->tabWidget->setCurrentIndex(1);
    QTextEdit *curBrowser = 1 == chainType ? _p->testOut : _p->formalOut;

    ui->outputStack->setCurrentWidget(curBrowser);
    curBrowser->append(text);
}
