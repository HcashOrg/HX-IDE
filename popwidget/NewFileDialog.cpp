#include "NewFileDialog.h"
#include "ui_NewFileDialog.h"

#include <QFileInfo>
#include <QPainter>
#include <QDir>
#include <QDebug>
#include "IDEUtil.h"
#include "ChainIDE.h"
#include "DataDefine.h"

class NewFileDialog::DataPrivate
{
public:
    DataPrivate(const QString &topDir,const QStringList &docs)
        :rootDir(topDir)
        ,types(docs)
    {
        IDEUtil::GetAllFileFolder(topDir,dirList);
    }
public:
    QStringList dirList;
    QString rootDir;
    QStringList types;

    QString newFilePath;
};

NewFileDialog::NewFileDialog(const QString &topDir,const QStringList &docs,QWidget *parent) :
    MoveableDialog(parent),
    ui(new Ui::NewFileDialog),
    _p(new DataPrivate(topDir,docs))
{
    ui->setupUi(this);
    InitWidget();
}

NewFileDialog::~NewFileDialog()
{
    delete _p;
    delete ui;
}

const QString &NewFileDialog::getNewFilePath() const
{
    return _p->newFilePath;
}

void NewFileDialog::TextChanged(const QString &text)
{
    ValidFile();
}

void NewFileDialog::comboBoxTextChanged(const QString &text)
{
    if(!text.startsWith(_p->rootDir+"/"))
    {
        ui->comboBox->setCurrentText(ui->comboBox->itemText(ui->comboBox->currentIndex()));
    }
    ValidFile();
}

void NewFileDialog::ConfirmSlot()
{
    bool isValid = false;
     foreach(QString var,_p->types)
     {
         if(ui->lineEdit->text().endsWith(var))
         {
             isValid = true;
             break;
         }
     }
     if(!isValid)
     {
         ui->lineEdit->setText(ui->lineEdit->text()+_p->types.front());
     }
     _p->newFilePath = ui->comboBox->currentText() + "/"+ui->lineEdit->text();
     //判断目录是不是根目录，如果是，则新建一个同名文件夹
     QString curtex = ui->comboBox->currentText();
     if(curtex.endsWith("/") || curtex.endsWith("\\"))
     {
         curtex.remove(curtex.length()-1,1);
     }
     QFileInfo fi(curtex);
     if(fi.absoluteFilePath() == _p->rootDir)
     {
         _p->newFilePath = _p->rootDir + "/" + QFileInfo(_p->newFilePath).baseName() + "/" + ui->lineEdit->text();
     }


     IDEUtil::TemplateFile(_p->newFilePath);
     close();
}

void NewFileDialog::CancelSlot()
{
    _p->newFilePath.clear();
    close();
}

void NewFileDialog::InitWidget()
{
    setWindowFlags(windowFlags()| Qt::FramelessWindowHint);

    ui->okBtn->setEnabled(false);
    ui->label_tip->setVisible(false);
    ui->comboBox->addItems(_p->dirList);
    ui->comboBox->setEditable(true);
    if(ui->comboBox->count() > 0)
    {
        ui->comboBox->setCurrentIndex(0);
    }


    connect(ui->lineEdit,&QLineEdit::textChanged,this,&NewFileDialog::TextChanged);
    connect(ui->okBtn,&QToolButton::clicked,this,&NewFileDialog::ConfirmSlot);
    connect(ui->cancelBtn,&QToolButton::clicked,this,&NewFileDialog::CancelSlot);
    connect(ui->closeBtn,&QToolButton::clicked,this,&QDialog::close);
    connect(ui->comboBox,static_cast<void (QComboBox::*)(int)>(&QComboBox::activated),this,&NewFileDialog::ValidFile);
    connect(ui->comboBox,&QComboBox::editTextChanged,this,&NewFileDialog::comboBoxTextChanged);

}

void NewFileDialog::ValidFile()
{
    QRegExp regx("[\\\\/:*?\"<>|]");
    if(ui->lineEdit->text().indexOf(regx) >= 0)
    {
        //文件名不合法
        ui->label_tip->setText(tr("invalid filename!"));
        ui->label_tip->setVisible(true);
        ui->okBtn->setEnabled(false);
    }
    else if(IDEUtil::isFileExist(ui->lineEdit->text(),ui->comboBox->currentText()))
    {//文件已存在
        ui->label_tip->setText(tr("already exist!"));
        ui->label_tip->setVisible(true);
        ui->okBtn->setEnabled(false);
    }
    else
    {
        ui->label_tip->setVisible(false);
        ui->okBtn->setEnabled(true);
    }
    if(ui->lineEdit->text().isEmpty())
    {
        ui->okBtn->setEnabled(false);
        ui->label_tip->setVisible(false);
    }
}
