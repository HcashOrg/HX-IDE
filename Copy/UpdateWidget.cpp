#include "UpdateWidget.h"
#include "ui_UpdateWidget.h"

#include <QDir>
#include <QTimer>
#include <QString>
#include <QProcess>
#include <QDebug>

#include "DataUtil.h"

static const QString OKBTN_STYLE =
        "QToolButton{\
            border:1px solid gray;\
            border-radius:10px;\
            color:white;\
            background-color:rgb(66,66,66);\
            height:30px;\
            width:80px;\
        }" ;

static const QString TEMP_FOLDER_NAME = "updatetemp";
static const QString PACKAGE_UN = "ide";
static const QString PACKAGE_NAME = "ide.zip";
static const QString MAINEXE_NAME = "ChainIDE.exe";

UpdateWidget::UpdateWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UpdateWidget)
{
    ui->setupUi(this);
    InitWidget();
}

UpdateWidget::~UpdateWidget()
{
    delete ui;
}

void UpdateWidget::startMove()
{
    //解压文件
    QString tempdir = QCoreApplication::applicationDirPath()+QDir::separator()+TEMP_FOLDER_NAME;
    QString compressPath = tempdir +QDir::separator()+ PACKAGE_NAME;
    DataUtil::unCompress( compressPath,tempdir);
    //复制文件到temp目录
    DataUtil::copyDir(tempdir+QDir::separator()+PACKAGE_UN,QCoreApplication::applicationDirPath());
    //删除压缩文件、解压目录
    qDebug()<<compressPath;
    QFile::remove(compressPath);
    DataUtil::deleteDir(tempdir + QDir::separator() + PACKAGE_UN);

    //复制临时文件到主目录
    DataUtil::copyDir(QCoreApplication::applicationDirPath()+QDir::separator()+TEMP_FOLDER_NAME,QCoreApplication::applicationDirPath());
    //删除临时文件
    DataUtil::deleteDir(QCoreApplication::applicationDirPath()+QDir::separator()+TEMP_FOLDER_NAME);
    //更新结束
    copyFinish();
}

void UpdateWidget::copyFinish()
{
    ui->update->setText(tr("update finish!"));
    ui->toolButton_close->setVisible(true);
    ui->toolButton_restart->setVisible(true);

    //restartWallet();
}

void UpdateWidget::restartWallet()
{
    //启动外部复制程序
    QProcess *copproc = new QProcess();
    copproc->start(MAINEXE_NAME);
    close();
}

void UpdateWidget::InitWidget()
{
    InitStyle();
    ui->toolButton_close->setVisible(false);
    ui->toolButton_restart->setVisible(false);

    ui->update->setText(tr("updating,please wait!"));
    QTimer::singleShot(500,this,&UpdateWidget::startMove);

    connect(ui->toolButton_close,&QToolButton::clicked,this,&UpdateWidget::close);
    connect(ui->toolButton_restart,&QToolButton::clicked,this,&UpdateWidget::restartWallet);
}

void UpdateWidget::InitStyle()
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setAutoFillBackground(true);
    QPalette palette;
    palette.setBrush( QPalette::Window,
                     QBrush(QPixmap(":/resource/bg.png").scaled(// 缩放背景图.
                         this->size(),
                         Qt::IgnoreAspectRatio,
                         Qt::SmoothTransformation)));
    setPalette(palette);

    ui->toolButton_close->setStyleSheet(OKBTN_STYLE);
    ui->toolButton_restart->setStyleSheet(OKBTN_STYLE);
}
