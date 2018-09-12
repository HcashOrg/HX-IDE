#include "selectpathwidget.h"
#include "ui_selectpathwidget.h"

#include <QFileDialog>

#include <QCoreApplication>

#include "ChainIDE.h"

SelectPathWidget::SelectPathWidget(QWidget *parent) :
    MoveableDialog(parent),
    ui(new Ui::SelectPathWidget)
{
    ui->setupUi(this);

    setWindowFlags(  this->windowFlags() | Qt::FramelessWindowHint);

    ui->pathLineEdit->setText(ChainIDE::getInstance()->getEnvAppDataPath());
    ui->tiplabel->setVisible(false);
}

SelectPathWidget::~SelectPathWidget()
{
    delete ui;
}

void SelectPathWidget::on_pathBtn_clicked()
{
    QString file = QFileDialog::getExistingDirectory(this,tr( "Select the path to store the blockchain"));
    if( !file.isEmpty())
    {
        file.replace("/","\\");
        ui->pathLineEdit->setText( file);
    }
    if(file.contains(" "))
    {
        ui->tiplabel->setText(tr("Folderpath cannot contain space character!"));
        ui->tiplabel->setVisible(true);
    }
    else
    {
        ui->tiplabel->setVisible(false);
    }
}

void SelectPathWidget::on_okBtn_clicked()
{
    if( ui->pathLineEdit->text().isEmpty() || ui->tiplabel->isVisible())  return;
    ChainIDE::getInstance()->setConfigAppDataPath(ui->pathLineEdit->text());

    emit enter();
    close();
}

void SelectPathWidget::on_closeBtn_clicked()
{
    emit cancel();
    close();
}

