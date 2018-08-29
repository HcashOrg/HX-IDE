#include "MessageNotifyWidget.h"
#include "ui_MessageNotifyWidget.h"

#include <QTimer>
#include <QApplication>
#include <QGuiApplication>
#include <QScreen>
#include <QDebug>
class MessageNotifyWidget::DataPrivate
{
public:
    DataPrivate()
        :showTimer(nullptr)
        ,stayTimer(nullptr)
        ,closeTimer(nullptr)
    {

    }
public:
    QTimer *showTimer;
    QTimer *stayTimer;
    QTimer *closeTimer;
};

MessageNotifyWidget::MessageNotifyWidget(const QString &info,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MessageNotifyWidget),
    _p(new DataPrivate())
{
    ui->setupUi(this);
    SetMessageContext(info);
    InitWdiget();
}

MessageNotifyWidget::~MessageNotifyWidget()
{
    delete _p;
    delete ui;
}

void MessageNotifyWidget::SetMessageContext(const QString &info)
{
    ui->textBrowser->setText(info);
}

void MessageNotifyWidget::OnShowSlots()
{
    if(this->geometry().bottomRight().y() < QGuiApplication::primaryScreen()->availableSize().height())
    {
        this->setWindowOpacity(1);
        _p->showTimer->stop();
        if(!this->rect().contains(this->mapFromGlobal(QCursor::pos())))
        {
            _p->stayTimer->start(2000);
        }
    }
    else
    {
        move(this->geometry().x(), this->geometry().topLeft().y()-1);
        this->setWindowOpacity(this->windowOpacity()+1.0/this->height());
        _p->showTimer->start(1000/this->height());
    }
}

void MessageNotifyWidget::OnStaySlots()
{
    _p->stayTimer->stop();
    _p->closeTimer->start(1000/this->height());
}

void MessageNotifyWidget::OnCloseSlots()
{
    if(this->windowOpacity() <= 0)
    {
        _p->closeTimer->stop();
        close();
    }
    else
    {
        setWindowOpacity(this->windowOpacity()-0.4/this->height());
    }
}

void MessageNotifyWidget::ShowWidget()
{
    QSize rect = QGuiApplication::primaryScreen()->availableSize();
    move(rect.width()-this->width(),rect.height());
    _p->showTimer->start(1);
    this->setWindowOpacity(0);
    show();
}

void MessageNotifyWidget::enterEvent(QEvent *event)
{
    _p->stayTimer->stop();
    _p->closeTimer->stop();
    this->setWindowOpacity(1);
}

void MessageNotifyWidget::leaveEvent(QEvent *event)
{
    _p->stayTimer->start(1500);
}

void MessageNotifyWidget::InitWdiget()
{
    setWindowFlags(Qt::SubWindow | Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint);
    ui->textBrowser->setContextMenuPolicy(Qt::NoContextMenu);

    _p->showTimer = new QTimer(this);
    _p->stayTimer = new QTimer(this);
    _p->closeTimer = new QTimer(this);
    connect(_p->showTimer,&QTimer::timeout,this,&MessageNotifyWidget::OnShowSlots);
    connect(_p->stayTimer,&QTimer::timeout,this,&MessageNotifyWidget::OnStaySlots);
    connect(_p->closeTimer,&QTimer::timeout,this,&MessageNotifyWidget::OnCloseSlots);
    connect(ui->closeBtn,&QToolButton::clicked,this,&MessageNotifyWidget::close);
}

