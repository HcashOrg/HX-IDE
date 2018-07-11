#include "ContextWidget.h"
#include "ui_ContextWidget.h"

#include <QFileInfo>
#include <QMessageBox>
#include <QDebug>

#include "aceeditor.h"
#include "codeeditor.h"
#include "bridge.h"

static const QString EMPTYSTRING = "";
class ContextWidget::DataPrivate
{
public:
    DataPrivate()
    {

    }
public:

};

ContextWidget::ContextWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ContextWidget),
    _p(new DataPrivate())
{
    ui->setupUi(this);
    InitWidget();
}

ContextWidget::~ContextWidget()
{
    delete _p;
    delete ui;
}

void ContextWidget::undo()
{
    if(Editor *editor = getCurrentEditor())
    {
        editor->undo();
    }
}

void ContextWidget::redo()
{
    if(Editor *editor = getCurrentEditor())
    {
        editor->redo();
    }
}

bool ContextWidget::isUndoAvailable()
{
    if(Editor *editor = getCurrentEditor())
    {
        return editor->isUndoAvailable();
    }
    return false;
}

bool ContextWidget::isRedoAvailable()
{
    if(Editor *editor = getCurrentEditor())
    {
        return editor->isRedoAvailable();
    }
    return false;

}

bool ContextWidget::hasFileUnsaved()
{
    bool hasUnsaved = false;
    for(int i = 0;i < ui->tabWidget->count();++i)
    {
        if( Editor* w = getEditor(i))
        {
            if( !w->isSaved())
            {
                hasUnsaved = true;
                break;
            }
        }
    }

    return hasUnsaved;
}

bool ContextWidget::currentFileUnsaved()
{
    if(Editor *editor = getCurrentEditor())
    {
        return !editor->isSaved();
    }
    return false;
}

const QString &ContextWidget::getCurrentFilePath() const
{
    return getPathFromNumber(ui->tabWidget->currentIndex());
}

void ContextWidget::TabBreakPoint()
{
    if(Editor *editor = getCurrentEditor())
    {
        editor->TabBreakPoint();
    }
}

void ContextWidget::ClearBreakPoint()
{
    for(int i = 0;i < ui->tabWidget->count();++i)
    {
        if(Editor* w = dynamic_cast<Editor*>(ui->tabWidget->widget(i)))
        {
            w->ClearBreakPoint();
        }
    }
}

void ContextWidget::showFile(QString path)
{
    for(int i = 0;i < ui->tabWidget->count();++i)
    {
        if(Editor* w = dynamic_cast<Editor*>(ui->tabWidget->widget(i)))
        {
            if(w->getFilePath() == path)
            {
                ui->tabWidget->setCurrentIndex(i);
                return;
            }
        }

    }
    //如果没找到，新建一个
    Editor* w = new /*AceEditor*/codeeditor(path );


    ui->tabWidget->addTab(w,QIcon(":/pic/saved.png"),QFileInfo(path).fileName());

    ui->tabWidget->setCurrentWidget(w);
    connect(w,&Editor::stateChanged,this,&ContextWidget::contextUpdate);
}

bool ContextWidget::closeFile(QString path)
{
    closeFile(getTabNumber(path));
    return true;
}

bool ContextWidget::closeAll()
{
    for(int i = 0;i < ui->tabWidget->count();++i)
    {
        if(!closeFile(i))
        {
            return false;
        }
    }
    return true;
}

void ContextWidget::saveFile()
{
    saveFile(ui->tabWidget->currentIndex());
}

void ContextWidget::saveFile(QString path)
{
    int i = getTabNumber(path);
    if(-1 != i)
    {
        saveFile(i);
    }
}

void ContextWidget::saveAll()
{
    for(int i = 0;i < ui->tabWidget->count();++i)
    {
        saveFile(i);
    }
}

void ContextWidget::onTextChanged()
{
    qDebug() << "ContentWidget::onTextChanged" ;//<< isUndoAvailable();

    Editor *w = getCurrentEditor();
    if(!w) return;
    w->setSaved(false);
    w->checkState();
}

