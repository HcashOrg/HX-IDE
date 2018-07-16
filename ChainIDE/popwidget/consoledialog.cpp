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
    MoveableDialog(parent),
    ui(new Ui::ConsoleDialog),
    cmdIndex(0)
{
    ui->setupUi(this);

    setWindowFlags(Qt::FramelessWindowHint | windowFlags());

    ui->consoleLineEdit->installEventFilter(this);

    ui->consoleLineEdit->setFocus();

    connect( ChainIDE::getInstance(),&ChainIDE::jsonDataUpdated, this, &ConsoleDialog::jsonDataUpdated);
}

ConsoleDialog::~ConsoleDialog()
{    
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
            array = QJsonDocument::fromJson(param.toLatin1()).array();
            break;
        }
        else
        {
            array << param;
        }
    }

    ChainIDE::getInstance()->postRPC( "console-" + str, IDEUtil::toJsonFormat( command, array )/*str*/);

    ui->consoleLineEdit->clear();

    return;
}

void ConsoleDialog::jsonDataUpdated(const QString &id,const QString &data)
{
    if( id.startsWith("console-"))
    {
        ui->consoleBrowser->append(">>>" + id.mid(8));
        QJsonParseError json_error;
        QJsonDocument parse_doucment = QJsonDocument::fromJson(data.toLatin1(),&json_error);
        if(json_error.error != QJsonParseError::NoError || !parse_doucment.isObject())
        {
            ui->consoleBrowser->append(json_error.errorString());
            ui->consoleBrowser->append("\n");
            return;
        }
        ui->consoleBrowser->append(parse_doucment.toJson());
        ui->consoleBrowser->append("\n");
        return;
    }
}

void ConsoleDialog::on_clearBtn_clicked()
{
    ui->consoleBrowser->clear();
}
