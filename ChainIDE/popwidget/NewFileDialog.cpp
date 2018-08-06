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
    DataPrivate(const QString &topDir,const QStringList &docs,const QString &dirpath)
        :rootDir(topDir)
        ,types(docs)
        ,defaultPath(dirpath)
    {
        IDEUtil::GetAllFileFolder(topDir,dirList);
    }
public:
    QStringList dirList;
    QString rootDir;
    QStringList types;

    QString newFilePath;

    QString defaultPath;
};

NewFileDialog::NewFileDialog(const QString &topDir,const QStringList &docs,const QString &dirpath,QWidget *parent) :
    MoveableDialog(parent),
    ui(new Ui::NewFileDialog),
    _p(new DataPrivate(topDir,docs,dirpath))
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
         QString dirPath = IDEUtil::createDir(_p->rootDir + "/" + QFileInfo(_p->newFilePath).baseName());
         _p->newFilePath = dirPath + "/" + ui->lineEdit->text();
     }

     //模板化文件
     IDEUtil::TemplateFile(_p->newFilePath,ui->comboBox_template->currentData().toString());
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
    //设置文件夹
    ui->comboBox->addItems(_p->dirList);
    ui->comboBox->setEditable(true);
    if(ui->comboBox->count() > 0)
    {
        ui->comboBox->setCurrentIndex(0);
        if(!_p->defaultPath.isEmpty())
        {
            ui->comboBox->setCurrentText(_p->defaultPath);
        }
    }
    //设置模板组合
    ui->comboBox_template->addItem(tr("default"),QVariant::fromValue<QString>("initTemplate"));
    if(_p->types.contains("."+DataDefine::GLUA_SUFFIX))
    {
        ui->comboBox_template->addItem(tr("publishCoin"),QVariant::fromValue<QString>("newtoken"));
    }
    ui->comboBox_template->addItem(tr("none"),QVariant::fromValue<QString>("none"));


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
