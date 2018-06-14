#include "accountlistdialog.h"
#include "ui_accountlistdialog.h"

#include "../hxchain.h"
#include "namedialog.h"
#include "importdialog.h"

#include <QDebug>
#include <QMessageBox>
#include <QLayout>
#include <QPainter>
#include <QMouseEvent>
#include <QDesktopWidget>
#include <QFileDialog>
#include <QTimer>

AccountListDialog::AccountListDialog(QWidget *parent) :
    QDialog(parent),
    mouse_press(false),
    currentItem(NULL),
    ui(new Ui::AccountListDialog)
{
    ui->setupUi(this);

    setWindowFlags(Qt::FramelessWindowHint);
    move( (QApplication::desktop()->width() - this->width())/2 , (QApplication::desktop()->height() - this->height())/2);


    connect( HXChain::getInstance(), SIGNAL(jsonDataUpdated(QString)), this, SLOT(jsonDataUpdated(QString)));

    ui->accountListTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->accountListTableWidget->horizontalHeader()->setVisible(true);
//    ui->accountListTableWidget->setSelectionMode(QAbstractItemView::NoSelection);
//    ui->accountListTableWidget->setFocusPolicy(Qt::NoFocus);
    ui->accountListTableWidget->setColumnWidth(0,100);
    ui->accountListTableWidget->setColumnWidth(1,305);
    ui->accountListTableWidget->setColumnWidth(2,150);
    ui->accountListTableWidget->setColumnWidth(3,100);
    ui->accountListTableWidget->setColumnWidth(4,100);

    currentSelectedAccount = HXChain::getInstance()->currentAccount;
    updateAccountList();

    QTimer::singleShot(100,this,SLOT(scrollToCurrentAccount()));  // show 之后 才能scrollto

    if( HXChain::getInstance()->isInSandBox)
    {
        ui->addAccountBtn->setEnabled(false);
        ui->importBtn->setEnabled(false);
        ui->tipLabel->show();
    }
    else
    {
        ui->tipLabel->hide();
    }

}

AccountListDialog::~AccountListDialog()
{
    delete ui;
}

void AccountListDialog::scrollToCurrentAccount()
{
    if( currentItem != NULL)
    {
        ui->accountListTableWidget->scrollToItem(currentItem);
    }
}

void AccountListDialog::updateAccountList()
{
    ui->accountListTableWidget->clearContents();
    QStringList accountList = HXChain::getInstance()->accountInfoMap.keys();
    QStringList sortedList  = accountList.filter(ui->accountLineEdit->text());
    int size = sortedList.size();
    ui->accountListTableWidget->setRowCount( size);

    for( int i = 0; i < size; i++)
    {
        QString name = sortedList.at(i);
        QTableWidgetItem* item0 = new QTableWidgetItem(name);
        item0->setTextAlignment(Qt::AlignCenter);
        QTableWidgetItem* item1 = new QTableWidgetItem(HXChain::getInstance()->accountInfoMap.value(name).address);
        item1->setTextAlignment(Qt::AlignCenter);
        QTableWidgetItem* item2 = new QTableWidgetItem(HXChain::getInstance()->accountInfoMap.value(name).balance);
        item2->setTextAlignment(Qt::AlignCenter);

        ui->accountListTableWidget->setItem( i, 0, item0);
        ui->accountListTableWidget->setItem( i, 1, item1);
        ui->accountListTableWidget->setItem( i, 2, item2);

        QLabel* label3 = new QLabel(this);
        label3->setAlignment(Qt::AlignCenter);
        if( name == currentSelectedAccount)
        {
            label3->setPixmap(QPixmap(":/pic2/radioChecked.png"));
            currentItem = item0;
        }
        else
        {
            label3->setPixmap(QPixmap(":/pic2/radioUnchecked.png"));
        }
        ui->accountListTableWidget->setCellWidget( i , 3, label3);

        QTableWidgetItem* item4 = new QTableWidgetItem(QString::fromLocal8Bit("导出私钥"));
        item4->setTextAlignment(Qt::AlignCenter);
        item4->setTextColor(QColor(68,217,199));
        item4->setFlags(Qt::NoItemFlags);

        ui->accountListTableWidget->setItem( i, 4, item4);

    }



}

void AccountListDialog::on_addAccountBtn_clicked()
{
    NameDialog nameDialog;
    QString name = nameDialog.pop();

    if( !name.isEmpty())
    {
        HXChain::getInstance()->postRPC(  "id_wallet_account_create_" + name,toJsonFormat( "wallet_account_create", QJsonArray() << name ));
    }
}

