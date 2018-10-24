#include "StatusBar.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QTimer>
#include "ChainIDE.h"
#include "IDEUtil.h"

class StatusBar::DataPrivate
{
public:
    DataPrivate()
        :perLabel(new QLabel())
        ,norLabel(new QLabel())
        ,timer(new QTimer())
    {

    }
    ~DataPrivate()
    {

    }
public:
    QLabel *perLabel;
    QLabel *norLabel;
    QTimer *timer;
};

StatusBar::StatusBar(QWidget *parent)
    : QStatusBar(parent)
    ,_p(new DataPrivate())
{
    InitWidget();
}

StatusBar::~StatusBar()
{
    delete _p;
}

void StatusBar::startStatus()
{
    connect(_p->timer,&QTimer::timeout,this,&StatusBar::queryBlock);
    _p->timer->start(1000);
}

void StatusBar::setPermanentMessage(const QString &mes)
{
    _p->perLabel->setText(mes);
}

void StatusBar::setNormalMessage(const QString &mes)
{
    _p->norLabel->setText(mes);
}

void StatusBar::setTempMessage(const QString &mes)
{
    _p->norLabel->clear();
    showMessage(mes,5000);
}

void StatusBar::queryBlock()
{
    if(ChainIDE::getInstance()->getStartChainTypes() == DataDefine::NONE)
    {
        _p->timer->stop();
        return;
    }

    if(ChainIDE::getInstance()->getChainClass() == DataDefine::UB)
    {
        ChainIDE::getInstance()->postRPC( "status-ubinfo", IDEUtil::toJsonFormat( "getblockchaininfo", QJsonArray()));
    }
    else if(ChainIDE::getInstance()->getChainClass() == DataDefine::HX)
    {
        ChainIDE::getInstance()->postRPC( "status-hxinfo", IDEUtil::toJsonFormat( "info", QJsonArray()));
    }
    else if(ChainIDE::getInstance()->getChainClass() == DataDefine::CTC)
    {
        ChainIDE::getInstance()->postRPC( "status-ctcinfo", IDEUtil::toJsonFormat( "info", QJsonArray(),true));
    }
}

void StatusBar::jsonDataUpdated(const QString &id, const QString &data)
{
    if("status-ubinfo" == id)
    {
        QJsonDocument parse_doucment = QJsonDocument::fromJson(data.toUtf8());
        QJsonObject jsonObject = parse_doucment.object();
        QJsonObject object = jsonObject.value("result").toObject();
        QString blockcount = QString::number(object.value("blocks").toInt());
        QString age = object.value("initialblockdownload").toBool()?"false" : "true";
        setPermanentMessage(QStringLiteral("Height: ") + blockcount + QStringLiteral("  Ready: ") + age);
    }
    else if("status-hxinfo" == id)
    {
        QJsonDocument parse_doucment = QJsonDocument::fromJson(data.toUtf8());
        QJsonObject jsonObject = parse_doucment.object();
        QJsonObject object = jsonObject.value("result").toObject();
        QString blockcount = QString::number(object.value("head_block_num").toInt());
        QString age = object.value("head_block_age").toString();
        setPermanentMessage(QStringLiteral("Height: ") + blockcount + QStringLiteral("  Age: ") + age);
    }
    else if("status-ctcinfo" == id)
    {
        QJsonDocument parse_doucment = QJsonDocument::fromJson(data.toUtf8());
        QJsonObject jsonObject = parse_doucment.object();
        QJsonObject object = jsonObject.value("result").toObject();
        QString blockcount = QString::number(object.value("blockchain_head_block_num").toInt());
        QString age = QString::number(object.value("blockchain_head_block_age").toInt());
        setPermanentMessage(QStringLiteral("Height: ") + blockcount + QStringLiteral("  Age: ") + age);
    }
}

void StatusBar::InitWidget()
{
    addPermanentWidget(_p->perLabel);
    addWidget(_p->norLabel);
    connect(ChainIDE::getInstance(),&ChainIDE::jsonDataUpdated,this,&StatusBar::jsonDataUpdated);
}
