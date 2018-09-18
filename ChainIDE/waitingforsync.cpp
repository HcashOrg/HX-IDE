#include "waitingforsync.h"
#include "ui_waitingforsync.h"

WaitingForSync::WaitingForSync(QWidget *parent) :
    MoveableDialog(parent),
    ui(new Ui::WaitingForSync)
{
    ui->setupUi(this);
    InitWidget();
}

WaitingForSync::~WaitingForSync()
{
    delete ui;
}

void WaitingForSync::ReceiveMessage(const QString &message)
{
    ui->waitSync->append(message);
}

void WaitingForSync::InitWidget()
{
    setWindowFlags(  this->windowFlags() | Qt::FramelessWindowHint);
    ui->title_Label->setVisible(false);
    ui->waitSync->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

