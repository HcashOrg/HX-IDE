#include "contentwidget.h"
#include "ui_contentwidget.h"
#include "aceeditor.h"
#include "control/tabBarWidget.h"
#include "bridge.h"
#include <QLayout>
#include <QMessageBox>
#include <QDebug>
#include <QFileDialog>
#include <QTextCodec>


ContentWidget::ContentWidget(QWidget *parent) :
    QWidget(parent),
    currentFilePath(""),
    ui(new Ui::ContentWidget)
{
    ui->setupUi(this);

    connect(ui->tabBarWidget,SIGNAL(fileClosed(QString)),this,SLOT(onFileClosed(QString)));
    connect(ui->tabBarWidget,SIGNAL(tabSelected(QString)),this,SLOT(onTabSelected(QString)));
    ui->tabBarWidget->setStyleSheet("#ui->tabBarWidget{background-color:red;}");

    connect((QObject *)bridge::instance(), SIGNAL(textChanged()), this, SLOT(onTextChanged()), Qt::QueuedConnection);
}

ContentWidget::~ContentWidget()
{
    delete ui;
}

void ContentWidget::undo()
{
    AceEditor* w = pathAceEditorMap.value(currentFilePath);
    if( w != NULL)
    {
        w->undo();
    }
}

void ContentWidget::redo()
{
    AceEditor* w = pathAceEditorMap.value(currentFilePath);
    if( w != NULL)
    {
        w->redo();
    }
}

bool ContentWidget::isUndoAvailable()
{
    AceEditor* w = pathAceEditorMap.value(currentFilePath);
    if( w != NULL)
    {
        if( !w->isEditable())  return false;
        return w->isUndoAvailable();
    }
    else
    {
        return false;
    }

}

bool ContentWidget::isRedoAvailable()
{
    AceEditor* w = pathAceEditorMap.value(currentFilePath);
    if( w != NULL)
    {
        if( !w->isEditable())  return false;
        return w->isRedoAvailable();
    }
    else
    {
        return false;
    }
}

bool ContentWidget::hasFileUnsaved()
{
    bool hasUnsaved = false;
    QStringList paths = pathAceEditorMap.keys();
    foreach (QString path, paths)
    {
        AceEditor* w = pathAceEditorMap.value(path);
        if( w != NULL)
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

bool ContentWidget::currentFileUnsaved()
{
    AceEditor* w = pathAceEditorMap.value(currentFilePath);
    if( w != NULL)
    {
        return !w->isSaved();
    }
    else
    {
        return false;
    }
}

void ContentWidget::closeSandboxFile()
{
    QStringList paths = pathAceEditorMap.keys();
    foreach (QString path, paths)
    {
        if( path.endsWith("_sandbox.glua") || path.endsWith("_sandbox.gpc"))
        {
            closeFile(path);
        }
    }
}

const QString &ContentWidget::getCurrentFilePath() const
{
    return currentFilePath;
}

const QMap<QString, AceEditor *> &ContentWidget::getContentMap() const
{
    return pathAceEditorMap;
}

void ContentWidget::showFile(QString path)
{
    if(!pathAceEditorMap.contains(path))
    {
        // 如果没打开过，打开并添加进map
        AceEditor* w = new AceEditor(path, true , this);
        ui->contentWidget->addWidget(w);
        ui->contentWidget->setCurrentWidget(w);
        pathAceEditorMap[path] = w;

        QFileInfo info(path);
        ui->tabBarWidget->addTab(info.fileName(), path);

        currentFilePath = path;


    }
    else
    {
        // 如果已经打开过，则显示出来
        AceEditor* w = pathAceEditorMap.value(path);
        if( w != nullptr)
        {
            ui->contentWidget->setCurrentWidget(w);
            currentFilePath = path;
            ui->tabBarWidget->setCurrentPath(path);
        }
    }
    emit contentStateChange();
}

bool ContentWidget::closeFile(QString path)
{
    if( !pathAceEditorMap.contains(path) || !pathAceEditorMap.value(path)) return false;

    // 如果已经打开过，则close 并从map中删除
    AceEditor* w = pathAceEditorMap.value(path);

    if( !w->isSaved())
    {
        QMessageBox::StandardButton choice = QMessageBox::information(NULL, "", path + " " + QString::fromLocal8Bit("文件已修改，是否保存?"), QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
        if( QMessageBox::Yes == choice)
        {
            saveFile(path);
        }
        else if( QMessageBox::No == choice)
        {

        }
        else
        {
            return false;
        }
    }

    pathAceEditorMap.remove(path);

    QString nextTab = ui->tabBarWidget->getNextTab(path);
    ui->tabBarWidget->removeTab(path);

    if( currentFilePath == path)
    {
        // 如果当前显示的就是要关闭的文件 显示后一个文件  如果没有文件了 currentFilePath置为空
        if( !nextTab.isEmpty())
        {
            onTabSelected(nextTab);
        }
        else
        {
            currentFilePath = "";
            onTabSelected("");
        }
    }

    if( w != NULL)
    {
        w->close();
    }
    else
    {
        qDebug() << " contentWidget closeFile() error";
    }

    emit contentStateChange();

    return true;

}

bool ContentWidget::closeAll()
{
    QStringList paths = pathAceEditorMap.keys();
    foreach (QString path, paths)
    {
        if(!closeFile(path))
        {
            return false;
        }
    }
}

void ContentWidget::saveFile()
{
    if( currentFilePath.isEmpty())   return;

    saveFile(currentFilePath);
}

void ContentWidget::saveFile(QString path)
{
    if( !pathAceEditorMap.keys().contains(path))     return;

    if(AceEditor* w = pathAceEditorMap[path])
    {
        if(w->saveFile())
        {
            ui->tabBarWidget->onFileSaved(path);
            emit contentStateChange();
        }
        else
        {
            QMessageBox::critical(NULL, "Error", "Save file" + path , QMessageBox::Ok);
        }
    }
}

void ContentWidget::saveAll()
{
    foreach(QString path,pathAceEditorMap.keys()){
        saveFile(path);
    }
}

void ContentWidget::onFileClosed(QString path)
{
    closeFile(path);
}

void ContentWidget::onTabSelected(QString path)
{
    if( !path.isEmpty())    showFile(path);
    emit fileSelected(path);
    emit contentStateChange();
}

void ContentWidget::onTextChanged()
{
    qDebug() << "ContentWidget::onTextChanged" ;//<< isUndoAvailable();
    if( isUndoAvailable())      // 如果可撤销  则未保存
    {
        ui->tabBarWidget->onTextChanged();

        AceEditor* w = pathAceEditorMap.value(currentFilePath);
        w->setSaved(false);
    }
    else
    {
        // 如果不可撤销 则已保存
        ui->tabBarWidget->onFileSaved(currentFilePath);

        AceEditor* w = pathAceEditorMap.value(currentFilePath);
        w->setSaved(true);
    }

    emit textChanged();
    emit contentStateChange();
}

void ContentWidget::onFileDeleted(QString path)
{
    if( pathAceEditorMap.keys().contains(path))
    {
        AceEditor* w = pathAceEditorMap.value(path);
        w->setSaved(true);          // 为了close的时候不提示已修改文件是否保存
        closeFile(path);
    }
}

