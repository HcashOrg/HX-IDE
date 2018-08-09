#include "FileWidget.h"
#include "ui_FileWidget.h"

#include "DataDefine.h"

#include <QDir>
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

QString FileWidget::getCurrentFile() const
{
    return ui->fileTree->getCurrentFilePath();
}

void FileWidget::SelectFile(const QString &filePath)
{
    ui->fileTree->selectFile(filePath);
    ui->interWidget->ShowInterface(filePath);
}

void FileWidget::retranslator()
{
    ui->retranslateUi(this);
    ui->fileTree->retranslator();
    ui->interWidget->retranslator();
}

void FileWidget::InitWidget()
{
    ui->splitter->setSizes(QList<int>()<<0.66*this->height()<<0.34*this->height());

    connect(ui->fileTree,&FileView::fileClicked,this,&FileWidget::fileClicked);
    connect(ui->fileTree,&FileView::fileClicked,ui->interWidget,&InterfaceWidget::ShowInterface);

    connect(ui->fileTree,&FileView::compileFile,this,&FileWidget::compileFile);
    connect(ui->fileTree,&FileView::deleteFile,this,&FileWidget::deleteFile);
    connect(ui->fileTree,&FileView::importContract,this,&FileWidget::importContract);
    connect(ui->fileTree,&FileView::exportContract,this,&FileWidget::exportContract);
    connect(ui->fileTree,&FileView::newFile,this,&FileWidget::newFile);
}
