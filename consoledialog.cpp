#include "consoledialog.h"
#include "ui_consoledialog.h"

#include <QKeyEvent>
#include <QDesktopWidget>
#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QPainter>

#include "ChainIDE.h"
#include "IDEUtil.h"


ConsoleDialog::ConsoleDialog(QWidget *parent) :
    QDialog(parent),
    cmdIndex(0),
    ui(new Ui::ConsoleDialog)
{
    ui->setupUi(this);

    setWindowFlags(Qt::FramelessWindowHint);

    ui->consoleLineEdit->installEventFilter(this);

    ui->consoleLineEdit->setFocus();

    connect( ChainIDE::getInstance(),&ChainIDE::jsonDataUpdated, this, &ConsoleDialog::jsonDataUpdated);
}

ConsoleDialog::~ConsoleDialog()
{    
    qDebug() << "ConsoleDialog delete";
    delete ui;    
}

void ConsoleDialog::pop()
{
    move( (QApplication::desktop()->width() - this->width())/2 , (QApplication::desktop()->height() - this->height())/2);
    exec();
}



bool ConsoleDialog::eventFilter(QObject *watched, QEvent *e)
{
    if(watched == ui->consoleLineEdit)
    {
        if(e->type() == QEvent::KeyPress)
        {
            QKeyEvent* event = static_cast<QKeyEvent*>(e);
            if( event->key() == Qt::Key_Up)
            {
                cmdIndex--;
                if( cmdIndex >= 0 && cmdIndex <= cmdVector.size() - 1)
                {
                    ui->consoleLineEdit->setText(cmdVector.at(cmdIndex));
                }

                if( cmdIndex < 0)
                {
                    cmdIndex = 0;
                }

            }
            else if( event->key() == Qt::Key_Down)
            {
                cmdIndex++;
                if( cmdIndex >= 0 && cmdIndex <= cmdVector.size() - 1)
                {
                    ui->consoleLineEdit->setText(cmdVector.at(cmdIndex));
                }

                if( cmdIndex > cmdVector.size() - 1)
                {
                    cmdIndex = cmdVector.size() - 1;
                }

            }
        }

    }

    return QWidget::eventFilter(watched,e);
}

void ConsoleDialog::on_closeBtn_clicked()
{
    close();
}


void ConsoleDialog::mousePressEvent(QMouseEvent *event)
{

    if(event->button() == Qt::LeftButton)
     {
          mouse_press = true;
          //鼠标相对于窗体的位置（或者使用event->globalPos() - this->pos()）
          move_point = event->pos();;
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

void ConsoleDialog::on_consoleLineEdit_returnPressed()
{
    

    if( !ui->consoleLineEdit->text().simplified().isEmpty())
    {
        cmdVector.removeAll(ui->consoleLineEdit->text());
        cmdVector.append(ui->consoleLineEdit->text());
        cmdIndex = cmdVector.size();
    }

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

    ChainIDE::getInstance()->postRPC( "console-" + str, IDEUtil::toJsonFormat( command, array ));

    ui->consoleLineEdit->clear();

    return;
}

void ConsoleDialog::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    painter.setPen(Qt::NoPen);
    painter.setBrush(ChainIDE::getInstance()->getCurrentTheme() == DataDefine::Black_Theme ?
                         DataDefine::BLACK_BACKGROUND : DataDefine::WHITE_BACKGROUND);
    painter.drawRect(rect());
    QDialog::paintEvent(event);
}

void ConsoleDialog::jsonDataUpdated(const QString &id,const QString &data)
{
    if( id.startsWith("console-"))
    {
        ui->consoleBrowser->append(">>>" + id.mid(8));
        ui->consoleBrowser->append( data);
        ui->consoleBrowser->append("\n");
        return;
    }
}

void ConsoleDialog::on_clearBtn_clicked()
{
    ui->consoleBrowser->clear();
}
