#include "FileWidget.h"
#include "ui_FileWidget.h"

FileWidget::FileWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FileWidget)
{
    ui->setupUi(this);
    InitWidget();
}

FileWidget::~FileWidget()
{
    delete ui;
}

void FileWidget::SelectFile(const QString &filePath)
{
    ui->fileTree->selectFile(filePath);
    ui->interWidget->ShowInterface(filePath);
}

void FileWidget::InitWidget()
{
    ui->splitter->setSizes(QList<int>()<<0.66*this->height()<<0.34*this->height());

    connect(ui->fileTree,&FileView::fileClicked,this,&FileWidget::fileClicked);
    connect(ui->fileTree,&FileView::fileClicked,ui->interWidget,&InterfaceWidget::ShowInterface);


}
