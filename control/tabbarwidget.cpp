#include "tabbarwidget.h"
#include "ui_tabbarwidget.h"
#include "singletab.h"

#include <QLayout>
#include <QDebug>

TabBarWidget::TabBarWidget(QWidget *parent) :
    QWidget(parent),
    currentPath(""),
    currentIndex(0),
    ui(new Ui::TabBarWidget)
{
    ui->setupUi(this);

    setFixedHeight(30);

//    QHBoxLayout* hbLayout = new QHBoxLayout;
//    hbLayout->addStretch();
//    hbLayout->addWidget(ui->leftBtn);
//    hbLayout->addWidget(ui->rightBtn);
//    hbLayout->setMargin(0);
//    hbLayout->setSpacing(0);
//    this->setLayout(hbLayout);


    ui->leftBtn->setFixedSize(12,20);
    ui->rightBtn->setFixedSize(12,20);
    ui->leftBtn->hide();
    ui->rightBtn->hide();

}

TabBarWidget::~TabBarWidget()
{
    delete ui;
}

QString TabBarWidget::getCurrentPath()
{
    return currentPath;
}

void TabBarWidget::setCurrentPath(QString path)
{
    if( path == currentPath)  return;
    currentPath = path;

    foreach (QString key, pathTabMap.keys())
    {
        if( key != currentPath)
        {
            pathTabMap.value(key)->setSelected(false);
        }
        else
        {
            pathTabMap.value(key)->setSelected(true);
        }
    }

    forceCurrentTabShow();
    //    emit tabSelected(path);
}

void TabBarWidget::onTextChanged()
{
    if( !pathTabMap.contains(currentPath))  return;
    pathTabMap.value(currentPath)->setSaved(false);
}

void TabBarWidget::onFileSaved(QString path)
{
    if( !pathTabMap.contains(path))  return;
    pathTabMap.value(path)->setSaved(true);
}

void TabBarWidget::addTab(QString name, QString path)
{
    SingleTab* tab = new SingleTab(name, path, this);
    connect(tab,SIGNAL(closeFile(QString)),this,SLOT(onCloseFile(QString)));
    connect(tab,SIGNAL(tabSelected(QString)),this,SLOT(onSelectTab(QString)));
    tab->setAttribute(Qt::WA_DeleteOnClose);
    tab->show();
    pathTabMap.insert(path,tab);
    tabVector.append(path);

//    QHBoxLayout* layout = (QHBoxLayout*)this->layout();
//    layout->insertWidget(layout->count() - 3, tab);

    adjustTabs();
    setCurrentPath(path);

}

void TabBarWidget::removeTab(QString path)
{
    if( !pathTabMap.contains(path))  return;

    SingleTab* tab = pathTabMap.value(path);
//    this->layout()->removeWidget(tab);
    pathTabMap.remove(path);
    tabVector.removeAll(path);
    tab->close();

    adjustTabs();
}

QString TabBarWidget::getNextTab(QString currentTab)
{
    if( !pathTabMap.contains(currentTab))  return "";
    int index = tabVector.indexOf(currentTab);

    if( index < tabVector.count() - 1)
    {
        // 如果当前tab 不是最后一个  返回下一个tab的path
        return tabVector.at(index + 1);
    }
    else if( index > 0)
    {
        // 如果当前tab是最后一个 且不是唯一一个  返回倒数第二个tab的path
        return tabVector.at(index - 1);
    }
    else
    {
        // 如果当前tab是唯一一个 返回空
        return "";
    }
}

void TabBarWidget::modifyTabPath(QString oldPath, QString newPath)
{
    if( oldPath == newPath) return;
    if( !pathTabMap.contains(oldPath))  return;

    SingleTab* tab = pathTabMap.value(oldPath);
    tab->modifyPath(newPath);

    pathTabMap.insert(newPath,tab);
    pathTabMap.remove(oldPath);

    tabVector.replace( tabVector.indexOf(oldPath), newPath);
    adjustTabs();

}


void TabBarWidget::onCloseFile(QString path)
{
    if( !pathTabMap.contains(path))  return;

    emit fileClosed(path);
}

void TabBarWidget::onSelectTab(QString path)
{
    if( !pathTabMap.contains(path))  return;

    emit tabSelected(path);

}

void TabBarWidget::resizeEvent(QResizeEvent *size)
{
    currentIndex = 0;

    ui->leftBtn->move(this->width() - 26,5);
    ui->rightBtn->move(this->width() - 14,5);
    adjustTabs();
}

void TabBarWidget::adjustTabs()
{
    if( tabVector.count() == 0)     return;
    int barWidth = this->width() - 30;
    int sumWidth = 0;

    ui->rightBtn->hide();
    for( int i = 0; i < tabVector.count(); i++)
    {
        SingleTab* tab = pathTabMap.value( tabVector.at(i) );
        if( i < currentIndex)       // currentindex 之前的就隐藏
        {
            tab->hide();
            continue;
        }

        tab->move(sumWidth,0);
        sumWidth += tab->width();

        if( sumWidth  > barWidth)
        {
            tab->hide();
            ui->rightBtn->show();
        }
        else
        {
            tab->show();
        }

    }


    if( currentIndex > 0)
    {
        ui->leftBtn->show();
    }
    else
    {
        ui->leftBtn->hide();
    }


}

void TabBarWidget::forceCurrentTabShow()
{
    while( !pathTabMap.value(currentPath)->isVisible())    // 如果当前窗口的tab没有显示 则左移或右移直到显示
    {
        int index = tabVector.indexOf(currentPath);
        if( index > currentIndex)   on_rightBtn_clicked();
        if( index < currentIndex)   on_leftBtn_clicked();
    }
}


void TabBarWidget::on_leftBtn_clicked()
{
    if( currentIndex < 1)  return;
    currentIndex--;
    adjustTabs();
}

void TabBarWidget::on_rightBtn_clicked()
{
    if( currentIndex >= tabVector.count())   return;
    currentIndex++;
    adjustTabs();
}
