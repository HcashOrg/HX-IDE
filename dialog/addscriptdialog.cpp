#include "addscriptdialog.h"
#include "ui_addscriptdialog.h"

#include "../hxchain.h"

#include <QFileInfo>
#include <QMessageBox>
#include <QDebug>
#include <QDir>
#include <QPainter>
#include <QMouseEvent>
#include <QDesktopWidget>

AddScriptDialog::AddScriptDialog(QString path, QWidget *parent) :
    QDialog(parent),
    scriptPath(path),
    mouse_press(false),
    ui(new Ui::AddScriptDialog)
{
    ui->setupUi(this);

//    setWindowFlags(  Qt::Dialog | Qt::WindowCloseButtonHint);
//    setWindowTitle(QString::fromLocal8Bit("添加脚本"));
    setWindowFlags(Qt::FramelessWindowHint);
    move( (QApplication::desktop()->width() - this->width())/2 , (QApplication::desktop()->height() - this->height())/2);

    connect(HXChain::getInstance(),SIGNAL(jsonDataUpdated(QString)),this,SLOT(jsonDataUpdated(QString)));

    QFileInfo info(scriptPath);
    ui->pathLabel->setText( info.fileName());

}

AddScriptDialog::~AddScriptDialog()
{
    delete ui;
}

void AddScriptDialog::on_okBtn_clicked()
{
    HXChain::getInstance()->postRPC(  "id_add_script_" + scriptPath, toJsonFormat("add_script", QJsonArray() << scriptPath << ui->descriptionLineEdit->text() ));
}

void AddScriptDialog::jsonDataUpdated(QString id)
{
    if( id == "id_add_script_" + scriptPath)
    {
        QString result = HXChain::getInstance()->jsonDataValue(id);

        if( result.startsWith("\"result\":"))
        {
            QString scriptId = result.mid(9);
            scriptId.remove('\"');

            // 如果脚本已经被添加过 则返回
            QStringList keys = HXChain::getInstance()->configGetAllScripts();
            foreach (QString path, keys)
            {
                if( scriptId == HXChain::getInstance()->configGetScriptId(path))
                {
                    QMessageBox::information(NULL, "", QString::fromLocal8Bit("脚本已存在!"), QMessageBox::Ok);
                    close();
                    return;
                }
            }


            // 已添加的脚本记录在config中  复制源码文件到 addedScripts文件夹下的 test或formal文件夹下 文件名后添加_add
            QString luaPath = scriptPath;
            luaPath = luaPath.left( luaPath.count() - 7);
            luaPath.append(".glua");

            // 如果有.glua文件 拷贝的是.glua文件 否则是.script文件
            QFileInfo luaInfo(luaPath);
            QString suffix = ".script";
            if( luaInfo.exists())
            {
                scriptPath = luaPath;
                suffix = ".glua";
            }

            QString addedPath;
            QDir dir("addedScripts");
            QFileInfo info(scriptPath);
            QString name = info.fileName();
            if( name.endsWith(".glua"))
            {
                name = name.left( name.count() - 5);
            }
            else if( name.endsWith(".script"))
            {
                name = name.left( name.count() - 7);
            }


            QString remark;
            if( HXChain::getInstance()->isInSandBox)
            {
                remark = "_sandbox";
            }
            else
            {
                remark = "";
            }

            if( HXChain::getInstance()->currenChain() == 1)
            {
                int num = 1;
                addedPath = dir.absolutePath() + "/test/" + name + "_add" + QString::number(num) + remark + suffix;
                QFileInfo info(addedPath);
                while ( info.exists())
                {
                    num++;
                    addedPath = dir.absolutePath() + "/test/" + name + "_add" + QString::number(num) + remark + suffix;
                    info.setFile(addedPath);
                }
            }
            else if( HXChain::getInstance()->currenChain() == 2)
            {
                int num = 1;
                addedPath = dir.absolutePath() + "/formal/" + name + "_add" + QString::number(num) + remark + suffix;
                QFileInfo info(addedPath);
                while ( info.exists())
                {
                    num++;
                    addedPath = dir.absolutePath() + "/formal/" + name + "_add" + QString::number(num) + remark + suffix;
                    info.setFile(addedPath);
                }
            }

            if( QFile::copy(scriptPath, addedPath) )
            {
                qDebug() << "added copy succeed: " << addedPath;
            }
            else
            {
                qDebug() << "added copy fail: " << addedPath;
            }

            HXChain::getInstance()->configSetScriptId(addedPath, scriptId);
            emit scriptAdded(addedPath);

            close();
        }
        else
        {
            if( result.mid(0,8) == "\"error\":")
            {
                int pos = result.indexOf("\"message\":\"") + 11;
                QString errorMessage = result.mid(pos, result.indexOf("\"", pos) - pos);

                QMessageBox::critical(NULL, "", "Add script fail : " + errorMessage, QMessageBox::Ok);
            }
        }


        return;
    }
}

void AddScriptDialog::on_cancelBtn_clicked()
{
    close();
}

void AddScriptDialog::on_closeBtn_clicked()
{
    close();
}

void AddScriptDialog::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setPen(QPen(QColor(68,217,199),Qt::SolidLine));
    painter.setBrush(QBrush(QColor(68,217,199),Qt::SolidPattern));
    painter.drawRect(0,0,this->width(),40);
}

void AddScriptDialog::mousePressEvent(QMouseEvent *event)
{

    if(event->button() == Qt::LeftButton)
     {
          mouse_press = true;
          //鼠标相对于窗体的位置（或者使用event->globalPos() - this->pos()）
          move_point = event->pos();
     }

}

void AddScriptDialog::mouseMoveEvent(QMouseEvent *event)
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

void AddScriptDialog::mouseReleaseEvent(QMouseEvent *)
{
    mouse_press = false;
}
