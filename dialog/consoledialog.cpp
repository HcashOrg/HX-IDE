#include "consoledialog.h"
#include "ui_consoledialog.h"
#include "../hxchain.h"
#include "../rpcthread.h"

#include <QTextCodec>
#include <QPainter>
#include <QKeyEvent>
#include <QDebug>
#include <QMouseEvent>
#include <QDesktopWidget>
#include <QJsonDocument>

ConsoleDialog::ConsoleDialog(QWidget *parent) :
    QDialog(parent),
    mouse_press(false),
    ui(new Ui::ConsoleDialog)
{
    ui->setupUi(this);

//    setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint);
//    setWindowTitle(QString::fromLocal8Bit("控制台"));
    setWindowFlags(Qt::FramelessWindowHint);
    move( (QApplication::desktop()->width() - this->width())/2 , (QApplication::desktop()->height() - this->height())/2);

    connect( HXChain::getInstance(), SIGNAL(jsonDataUpdated(QString)), this, SLOT(jsonDataUpdated(QString)));

    ui->consoleLineEdit->setFocus();

    ui->checkBox->setStyleSheet("QCheckBox::indicator{ image:url(:/pic2/checkBox_unchecked.png); }"
                                    "QCheckBox::indicator:checked{ image:url(:/pic2/checkBox_checked.png); }");

    ui->checkBox->setChecked(true);
//    ui->checkBox->setEnabled(false);


}

ConsoleDialog::~ConsoleDialog()
{
    delete ui;
}

void ConsoleDialog::outputRead(QString output)
{
    ui->consoleBrowser->append(output);
    ui->consoleLineEdit->clear();
}

void ConsoleDialog::on_consoleLineEdit_returnPressed()
{

    if( ui->checkBox->isChecked())
    {
        QString str = ui->consoleLineEdit->text();
        str = str.simplified();
        QStringList paramaters = str.split(' ');
        QString command = paramaters.at(0);
        paramaters.removeFirst();

        QJsonArray array;
        foreach (QString param, paramaters)
        {
            if(param.startsWith("[") && param.endsWith("]"))
            {
                array << QJsonDocument::fromJson(param.toLatin1()).array();
            }
            else
            {
                array << param;
            }
        }

        HXChain::getInstance()->postRPC(  "console_" + str, toJsonFormat(command, array ));

        ui->consoleLineEdit->clear();
        return;
    }

    QString str = ui->consoleLineEdit->text() + '\n';
    HXChain::getInstance()->write(str);
    //ui->consoleBrowser->append(">>>" + str);
}

void ConsoleDialog::jsonDataUpdated(QString id)
{
    if( id.startsWith("console_"))
    {
        ui->consoleBrowser->append(">>>" + id.mid(8));
        ui->consoleBrowser->append( HXChain::getInstance()->jsonDataValue(id));
        ui->consoleBrowser->append("\n");
        return;
    }
}

void ConsoleDialog::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setPen(QPen(QColor(68,217,199),Qt::SolidLine));
    painter.setBrush(QBrush(QColor(68,217,199),Qt::SolidPattern));
    painter.drawRect(0,0,this->width(),40);
}

void ConsoleDialog::mousePressEvent(QMouseEvent *event)
{

    if(event->button() == Qt::LeftButton)
     {
          mouse_press = true;
          //鼠标相对于窗体的位置（或者使用event->globalPos() - this->pos()）
          move_point = event->pos();
     }

}

void ConsoleDialog::mouseMoveEvent(QMouseEvent *event)
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

void ConsoleDialog::mouseReleaseEvent(QMouseEvent *)
{
    mouse_press = false;
}

void ConsoleDialog::on_closeBtn_clicked()
{
    close();
}
