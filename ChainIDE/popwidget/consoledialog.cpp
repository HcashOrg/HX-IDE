#include "consoledialog.h"
#include "ui_consoledialog.h"

#include <QKeyEvent>
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
    if((ui->consoleLineEdit->text().startsWith("stop") && (ChainIDE::getInstance()->getChainClass() == DataDefine::UB))||
        ((ui->consoleLineEdit->text().startsWith("witness_node_stop")||ui->consoleLineEdit->text().startsWith("lock")) && (ChainIDE::getInstance()->getChainClass() == DataDefine::HX)))
    {
        ui->consoleBrowser->append(QStringLiteral("operate is forbidden!\n"));
        ui->consoleLineEdit->clear();
        return;
    }
    if( !ui->consoleLineEdit->text().simplified().isEmpty())
    {
        cmdVector.removeAll(ui->consoleLineEdit->text());
        cmdVector.append(ui->consoleLineEdit->text());
        cmdIndex = cmdVector.size();
    }

    QStringList paramaters = ui->consoleLineEdit->text().simplified().split(' ');
    QString command = paramaters.at(0);
    paramaters.removeFirst();    

    QJsonArray array;

    if(!paramaters.empty())
    {
        if(1 == paramaters.size() && paramaters.first().startsWith("[") && paramaters.first().endsWith("]"))
        {
            array = QJsonDocument::fromJson(paramaters.first().toLatin1()).array();
        }
        else
        {
            QString data = "[";
            foreach (QString param, paramaters)
            {
                data.append(param).append(",");
            }
            data.remove(data.length()-1,1);
            data.append("]");
            array = QJsonDocument::fromJson(data.toLatin1()).array();
        }
    }
    ChainIDE::getInstance()->postRPC( "console-" + ui->consoleLineEdit->text().simplified(), IDEUtil::toJsonFormat( command, array )/*str*/);

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
