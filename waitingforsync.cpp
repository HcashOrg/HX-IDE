#include "waitingforsync.h"
#include "ui_waitingforsync.h"

#include <QTimer>
#include <QDebug>
#include <QMovie>
#include <QDesktopServices>

WaitingForSync::WaitingForSync(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WaitingForSync)
{
    ui->setupUi(this);


    setWindowFlags(  this->windowFlags() & ~Qt::WindowMaximizeButtonHint);
    setFixedSize(960,580);


    setAutoFillBackground(true);
    QPalette palette;
    palette.setBrush(QPalette::Background, QBrush(QPixmap(":/pic/bg.png")));
    setPalette(palette);

    gif = new QMovie(":/pic/loading.gif");
    ui->gifLabel->setMovie(gif);
    gif->start();


}

WaitingForSync::~WaitingForSync()
{
    delete ui;
}

