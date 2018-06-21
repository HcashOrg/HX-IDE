#include "NewFileDialog.h"
#include "ui_NewFileDialog.h"

#include <QFileInfo>
#include <QPainter>
#include <QDebug>
#include "IDEUtil.h"
#include "ChainIDE.h"

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
    QDialog(parent),
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

     qDebug()<<_p->newFilePath;
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

void NewFileDialog::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    painter.setPen(Qt::NoPen);
    painter.setBrush(ChainIDE::getInstance()->getCurrentTheme() == DataDefine::Black_Theme ?
                     DataDefine::BLACK_BACKGROUND : DataDefine::WHITE_BACKGROUND);
    painter.drawRect(rect());
    QDialog::paintEvent(event);
}

void NewFileDialog::mousePressEvent(QMouseEvent *event)
{

    if(event->button() == Qt::LeftButton)
     {
          mouse_press = true;
          //鼠标相对于窗体的位置（或者使用event->globalPos() - this->pos()）
          move_point = event->pos();;
     }
}

void NewFileDialog::mouseMoveEvent(QMouseEvent *event)
{
    //若鼠标左键被按下
    if(mouse_press)
    {
        //鼠标相对于屏幕的位置
        QPoint move_pos = event->globalPos();

        //移动主窗体位置
        this->move(move_pos - move_point);
    }
}

void NewFileDialog::mouseReleaseEvent(QMouseEvent *)
{
    mouse_press = false;
}
