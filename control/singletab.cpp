#include "singletab.h"
#include "ui_singletab.h"

#include <QLayout>
#include <QDebug>
#include <QFileInfo>
#include <QPainter>

SingleTab::SingleTab(QString name, QString path, QWidget *parent) :
    QWidget(parent),
    fileName(name),
    filePath(path),
    saved(true),
    ui(new Ui::SingleTab)
{
    ui->setupUi(this);


//    QHBoxLayout* hbLayout = new QHBoxLayout;
//    hbLayout->addSpacing(5);
//    hbLayout->addWidget(ui->savedBtn);
//    hbLayout->addSpacing(5);
//    hbLayout->addWidget(ui->nameBtn);
//    hbLayout->addSpacing(5);
//    hbLayout->addWidget(ui->closeBtn);
//    hbLayout->addSpacing(5);
//    hbLayout->setMargin(0);
//    hbLayout->setSpacing(0);
//    this->setLayout(hbLayout);

    ui->savedBtn->setStyleSheet("QPushButton{background-image:url(:/pic2/saved.png);border-style: flat;}");
    ui->savedBtn->setFixedSize(13,13);
    ui->savedBtn->move(5,9);
    ui->nameBtn->setStyleSheet("QPushButton{background:transparent;border:none;}");
    ui->nameBtn->setText(fileName);
    ui->nameBtn->setToolTip(filePath);
    ui->nameBtn->setFixedHeight(30);
    ui->nameBtn->adjustSize();
    ui->nameBtn->setFixedWidth(ui->nameBtn->width());
    ui->nameBtn->move(23,0);
    ui->closeBtn->setFixedSize(14,14);
    ui->closeBtn->move(28 + ui->nameBtn->width(),8);
    setFixedSize(52 + ui->nameBtn->width(),30);

    this->setStyleSheet("SingleTab{background-color:rgb(235,235,235);border: 1px solid rgb(210,210,210);"
                        "border-left-color:rgb(235,235,235);"
                        "border-bottom-color:rgb(235,235,235);"
                        "border-top-left-radius:8px;"
                        "border-top-right-radius:8px;}");
}

SingleTab::~SingleTab()
{
    delete ui;
}

void SingleTab::setSelected(bool isSelected)
{

    if( isSelected)
    {

        this->setStyleSheet( "SingleTab{background-color:rgb(255,255,255);border: 1px solid rgb(210,210,210);"
                             "border-left-color:rgb(235,235,235);"
                             "border-bottom-color:rgb(235,235,235);"
                             "border-top-left-radius:8px;"
                             "border-top-right-radius:8px;}");
    }
    else
    {
        this->setStyleSheet("SingleTab{background-color:rgb(235,235,235);border: 1px solid rgb(210,210,210);"
                            "border-left-color:rgb(235,235,235);"
                            "border-bottom-color:rgb(235,235,235);"
                            "border-top-left-radius:8px;"
                            "border-top-right-radius:8px;}");
    }
}

void SingleTab::setSaved(bool hasSaved)
{
    saved = hasSaved;
    if( hasSaved)
    {
        ui->savedBtn->setStyleSheet("QPushButton{background-image:url(:/pic2/saved.png);border-style: flat;}");
    }
    else
    {
        ui->savedBtn->setStyleSheet("QPushButton{background-image:url(:/pic2/unsaved.png);border-style: flat;}");
    }
}

void SingleTab::modifyPath(QString newPath)
{
    filePath = newPath;

    QFileInfo info(newPath);
    fileName = info.fileName();
    ui->nameBtn->setText(fileName);
    ui->nameBtn->setToolTip(filePath);
    ui->nameBtn->setFixedWidth(ui->nameBtn->sizeHint().width());
    ui->closeBtn->move(28 + ui->nameBtn->width(),8);
    setFixedSize(52 + ui->nameBtn->width(),30);
}

void SingleTab::on_closeBtn_clicked()
{
    emit closeFile(filePath);
}

void SingleTab::on_nameBtn_clicked()
{
    emit tabSelected(filePath);
}

void SingleTab::on_savedBtn_clicked()
{
    on_nameBtn_clicked();
}

void SingleTab::paintEvent(QPaintEvent *)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void SingleTab::mousePressEvent(QMouseEvent *)
{
    on_nameBtn_clicked();
}
