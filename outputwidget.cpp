#include "outputwidget.h"
#include "ui_outputwidget.h"

#include "ChainIDE.h"
#include <QDebug>
#include <QTextCodec>
#include <QDateTime>
#include <QProcess>
#include <QTextBrowser>
#include <QFile>

class OutputWidget::DataPrivate
{
public:
    DataPrivate()
        :testOut(new QTextBrowser())
        ,testCompile(new QTextBrowser())
        ,formalOut(new QTextBrowser())
        ,formalCompile(new QTextBrowser())
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
    QTextBrowser *testOut;
    QTextBrowser *testCompile;
    QTextBrowser *formalOut;
    QTextBrowser *formalCompile;
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
    refreshStyle();
}

OutputWidget::~OutputWidget()
{
    delete _p;
    delete ui;
}

void OutputWidget::receiveCompileMessage(const QString &text, int chainType)
{
    ui->tabWidget->setCurrentIndex(0);
    QTextBrowser *curBrowser = 1 == chainType ? _p->testCompile : _p->formalCompile;

    ui->compileStack->setCurrentWidget(curBrowser);
    curBrowser->append(text);
}

void OutputWidget::receiveOutputMessage(const QString &text, int chainType)
{
    ui->tabWidget->setCurrentIndex(1);
    QTextBrowser *curBrowser = 1 == chainType ? _p->testOut : _p->formalOut;

    ui->outputStack->setCurrentWidget(curBrowser);
    curBrowser->append(text);
}

void OutputWidget::retranslator()
{
    ui->retranslateUi(this);
}

void OutputWidget::refreshStyle()
{
    //不知道为啥，只有textbrowser的背景色样式表不起作用，需要单独设置，诡异的很
    QString background = ChainIDE::getInstance()->getCurrentTheme() == DataDefine::Black_Theme ?
                         "background-color:rgb(46,46,46);":"background-color:rgb(255,255,255);";
    _p->testCompile->setStyleSheet(background);
    _p->testOut->setStyleSheet(background);
    _p->formalCompile->setStyleSheet(background);
    _p->formalOut->setStyleSheet(background);

}
