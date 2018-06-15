#include "outputwidget.h"
#include "ui_outputwidget.h"

#include "hxchain.h"
#include <QDebug>
#include <QTextCodec>
#include <QDateTime>

OutputWidget::OutputWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::OutputWidget)
{
    ui->setupUi(this);

    connect(HXChain::getInstance(),SIGNAL(jsonDataUpdated(QString)),this,SLOT(jsonDataUpdated(QString)));

    QHBoxLayout* hbLayout = new QHBoxLayout;
    hbLayout->addWidget(ui->compileBtn);
    hbLayout->addWidget(ui->outputBtn);
    hbLayout->addStretch();
    hbLayout->setMargin(0);
    hbLayout->setSpacing(0);

    QVBoxLayout* vbLayout = new QVBoxLayout;
    vbLayout->addLayout(hbLayout);
    vbLayout->addWidget(ui->tabWidget);
    vbLayout->setMargin(0);
    vbLayout->setSpacing(0);
    this->setLayout( vbLayout);

    vbLayout2 = new QVBoxLayout;
    vbLayout2->addWidget(ui->testErrorBrowser);
    vbLayout2->addWidget(ui->formalErrorBrowser);
    vbLayout2->setMargin(0);
    ui->errorPage->setLayout( vbLayout2);

    vbLayout3 = new QVBoxLayout;
    vbLayout3->addWidget(ui->testOutputBrowser);
    vbLayout3->addWidget(ui->formalOutputBrowser);
    vbLayout3->setMargin(0);
    ui->outputPage->setLayout( vbLayout3);

    ui->tabWidget->tabBar()->hide();

    ui->formalErrorBrowser->hide();
    ui->formalOutputBrowser->hide();

    on_compileBtn_clicked();
}

OutputWidget::~OutputWidget()
{
    delete ui;
}

void OutputWidget::appendText(int widgetNum, QString text)
{
    if( widgetNum == 0)
    {
        ui->testErrorBrowser->append(text);
    }
    else if( widgetNum == 1)
    {
        ui->testOutputBrowser->append(text);
    }
}

void OutputWidget::changeToTest()
{
//    vbLayout2->replaceWidget(ui->formalErrorBrowser, ui->testErrorBrowser);
//    vbLayout3->replaceWidget(ui->formalOutputBrowser, ui->testOutputBrowser);

    ui->formalErrorBrowser->hide();
    ui->formalOutputBrowser->hide();
    ui->testErrorBrowser->show();
    ui->testOutputBrowser->show();
}

void OutputWidget::changeToFormal()
{
//    vbLayout2->replaceWidget(ui->testErrorBrowser, ui->formalErrorBrowser);
//    vbLayout3->replaceWidget(ui->testOutputBrowser, ui->formalOutputBrowser);

    ui->formalErrorBrowser->show();
    ui->formalOutputBrowser->show();
    ui->testErrorBrowser->hide();
    ui->testOutputBrowser->hide();
}

void OutputWidget::testHasOutputToRead()
{
    QTextCodec* gbkCodec = QTextCodec::codecForName("GBK");
    QString result = gbkCodec->toUnicode( HXChain::getInstance()->currentProcess()->readAllStandardOutput() );

    ui->testOutputBrowser->append(  result );
    if( HXChain::getInstance()->currenChain() == 1)
    {
        on_outputBtn_clicked();
        emit outputRead(result);
    }
}

void OutputWidget::formalHasOutputToRead()
{
    QTextCodec* gbkCodec = QTextCodec::codecForName("GBK");
    QString result = gbkCodec->toUnicode( HXChain::getInstance()->currentProcess()->readAllStandardOutput() );

    ui->formalOutputBrowser->append("<body><font color=#C800C8>" + result + "</font></body>");
    if( HXChain::getInstance()->currenChain() == 2)
    {
        on_outputBtn_clicked();
        emit outputRead(result);
    }
}

