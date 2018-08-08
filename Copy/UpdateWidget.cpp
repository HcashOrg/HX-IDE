#include "UpdateWidget.h"
#include "ui_UpdateWidget.h"

#include <QDir>
#include <QTimer>
#include <QString>
#include <QProcess>
#include <QDebug>
#include <QSettings>
#include <QTranslator>

#include "DataUtil.h"

class UpdateWidget::DataPrivate
{
public:
    DataPrivate(const QString &package,const QString &mainname,const QString &unpackagename,const QString &tempName)
        :tempDir(QCoreApplication::applicationDirPath()+QDir::separator()+tempName)
        ,packagePath(QCoreApplication::applicationDirPath()+QDir::separator()+tempName +QDir::separator()+ package)
        ,mainExePath(QCoreApplication::applicationDirPath()+QDir::separator()+mainname)
        ,unpackageDir(QCoreApplication::applicationDirPath()+QDir::separator()+tempName +QDir::separator()+ unpackagename)
    {

    }
public:
    QString tempDir;
    QString packagePath;
    QString unpackageDir;
    QString mainExePath;
};

UpdateWidget::UpdateWidget(const QString &packageName,const QString &mainName,const QString &unpackageName,const QString &tempName,QWidget *parent) :
    MoveableDialog(parent),
    ui(new Ui::UpdateWidget),
    _p(new DataPrivate(packageName,mainName,unpackageName,tempName))
{
    ui->setupUi(this);
    InitWidget();
}

UpdateWidget::~UpdateWidget()
{
    delete _p;
    delete ui;
}

void UpdateWidget::startMove()
{
    //解压文件到临时目录
    DataUtil::unCompress( _p->packagePath,_p->tempDir);

    //复制解压后的文件到主目录
    DataUtil::copyDir(_p->unpackageDir,QCoreApplication::applicationDirPath());

    //删除压缩文件、解压目录
    QFile::remove(_p->packagePath);
    DataUtil::deleteDir(_p->unpackageDir);

    //复制其他临时文件到主目录
    DataUtil::copyDir(_p->tempDir,QCoreApplication::applicationDirPath());
    //删除临时文件
    DataUtil::deleteDir(_p->tempDir);

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
    copproc->startDetached(_p->mainExePath);
    close();
}

void UpdateWidget::InitWidget()
{
    InitStyle();
    ui->toolButton_close->setVisible(false);
    ui->toolButton_restart->setVisible(false);

    ui->update->setText(tr("updating,please wait!"));
    QTimer::singleShot(1000,this,&UpdateWidget::startMove);

    connect(ui->toolButton_close,&QToolButton::clicked,this,&UpdateWidget::close);
    connect(ui->toolButton_restart,&QToolButton::clicked,this,&UpdateWidget::restartWallet);
}

void UpdateWidget::InitStyle()
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Widget);
    QSettings configFile(QCoreApplication::applicationDirPath() + QDir::separator() + "config.ini", QSettings::IniFormat);
    QString path ;
    if(configFile.value("/settings/theme") == "white")
    {
        path = ":/qss/white_style.qss";
    }
    else
    {
        path = ":/qss/black_style.qss";
    }

    QFile inputFile(path);
    inputFile.open(QIODevice::ReadOnly);
    QString css = inputFile.readAll();
    inputFile.close();
    this->setStyleSheet( css);

    QTranslator*  translator = new QTranslator();
    translator->load(configFile.value("/settings/language") == "SimChinese" ?  ":/Chinese.qm":":/English.qm" );
    QApplication::installTranslator(translator);
    ui->retranslateUi(this);
}
