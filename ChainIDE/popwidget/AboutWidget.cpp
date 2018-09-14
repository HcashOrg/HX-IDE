#include "AboutWidget.h"
#include "ui_AboutWidget.h"

#include <QPainter>
#include "update/UpdateProcess.h"

static const QString IDE_VERSION = "1.0.7";

class AboutWidget::DataPrivate
{
public:
    DataPrivate()
        :updateProcess(new UpdateProcess())
    {

    }

public:
    UpdateProcess *updateProcess;
};

AboutWidget::AboutWidget(QWidget *parent) :
    MoveableDialog(parent),
    ui(new Ui::AboutWidget),
    _p(new DataPrivate())
{
    ui->setupUi(this);
    InitWidget();
}

AboutWidget::~AboutWidget()
{
    delete _p;
    delete ui;
}

void AboutWidget::CheckUpdateSlot()
{
    emit UpdateNeeded(false);

    ui->label_updatetip->setText(tr("Checking updates..."));
    ui->label_updatetip->setVisible(true);
    showButtonState(-1);
    _p->updateProcess->checkUpdate();
}

void AboutWidget::CheckResultSlot(const QString &version)
{
    if(version.isEmpty())
    {
        //没有更新
        ui->label_updatetip->setText(tr("No new version!"));

        showButtonState(0);
    }
    else
    {
        ui->label_updatetip->setText(tr("New version found! " )+ version);

        showButtonState(1);
    }
}

void AboutWidget::UpdateSlot()
{
    ui->label_updatetip->setText(tr("Downloading... Please wait a moment! " ));

    showButtonState(-1);
    _p->updateProcess->startUpdate();
}

void AboutWidget::UpdateFinishSlot()
{
    ui->label_updatetip->setText(tr("Update finished. Restart and it will take effect! " ));
    showButtonState(2);
    emit UpdateNeeded(true);
}

void AboutWidget::UpdateWrongSlot()
{
    ui->label_updatetip->setText(tr("Update error! " ));
    showButtonState(0);

    emit UpdateNeeded(false);
}

void AboutWidget::InitWidget()
{
    setWindowFlags(windowFlags()| Qt::FramelessWindowHint);
    showButtonState(0);
    ui->label_updatetip->setVisible(false);

#ifdef WIN64
    ui->label_version->setText(QString("windows 64bit v") + IDE_VERSION);
#elif defined(TARGET_OS_MAC)
    ui->label_version->setText(QString("mac v") + IDE_VERSION);
#else
    ui->label_version->setText(QString("windows 32bit v") + IDE_VERSION);
#endif

    connect(_p->updateProcess,&UpdateProcess::updateFinish,this,&AboutWidget::UpdateFinishSlot);
    connect(_p->updateProcess,&UpdateProcess::updateWrong,this,&AboutWidget::UpdateWrongSlot);
    connect(_p->updateProcess,&UpdateProcess::NewstVersionSignal,this,&AboutWidget::CheckResultSlot);

    connect(ui->toolButton_checkUpdate,&QToolButton::clicked,this,&AboutWidget::CheckUpdateSlot);
    connect(ui->toolButton_update,&QToolButton::clicked,this,&AboutWidget::UpdateSlot);
    connect(ui->toolButton_restart,&QToolButton::clicked,this,&AboutWidget::close);
    connect(ui->toolButton_restart,&QToolButton::clicked,this,&AboutWidget::RestartSignal);
    connect(ui->closeBtn,&QToolButton::clicked,this,&AboutWidget::close);
}

void AboutWidget::showButtonState(int type)
{
    ui->toolButton_checkUpdate->setVisible(0 == type);
    ui->toolButton_update->setVisible(1 == type);
    ui->toolButton_restart->setVisible(2 == type);
}