void ContextWidget::gutterRightClickedSignal()
{
    Editor *w = getCurrentEditor();
    if(!w) return;

}

void ContextWidget::CheckDeleteFile()
{
    //获取所有当前打开的文件
    for(int i = 0;i < ui->tabWidget->count();++i)
    {
        QString path = getPathFromNumber(i);
        if(!QFileInfo(path).exists())
        {
            ui->tabWidget->removeTab(i);
            contextUpdate();
            break;
        }
    }

}

void ContextWidget::currentTabChanged(int i)
{
    emit fileSelected(getPathFromNumber(i));
    contextUpdate();
}

void ContextWidget::tabCloseRquest(int i)
{
    closeFile(i);
    contextUpdate();
}

bool ContextWidget::saveFile(int i)
{
    if(Editor * w = getEditor(i))
    {
        if(!w->saveFile())
        {
            QMessageBox::critical(NULL, "Error", "Save file" + getPathFromNumber(i) , QMessageBox::Ok);
            return false;
        }
        return true;
    }
    return false;
}

bool ContextWidget::closeFile(int i)
{
    // 如果已经打开过，则close 并从map中删除
    Editor* w = getEditor(i);
    if(!w) return false;

    if( !w->isSaved())
    {
        QMessageBox::StandardButton choice = QMessageBox::information(NULL, "保存提示", getPathFromNumber(i) + " " + "文件已修改，是否保存?", QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
        if( QMessageBox::Yes == choice)
        {
            saveFile(i);
        }
        else if( QMessageBox::No == choice)
        {

        }
        else
        {
            return false;
        }
    }

    //关闭当前页面
    ui->tabWidget->removeTab(i);
    contextUpdate();
    return true;
}

int ContextWidget::getTabNumber(const QString &path) const
{
    for(int i = 0;i < ui->tabWidget->count();++i)
    {
        if(Editor *w = dynamic_cast<Editor*>(ui->tabWidget->widget(i)))
        {
            if(path == w->getFilePath())
            {
                return i;
            }
        }
    }
    return -1;
}

const QString &ContextWidget::getPathFromNumber(int i) const
{
    if(Editor *w = dynamic_cast<Editor*>(ui->tabWidget->widget(i)))
    {
        return w->getFilePath();
    }
    return EMPTYSTRING;
}

Editor *ContextWidget::getCurrentEditor() const
{
    if(ui->tabWidget->currentWidget())
    {
        return dynamic_cast<Editor*>(ui->tabWidget->currentWidget());
    }
    return nullptr;
}

Editor *ContextWidget::getEditor(int i) const
{
    if(ui->tabWidget->widget(i))
    {
        return dynamic_cast<Editor*>(ui->tabWidget->widget(i));
    }
    return nullptr;

}

Editor *ContextWidget::getEditor(const QString &path) const
{
    for(int i = 0;i < ui->tabWidget->count();++i)
    {
        if(Editor *w = getEditor(i))
        {
            if(w->getFilePath() == path)
            {
                return w;
            }
        }
    }
    return nullptr;
}

void ContextWidget::contextUpdate()
{
    for(int i = 0;i < ui->tabWidget->count();++i)
    {
        if(Editor *w = getEditor(i))
        {
            ui->tabWidget->setTabIcon(i,w->isSaved()?QIcon(":/pic/saved.png"):QIcon(":/pic/unsaved.png"));
        }
    }
    emit contentStateChange();
}

void ContextWidget::InitWidget()
{
    //ui->tabWidget->setStyleSheet("background-color:black;");
    ui->tabWidget->setTabsClosable(true);
    connect(ui->tabWidget,&QTabWidget::tabCloseRequested,this,&ContextWidget::tabCloseRquest);
    connect(ui->tabWidget,&QTabWidget::currentChanged,this,&ContextWidget::currentTabChanged);
    connect((QObject *)bridge::instance(), SIGNAL(textChanged()), this, SLOT(onTextChanged()), Qt::QueuedConnection);
    connect((QObject *)bridge::instance(), SIGNAL(gutterRightClickedSignal()), this, SLOT(gutterRightClickedSignal()), Qt::QueuedConnection);
}