void OutputWidget::jsonDataUpdated(QString id)
{
    if( id.startsWith("id_compile_contract_") )
    {
        QString result = HXChain::getInstance()->jsonDataValue(id);

        QString path = id.mid(20);

//        ui->errorBrowser->append( "Compile " + path);
//        ui->errorBrowser->append(result);
//        ui->errorBrowser->append("");
        on_compileBtn_clicked();

        QTextBrowser* currentTextBrowser;
        if( HXChain::getInstance()->currenChain() == 1)
        {
            currentTextBrowser = ui->testErrorBrowser;
        }
        else
        {
            currentTextBrowser = ui->formalErrorBrowser;
        }


        QString timeStr = QDateTime::currentDateTime().toString("hh:mm:ss ");
        currentTextBrowser->append( "<body><font color=#0000AA>" + timeStr + "Compile " + path + "</font></body>");

        if( result.startsWith("\"error\":"))
        {
            int pos = result.indexOf("\"message\":\"") + 11;
            QString errorMessage = result.mid(pos, result.indexOf("\"", pos) - pos);
            currentTextBrowser->append( ">>>>>  " + errorMessage);

            pos = result.indexOf("\"format\":\"") + 10;
            QString format = result.mid(pos, result.indexOf("\"", pos) - pos);
            currentTextBrowser->append( ">>>>>  " + format);
            currentTextBrowser->append("");
        }
        else if( result.startsWith("\"result\":"))
        {
            currentTextBrowser->append( ">>>>>  " + result);
            currentTextBrowser->append("");
        }

        return;
    }

    if( id.startsWith("id_compile_script_") )
    {
        QString result = HXChain::getInstance()->jsonDataValue(id);

        QString path = id.mid(18);

        QTextBrowser* currentTextBrowser;
        if( HXChain::getInstance()->currenChain() == 1)
        {
            currentTextBrowser = ui->testErrorBrowser;
        }
        else
        {
            currentTextBrowser = ui->formalErrorBrowser;
        }

        QString timeStr = QDateTime::currentDateTime().toString("hh:mm:ss ");
        currentTextBrowser->append( "<body><font color=#0000AA>" + timeStr + "Compile " + path + "</font></body>");
        on_compileBtn_clicked();

        if( result.startsWith("\"error\":"))
        {
            int pos = result.indexOf("\"message\":\"") + 11;
            QString errorMessage = result.mid(pos, result.indexOf("\"", pos) - pos);
            currentTextBrowser->append( ">>>>>  " + errorMessage);

            pos = result.indexOf("\"format\":\"") + 10;
            QString format = result.mid(pos, result.indexOf("\"", pos) - pos);
            currentTextBrowser->append( ">>>>>  " + format);
            currentTextBrowser->append("");
        }
        else if( result.startsWith("\"result\":"))
        {
            currentTextBrowser->append( ">>>>>  " + result);
            currentTextBrowser->append("");
        }


        return;
    }

    if( id == "id_call_contract" )
    {
        QString result = HXChain::getInstance()->jsonDataValue(id);

        QTextBrowser* currentTextBrowser;
        if( HXChain::getInstance()->currenChain() == 1)
        {
            currentTextBrowser = ui->testErrorBrowser;
        }
        else
        {
            currentTextBrowser = ui->formalErrorBrowser;
        }

        currentTextBrowser->append(result);
        on_compileBtn_clicked();

    }
}

void OutputWidget::on_compileBtn_clicked()
{
    ui->tabWidget->setCurrentIndex(0);
    ui->compileBtn->setStyleSheet( QString::fromLocal8Bit(BTN_SELECTED_STYLESHEET) );
    ui->outputBtn->setStyleSheet( QString::fromLocal8Bit(BTN_UNSELECTED_STYLESHEET) );
}

void OutputWidget::on_outputBtn_clicked()
{
    ui->tabWidget->setCurrentIndex(1);
    ui->compileBtn->setStyleSheet( QString::fromLocal8Bit(BTN_UNSELECTED_STYLESHEET) );
    ui->outputBtn->setStyleSheet( QString::fromLocal8Bit(BTN_SELECTED_STYLESHEET));
}
