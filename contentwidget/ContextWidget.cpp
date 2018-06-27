#include "ContextWidget.h"
#include "ui_ContextWidget.h"

#include <QFileInfo>
#include <QMessageBox>
#include <QDebug>

#include "aceeditor.h"
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
    if(AceEditor *editor = getCurrentEditor())
    {
        editor->undo();
        contextUpdate();
    }
}

void ContextWidget::redo()
{
    if(AceEditor *editor = getCurrentEditor())
    {
        editor->redo();
        contextUpdate();
    }
}

bool ContextWidget::isUndoAvailable()
{
    if(AceEditor *editor = getCurrentEditor())
    {
        return editor->isUndoAvailable();
    }
    return false;
}

bool ContextWidget::isRedoAvailable()
{
    if(AceEditor *editor = getCurrentEditor())
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
        if( AceEditor* w = getEditor(i))
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
    if(AceEditor *editor = getCurrentEditor())
    {
        return !editor->isSaved();
    }
    return false;
}

const QString &ContextWidget::getCurrentFilePath() const
{
    return getPathFromNumber(ui->tabWidget->currentIndex());
}

void ContextWidget::showFile(QString path)
{
    for(int i = 0;i < ui->tabWidget->count();++i)
    {
        if(AceEditor* w = dynamic_cast<AceEditor*>(ui->tabWidget->widget(i)))
        {
            if(w->getFilePath() == path)
            {
                ui->tabWidget->setCurrentIndex(i);
                return;
            }
        }

    }
    //如果没找到，新建一个
    AceEditor* w = new AceEditor(path, true );


    ui->tabWidget->addTab(w,QIcon(":/pic/saved.png"),QFileInfo(path).fileName());

    ui->tabWidget->setCurrentWidget(w);
}

bool ContextWidget::closeFile(QString path)
{
    closeFile(getTabNumber(path));
    contextUpdate();
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

    contextUpdate();
    return true;
}

void ContextWidget::saveFile()
{
    saveFile(ui->tabWidget->currentIndex());
    contextUpdate();
}

void ContextWidget::saveFile(QString path)
{
    int i = getTabNumber(path);
    if(-1 != i)
    {
        saveFile(i);
        contextUpdate();
    }
}

void ContextWidget::saveAll()
{
    for(int i = 0;i < ui->tabWidget->count();++i)
    {
        saveFile(i);
    }
    contextUpdate();
}

void ContextWidget::onTextChanged()
{
    qDebug() << "ContentWidget::onTextChanged" ;//<< isUndoAvailable();

    AceEditor *w = getCurrentEditor();
    if(!w) return;

    if( isUndoAvailable())      // 如果可撤销  则未保存
    {
        w->setSaved(false);
    }
    else
    {
        // 如果不可撤销 则已保存
        w->setSaved(true);
    }

    contextUpdate();
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
    if(AceEditor * w = getEditor(i))
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
    AceEditor* w = getEditor(i);
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
    return true;
}

int ContextWidget::getTabNumber(const QString &path) const
{
    for(int i = 0;i < ui->tabWidget->count();++i)
    {
        if(AceEditor *w = dynamic_cast<AceEditor*>(ui->tabWidget->widget(i)))
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
    if(AceEditor *w = dynamic_cast<AceEditor*>(ui->tabWidget->widget(i)))
    {
        return w->getFilePath();
    }
    return EMPTYSTRING;
}

AceEditor *ContextWidget::getCurrentEditor() const
{
    if(ui->tabWidget->currentWidget())
    {
        qDebug()<<"getcurrent";
        return dynamic_cast<AceEditor*>(ui->tabWidget->currentWidget());
    }
    return nullptr;
}

AceEditor *ContextWidget::getEditor(int i) const
{
    if(ui->tabWidget->widget(i))
    {
        return dynamic_cast<AceEditor*>(ui->tabWidget->widget(i));
    }
    return nullptr;

}

AceEditor *ContextWidget::getEditor(const QString &path) const
{
    for(int i = 0;i < ui->tabWidget->count();++i)
    {
        if(AceEditor *w = getEditor(i))
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
        if(AceEditor *w = getEditor(i))
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
}

