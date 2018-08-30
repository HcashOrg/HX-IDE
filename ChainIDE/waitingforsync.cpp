#include "waitingforsync.h"
#include "ui_waitingforsync.h"

WaitingForSync::WaitingForSync(QWidget *parent) :
    MoveableDialog(parent),
    ui(new Ui::WaitingForSync)
{
    ui->setupUi(this);

    setWindowFlags(  this->windowFlags() | Qt::FramelessWindowHint);

}

WaitingForSync::~WaitingForSync()
{
    delete ui;
}