void AccountListDialog::jsonDataUpdated(QString id)
{
    if( id.startsWith("id_wallet_account_create_") )
    {
        QString result = HXChain::getInstance()->jsonDataValue(id);

        if(result.startsWith("\"result\":"))   // 创建账号成功
        {
            emit newAccount();
        }
        else
        {
            QMessageBox::critical(NULL, "Error", "Create account failed!", QMessageBox::Ok);
        }

        return;
    }

    if( id.startsWith("id_wallet_dump_account_private_key_") )
    {
        QString filePath = id.mid(35);
        QString result = HXChain::getInstance()->jsonDataValue(id);


        if( result.mid(0,8) == "\"error\":")
        {
            qDebug() << "wallet_dump_account_private_key " + filePath + " ERROR: " + result;
        }
        else
        {
            QString privateKey = result.mid(10,51);
            QByteArray ba = privateKey.toUtf8();

            QFile file( filePath);

            QString fName = file.fileName();
            fName.replace("/","\\");
            fName = fName.mid( fName.lastIndexOf("\\") + 1);

//            if( file.exists())
//            {
//                if( QMessageBox::Yes != QMessageBox::information(NULL, "", QString::fromLocal8Bit("文件已存在，是否覆盖？"), QMessageBox::Yes | QMessageBox::No))
//                {
//                    return;
//                }
//            }

            if( !file.open(QIODevice::WriteOnly))
            {
                qDebug() << "privatekey file open " + fName + " ERROR";

                QMessageBox::critical(NULL, "Error", "Open file " + filePath + " error : "  + file.errorString(), QMessageBox::Ok);

                return;
            }

            file.resize(0);
            QTextStream ts( &file);
            ts << ba.toBase64();
            file.close();

            QMessageBox::information(NULL, "", QString::fromLocal8Bit("导出成功!"), QMessageBox::Ok);

        }

        return;
    }
}

void AccountListDialog::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setPen(QPen(QColor(68,217,199),Qt::SolidLine));
    painter.setBrush(QBrush(QColor(68,217,199),Qt::SolidPattern));
    painter.drawRect(0,0,this->width(),40);
}

void AccountListDialog::mousePressEvent(QMouseEvent *event)
{

    if(event->button() == Qt::LeftButton)
     {
          mouse_press = true;
          //鼠标相对于窗体的位置（或者使用event->globalPos() - this->pos()）
          move_point = event->pos();
     }

}

void AccountListDialog::mouseMoveEvent(QMouseEvent *event)
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

void AccountListDialog::mouseReleaseEvent(QMouseEvent *)
{
    mouse_press = false;
}

void AccountListDialog::on_closeBtn_clicked()
{
    close();
}

void AccountListDialog::on_accountListTableWidget_cellClicked(int row, int column)
{
    if( column == 3)
    {
        QTableWidgetItem* item = ui->accountListTableWidget->item(row,0);

        currentSelectedAccount = item->text();
        updateAccountList();
    }
    else if( column == 4)
    {
        QTableWidgetItem* item = ui->accountListTableWidget->item(row,0);
        QString accountName = item->text();
        QString filePath = QFileDialog::getSaveFileName(this, QString::fromLocal8Bit("导出私钥"), "./" + accountName, "(*.gkey)");

        if( !filePath.isEmpty())
        {
            HXChain::getInstance()->postRPC(  "id_wallet_dump_account_private_key_" + filePath,toJsonFormat( "wallet_dump_account_private_key", QJsonArray() << accountName << "0" ));
        }

    }
}

void AccountListDialog::on_searchBtn_clicked()
{
    updateAccountList();
}

void AccountListDialog::on_okBtn_clicked()
{

    if( currentSelectedAccount == HXChain::getInstance()->currentAccount)
    {
        close();
        return;
    }

    HXChain::getInstance()->setCurrentAccount(currentSelectedAccount);

    emit currentAccountChanged(currentSelectedAccount);
    close();
}

void AccountListDialog::on_importBtn_clicked()
{
    ImportDialog importDialog;
    connect( &importDialog, SIGNAL(accountImported()), this, SIGNAL(newAccount()));
    importDialog.exec();
}

void AccountListDialog::on_cancelBtn_clicked()
{
    close();
}

void AccountListDialog::on_accountLineEdit_textChanged(const QString &arg1)
{
    on_searchBtn_clicked();
}
