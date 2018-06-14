#include "selectpathwidget.h"
#include "ui_selectpathwidget.h"
#include "hxchain.h"
#include "rpcthread.h"
#include <QPainter>

#include <QFileDialog>
#include <QDebug>
#include <QDesktopServices>

SelectPathWidget::SelectPathWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SelectPathWidget)
{
    ui->setupUi(this);

    setWindowFlags(  this->windowFlags() & ~Qt::WindowMaximizeButtonHint);
    setFixedSize(960,580);

    setAutoFillBackground(true);
    QPalette palette;
    palette.setBrush(QPalette::Background, QBrush(QPixmap(":/pic2/bg.png")));
    setPalette(palette);

    ui->pathLineEdit->setText( HXChain::getInstance()->appDataPath);



//    getUpdateXml();
}

SelectPathWidget::~SelectPathWidget()
{qDebug() << "~~~~~SelectPathWidget";
    delete ui;
}

void SelectPathWidget::on_selectPathBtn_clicked()
{
    QString file = QFileDialog::getExistingDirectory(this,tr( "Select the path to store the blockchain"));
    if( !file.isEmpty())
    {
        file.replace("/","\\");
        ui->pathLineEdit->setText( file);
    }
}

void SelectPathWidget::on_okBtn_clicked()
{
    if( ui->pathLineEdit->text().isEmpty())  return;
    HXChain::getInstance()->configFile->setValue("/settings/hxchainPath", ui->pathLineEdit->text());

    HXChain::getInstance()->appDataPath = ui->pathLineEdit->text();
    emit enter();
    this->close();

//    HXChain::getInstance()->proc->start("wallet-c.exe",QStringList() << "--data-dir" << ui->pathLineEdit->text()
//                                       << "--rpcuser" << "a" << "--rpcpassword" << "b" << "--rpcport" << QString::number( RPC_PORT) << "--server");
//    if( HXChain::getInstance()->proc->waitForStarted())
//    {
//        mutexForConfigFile.lock();
//        HXChain::getInstance()->configFile->setValue("/settings/hxchainPath", ui->pathLineEdit->text());
//        mutexForConfigFile.unlock();
//        HXChain::getInstance()->appDataPath = ui->pathLineEdit->text();
//        emit enter();
//        this->close();
//    }
//    else
//    {
//        qDebug() << "laungh wallet-c.exe failed";
//    }
}

