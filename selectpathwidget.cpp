#include "selectpathwidget.h"
#include "ui_selectpathwidget.h"

#include <QPainter>

#include <QFileDialog>
#include <QDebug>
#include <QDesktopServices>

#include <QCoreApplication>

#include "ChainIDE.h"

SelectPathWidget::SelectPathWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SelectPathWidget)
{
    ui->setupUi(this);

    setWindowFlags(  this->windowFlags() & ~Qt::WindowMaximizeButtonHint);
    setFixedSize(960,580);

    setAutoFillBackground(true);
    QPalette palette;
    palette.setBrush(QPalette::Background, QBrush(QPixmap(":/pic/bg.png")));
    setPalette(palette);

    ui->pathLineEdit->setText(ChainIDE::getInstance()->getEnvAppDataPath());
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
    ChainIDE::getInstance()->setConfigAppDataPath(ui->pathLineEdit->text());

    emit enter();
    close();
}

