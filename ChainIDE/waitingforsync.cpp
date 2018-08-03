#include "waitingforsync.h"
#include "ui_waitingforsync.h"

WaitingForSync::WaitingForSync(QWidget *parent) :
    MoveableDialog(parent),
    ui(new Ui::WaitingForSync)
{
    ui->setupUi(this);

    setWindowFlags(  this->windowFlags() | Qt::FramelessWindowHint);

//    setAutoFillBackground(true);
//    QPalette palette;
//    palette.setBrush(QPalette::Background, QBrush(QPixmap(":/pic/bg.png")));
//    setPalette(palette);

}

WaitingForSync::~WaitingForSync()
{
    delete ui;
}

