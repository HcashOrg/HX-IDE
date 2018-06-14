#include "contentwidget.h"
#include "ui_contentwidget.h"
#include "codeeditor.h"
#include "hxchain.h"
#include "aceeditor.h"
#include "control/tabbarwidget.h"
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

    tabBarWidget = new TabBarWidget(this);
    connect(tabBarWidget,SIGNAL(fileClosed(QString)),this,SLOT(onFileClosed(QString)));
    connect(tabBarWidget,SIGNAL(tabSelected(QString)),this,SLOT(onTabSelected(QString)));
    tabBarWidget->setStyleSheet("#TabBarWidget{background-color:red;}");

    QVBoxLayout* vbLayout = new QVBoxLayout;
    vbLayout->addWidget(tabBarWidget);
    vbLayout->addWidget(ui->containerWidget);
    vbLayout->setMargin(0);
    vbLayout->setSpacing(0);
    this->setLayout(vbLayout);
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

void ContentWidget::showFile(QString path)
{
    if( pathAceEditorMap.contains(path) )
    {
        // 如果已经打开过，则显示出来
        AceEditor* w = pathAceEditorMap.value(path);
        if( w != NULL)
        {
            w->setGeometry(0,0, this->width(), this->height());
            w->show();
            w->raise();

            currentFilePath = path;

            tabBarWidget->setCurrentPath(path);
        }
        else
        {
            qDebug() << " contentWidget showfile() error";
        }
    }
    else
    {
        // 如果没打开过，打开并添加进map
        AceEditor* w = new AceEditor(path, ui->containerWidget);
        w->setAttribute(Qt::WA_DeleteOnClose);
        connect((QObject *)bridge::instance(), SIGNAL(textChanged()), this, SLOT(onTextChanged()), Qt::QueuedConnection);
        //connect(w,SIGNAL(textChanged()), this, SLOT(onTextChanged()));
        pathAceEditorMap.insert(path,w);

        QFileInfo info(path);
        tabBarWidget->addTab(info.fileName(), path);

        QDir dir(path);
        qDebug() << "show file: " << path;
        if( HXChain::getInstance()->fileRecordMap.value(dir.absolutePath()).newBuilt )
        {
            // 如果是新建文件
            w->setGeometry(0,0, this->width(), this->height());

            w->show();
            w->raise();
            w->setMode("glua");

            if( HXChain::getInstance()->isInContracts(path))
            {
                // 新建的合约显示 初始模板
                w->setInitTemplate();
            }
            else
            {
                w->initFinish();
            }

            currentFilePath = path;

            return;
        }

        QFile file(path);
        if( file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            QTextCodec* gbkCodec = QTextCodec::codecForName("GBK");
            QByteArray ba = file.readAll();
            if( path.endsWith(".gpc"))
            {
                //                w->appendPlainText( QString(ba.toHex()));
                w->setText(ba.toHex());
            }
            else
            {
                //                w->appendPlainText( QString( gbkCodec->toUnicode(ba) ));
                w->setText( QString( gbkCodec->toUnicode(ba) ));
            }

            file.close();
            w->setGeometry(0,0, this->width(), this->height());

            if( path.endsWith(".gpc"))
            {
                // 如果是字节码文件 不允许编辑
                w->setReadOnly(true);
                w->setEditable(false);
                w->setMode("text");
            }
            else if( path.endsWith(".script"))
            {
                // 如果是字节码文件 不允许编辑
                w->setReadOnly(true);
                w->setEditable(false);
                w->setMode("text");
            }
            else
            {
                w->setMode("glua");
            }

            dir.cdUp();
            dir.cdUp();
            QDir dir2("registered");

            if( dir2.absolutePath() ==  dir.absolutePath() )
            {
                // 如果合约已经注册了 不允许编辑

                mutexForEditor.lock();

                w->moveCursorTo(0,0);
                QString address = HXChain::getInstance()->configGetContractAddress(dir.absoluteFilePath(path));

                w->insert( QString::fromLocal8Bit("合约地址    ") + address + "\n");
                w->insert( QString::fromLocal8Bit("合约名称    ") + HXChain::getInstance()->contractInfoMap.value(address).name + "\n");
                w->insert( QString::fromLocal8Bit("合约描述    ") + HXChain::getInstance()->contractInfoMap.value(address).description + "\n");
                if( !HXChain::getInstance()->contractInfoMap.value(address).ownerName.isEmpty())
                {   // 如果有ownername 则显示ownername  否则显示ownerAddress
                    w->insert( QString::fromLocal8Bit("合约所有者  ") + HXChain::getInstance()->contractInfoMap.value(address).ownerName + "\n");
                }
                else
                {
                    w->insert( QString::fromLocal8Bit("合约所有者  ") + HXChain::getInstance()->contractInfoMap.value(address).ownerAddress + "\n");
                }
                w->insert( QString::fromLocal8Bit("合约状态    ") + HXChain::getInstance()->contractInfoMap.value(address).level + "\n");

                w->insert("\n");
                w->insert("\n");

                w->setReadOnly(true);
                w->setEditable(false);

                mutexForEditor.unlock();
            }

            if( !HXChain::getInstance()->configGetScriptId(path).isEmpty() )
            {
                w->moveCursorTo(0,0);
                QString scriptId = HXChain::getInstance()->configGetScriptId(path);
                w->insert( QString::fromLocal8Bit("脚本ID      ") + scriptId + "\n");

                if( HXChain::getInstance()->scriptInfoMap.contains(scriptId))
                {
                    w->insert( QString::fromLocal8Bit("脚本描述    ") + HXChain::getInstance()->scriptInfoMap.value(scriptId).description + "\n");
                }

                w->insert("\n");
                w->insert("\n");

                // 如果脚本已经添加到钱包 不允许编辑
                w->setReadOnly(true);
                w->setEditable(false);
            }

//            w->document()->clearUndoRedoStacks();  // 清空撤销/恢复操作 不然append的内容可撤销
            w->initFinish();


            w->setGeometry(0,0, this->width(), this->height());
            w->show();
            w->raise();

            currentFilePath = path;
        }
        else
        {
            QMessageBox::critical(NULL, "Error", "Open file " + path + " error : "  + file.errorString(), QMessageBox::Ok);
        }
    }
}

bool ContentWidget::closeFile(QString path)
{
    if( pathAceEditorMap.contains(path) )
    {
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

        QString nextTab = tabBarWidget->getNextTab(path);
        tabBarWidget->removeTab(path);

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

        return true;
    }
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

void ContentWidget::showContract(QString address)
{
    if( pathAceEditorMap.contains(address) )
    {
        // 如果已经打开过，则显示出来
        AceEditor* w = pathAceEditorMap.value(address);
        if( w != NULL)
        {
            w->setGeometry(0,0, this->width(), this->height());
            w->show();
            w->raise();

            currentFilePath = address;

            tabBarWidget->setCurrentPath(address);
        }
        else
        {
            qDebug() << " contentWidget showContract() error";
        }
    }
    else
    {
        // 如果没打开过，打开并添加进map
        qDebug() << "mmmmmmmmmmmmmmmmmmmmmm ";

        AceEditor* w = new AceEditor(address, ui->containerWidget);
        w->setAttribute(Qt::WA_DeleteOnClose);
//        connect(w,SIGNAL(textChanged()), this, SLOT(onTextChanged()));
        pathAceEditorMap.insert(address,w);

        tabBarWidget->addTab(address.left(6) + "...", address);

        w->setGeometry(0,0, this->width(), this->height());
        w->show();
        w->raise();

        mutexForEditor.lock();

        w->setMode("glua");
        w->setReadOnly(true);
        w->setEditable(false);
        currentFilePath = address;

        w->insert( QString::fromLocal8Bit("合约地址    ") + address + "\n");
        w->insert( QString::fromLocal8Bit("合约名称    ") + HXChain::getInstance()->foreverContractInfoMap.value(address).name + "\n");
        w->insert( QString::fromLocal8Bit("合约描述    ") + HXChain::getInstance()->foreverContractInfoMap.value(address).description + "\n");
        if( !HXChain::getInstance()->contractInfoMap.value(address).ownerName.isEmpty())
        {   // 如果有ownername 则显示ownername  否则显示ownerAddress
            w->insert( QString::fromLocal8Bit("合约所有者  ") + HXChain::getInstance()->contractInfoMap.value(address).ownerName + "\n");
        }
        else
        {
            w->insert( QString::fromLocal8Bit("合约所有者  ") + HXChain::getInstance()->contractInfoMap.value(address).ownerAddress + "\n");
        }
        w->insert( QString::fromLocal8Bit("合约状态    ") + HXChain::getInstance()->contractInfoMap.value(address).level + "\n");

        w->initFinish();

        mutexForEditor.unlock();
    }

}

void ContentWidget::saveFile()
{
    if( currentFilePath.isEmpty())   return;

    QFileInfo info(currentFilePath);
    QDir dir1("contracts");
    QDir dir2("scripts");

    // contracts 和scripts 下的.cs文件才需要保存
    qDebug() << "dir1: " << dir1 << ", dir2: " << dir2 << ", currentFilePath: " << currentFilePath;
    if( !(info.absolutePath() == dir1.absolutePath() || info.absolutePath() == dir2.absolutePath()) )   return;
    if( !currentFilePath.endsWith(".glua")  )     return;

    QDir dir(currentFilePath);

    if( HXChain::getInstance()->fileRecordMap.value( dir.absolutePath()).newBuilt)
    {
        // 如果是新建的  弹出保存文件对话框
        QString filePath = QFileDialog::getSaveFileName(this, QString::fromLocal8Bit("保存"), dir.path(), "(*.glua)");

        if( filePath.isEmpty())  return;
        if( !filePath.endsWith(".glua"))    filePath.append(".glua");    // 如果filedialog里填的文件名含后缀 则不会自动添加.glua后缀 需自己添加

        QFile file(filePath);
        if( file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
        {
            QTextStream out(&file);
            out << pathAceEditorMap.value(currentFilePath)->getText();
            file.close();

            if( HXChain::getInstance()->isInContracts(filePath) ||  HXChain::getInstance()->isInScripts(filePath))
            {
                // 如果保存在contracts或scripts文件夹下 名称变更后 作相应修改

                if( currentFilePath == filePath)        return;

                // TODO  如果覆盖了原来的文件   contentWidget中未关闭原来的文件
                if( pathAceEditorMap.contains(filePath))
                {
                    pathAceEditorMap.value(filePath)->close();
                    tabBarWidget->removeTab(filePath);
                    pathAceEditorMap.insert( filePath, pathAceEditorMap.value(currentFilePath));
                    pathAceEditorMap.value(filePath)->setSaved(true);
                    pathAceEditorMap.remove( currentFilePath);

                    emit newFileSaved( currentFilePath, filePath);

                    tabBarWidget->onFileSaved(currentFilePath);
                    tabBarWidget->modifyTabPath(currentFilePath, filePath);

                    currentFilePath = filePath;
                }
                else
                {
                    pathAceEditorMap.insert( filePath, pathAceEditorMap.value(currentFilePath));
                    pathAceEditorMap.value(filePath)->setSaved(true);
                    pathAceEditorMap.remove( currentFilePath);


                    emit newFileSaved( currentFilePath, filePath);

                    tabBarWidget->onFileSaved(currentFilePath);
                    tabBarWidget->modifyTabPath(currentFilePath, filePath);
                    currentFilePath = filePath;
                }


            }

        }
        else
        {
            QMessageBox::critical(NULL, "Error", "Save new file" + currentFilePath + " error : " + file.errorString(), QMessageBox::Ok);
        }

        return;
    }


    QFile file(currentFilePath);
    if( file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
    {
        QTextStream out(&file);
        out << pathAceEditorMap.value(currentFilePath)->getText();
        file.close();

        pathAceEditorMap.value(currentFilePath)->setSaved(true);
        tabBarWidget->onFileSaved(currentFilePath);
    }
    else
    {
        QMessageBox::critical(NULL, "Error", "Save file" + currentFilePath + " error : " + file.errorString(), QMessageBox::Ok);
    }

}

void ContentWidget::saveFile(QString path)
{
    if( !pathAceEditorMap.keys().contains(path))     return;

    QFileInfo info(path);
    QDir dir1("contracts");
    QDir dir2("scripts");

    // contracts 和scripts 下的.glua文件才需要保存
    if( !(info.absolutePath() == dir1.absolutePath() || info.absolutePath() == dir2.absolutePath()) )   return;
    if( !path.endsWith(".glua")  )     return;

    QDir dir(path);
    if( HXChain::getInstance()->fileRecordMap.value( dir.absolutePath()).newBuilt)
    {
        // 如果是新建的  弹出保存文件对话框
        QString filePath = QFileDialog::getSaveFileName(this, QString::fromLocal8Bit("保存"), dir.path(), "(*.glua)");

        if( filePath.isEmpty())  return;
        if( !filePath.endsWith(".glua"))    filePath.append(".glua");    // 如果filedialog里填的文件名含后缀 则不会自动添加.glua后缀 需自己添加

        QFile file(filePath);
        if( file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
        {
            QTextStream out(&file);
            out << pathAceEditorMap.value(path)->getText();
            file.close();

            if( HXChain::getInstance()->isInContracts(filePath) ||  HXChain::getInstance()->isInScripts(filePath))
            {
                // 如果保存在contracts或scripts文件夹下 名称变更后 作相应修改

                if( path == filePath)        return;

                // TODO  如果覆盖了原来的文件   contentWidget中未关闭原来的文件
                if( pathAceEditorMap.contains(filePath))
                {
                    pathAceEditorMap.value(filePath)->close();
                    tabBarWidget->removeTab(filePath);
                    pathAceEditorMap.insert( filePath, pathAceEditorMap.value(path));
                    pathAceEditorMap.value(filePath)->setSaved(true);
                    pathAceEditorMap.remove( path);


                    emit newFileSaved( path, filePath);

                    tabBarWidget->onFileSaved(path);
                    tabBarWidget->modifyTabPath(path, filePath);

                    if( currentFilePath == path)        // 如果当前文件保存更改了文件名
                    {
                        currentFilePath = filePath;
                    }
                }
                else
                {
                    pathAceEditorMap.insert( filePath, pathAceEditorMap.value(path));
                    pathAceEditorMap.value(filePath)->setSaved(true);
                    pathAceEditorMap.remove( path);

                    emit newFileSaved( path, filePath);

                    tabBarWidget->onFileSaved(path);
                    tabBarWidget->modifyTabPath(path, filePath);

                    if( currentFilePath == path)        // 如果当前文件保存更改了文件名
                    {
                        currentFilePath = filePath;
                    }
                }


            }

        }
        else
        {
            QMessageBox::critical(NULL, "Error", "Save new file" + path + " error : " + file.errorString(), QMessageBox::Ok);
        }

        return;
    }


    QFile file(path);
    if( file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
    {
        QTextStream out(&file);
        out << pathAceEditorMap.value(path)->getText();
        file.close();

        pathAceEditorMap.value(path)->setSaved(true);
        tabBarWidget->onFileSaved(path);
    }
    else
    {
        QMessageBox::critical(NULL, "Error", "Save file" + path + " error : " + file.errorString(), QMessageBox::Ok);
    }
}

void ContentWidget::saveAll()
{
    QStringList paths = pathAceEditorMap.keys();
    foreach (QString path, paths)
    {
        QFileInfo info(path);
        QDir dir1("contracts");
        QDir dir2("scripts");

        // contracts 和scripts 下的.glua文件才需要保存
        if( !(info.absolutePath() == dir1.absolutePath() || info.absolutePath() == dir2.absolutePath()) )   continue;
        if( !path.endsWith(".glua")  )     continue;

        QDir dir(path);
        if( HXChain::getInstance()->fileRecordMap.value( dir.absolutePath()).newBuilt)
        {
            // 如果是新建的  弹出保存文件对话框
            QString filePath = QFileDialog::getSaveFileName(this, QString::fromLocal8Bit("保存"), dir.path(), "(*.glua)");

            if( filePath.isEmpty())  continue;
            if( !filePath.endsWith(".glua"))    filePath.append(".glua");    // 如果filedialog里填的文件名含后缀 则不会自动添加.glua后缀 需自己添加

            QFile file(filePath);
            if( file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
            {
                QTextStream out(&file);
                out << pathAceEditorMap.value(path)->getText();
                file.close();

                if( HXChain::getInstance()->isInContracts(filePath) ||  HXChain::getInstance()->isInScripts(filePath))
                {
                    // 如果保存在contracts或scripts文件夹下 名称变更后 作相应修改

                    if( path == filePath)        return;

                    // TODO  如果覆盖了原来的文件   contentWidget中未关闭原来的文件
                    if( pathAceEditorMap.contains(filePath))
                    {
                        pathAceEditorMap.value(filePath)->close();
                        tabBarWidget->removeTab(filePath);
                        pathAceEditorMap.insert( filePath, pathAceEditorMap.value(path));
                        pathAceEditorMap.value(filePath)->setSaved(true);
                        pathAceEditorMap.remove( path);

                        emit newFileSaved( path, filePath);

                        tabBarWidget->onFileSaved(path);
                        tabBarWidget->modifyTabPath(path, filePath);

                        if( currentFilePath == path)        // 如果当前文件保存更改了文件名
                        {
                            currentFilePath = filePath;
                        }
                    }
                    else
                    {
                        pathAceEditorMap.insert( filePath, pathAceEditorMap.value(path));
                        pathAceEditorMap.value(filePath)->setSaved(true);
                        pathAceEditorMap.remove( path);

                        emit newFileSaved( path, filePath);

                        tabBarWidget->onFileSaved(path);
                        tabBarWidget->modifyTabPath(path, filePath);

                        if( currentFilePath == path)        // 如果当前文件保存更改了文件名
                        {
                            currentFilePath = filePath;
                        }
                    }


                }

            }
            else
            {
                QMessageBox::critical(NULL, "Error", "Save new file" + path + " error : " + file.errorString(), QMessageBox::Ok);
            }

            continue;
        }


        QFile file(path);
        if( file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
        {
            QTextStream out(&file);
            out << pathAceEditorMap.value(path)->getText();
            file.close();

            pathAceEditorMap.value(path)->setSaved(true);
            tabBarWidget->onFileSaved(path);
        }
        else
        {
            QMessageBox::critical(NULL, "Error", "Save file" + path + " error : " + file.errorString(), QMessageBox::Ok);
        }
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
}

void ContentWidget::onTextChanged()
{
    qDebug() << "ContentWidget::onTextChanged" ;//<< isUndoAvailable();
    if( isUndoAvailable())      // 如果可撤销  则未保存
    {
        tabBarWidget->onTextChanged();

        AceEditor* w = pathAceEditorMap.value(currentFilePath);
        w->setSaved(false);
    }
    else
    {
        // 如果不可撤销 则已保存
        tabBarWidget->onFileSaved(currentFilePath);

        AceEditor* w = pathAceEditorMap.value(currentFilePath);
        w->setSaved(true);
    }

    emit textChanged();
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

void ContentWidget::onChainChanged()
{
    QStringList keys = pathAceEditorMap.keys();
    foreach (QString key, keys)
    {
        if( HXChain::getInstance()->isInContracts(key) || HXChain::getInstance()->isInScripts(key))
        {

        }
        else
        {
            closeFile(key);
        }

//        pathAceEditorMap.value(key)->hide();
    }
}

void ContentWidget::onAccountChanged()
{
    QStringList keys = pathAceEditorMap.keys();
    foreach (QString key, keys)
    {
        if( HXChain::getInstance()->isInContracts(key) || HXChain::getInstance()->isInScripts(key) || HXChain::getInstance()->foreverContractInfoMap.contains(key))
        {

        }
        else
        {
            closeFile(key);
        }

    }
}

void ContentWidget::contractInfoUpdated(QString address)
{
    QStringList keys = pathAceEditorMap.keys();
    foreach (QString key, keys)
    {
        if( key.startsWith("CON") || !key.contains('.'))
        {
            if( key == address)
            {
                AceEditor* w = pathAceEditorMap.value(key);

                mutexForEditor.lock();

                w->removeLines(0,4);

                w->moveCursorTo(0,0);
                w->insert( QString::fromLocal8Bit("合约地址    ") + address + "\n");
                w->insert( QString::fromLocal8Bit("合约名称    ") + HXChain::getInstance()->contractInfoMap.value(address).name + "\n");
                w->insert( QString::fromLocal8Bit("合约描述    ") + HXChain::getInstance()->contractInfoMap.value(address).description + "\n");
                if( !HXChain::getInstance()->contractInfoMap.value(address).ownerName.isEmpty())
                {   // 如果有ownername 则显示ownername  否则显示ownerAddress
                    w->insert( QString::fromLocal8Bit("合约所有者  ") + HXChain::getInstance()->contractInfoMap.value(address).ownerName + "\n");
                }
                else
                {
                    w->insert( QString::fromLocal8Bit("合约所有者  ") + HXChain::getInstance()->contractInfoMap.value(address).ownerAddress + "\n");
                }
                w->insert( QString::fromLocal8Bit("合约状态    ") + HXChain::getInstance()->contractInfoMap.value(address).level + "\n");

                mutexForEditor.unlock();
                continue;
            }
            else
            {
                continue;
            }
        }

        QString contractAddress = HXChain::getInstance()->configGetContractAddress(key);
        if( contractAddress == address)
        {
            AceEditor* w = pathAceEditorMap.value(key);

            mutexForEditor.lock();

            w->removeLines(0,4);

            w->moveCursorTo(0,0);
            w->insert( QString::fromLocal8Bit("合约地址    ") + address + "\n");
            w->insert( QString::fromLocal8Bit("合约名称    ") + HXChain::getInstance()->contractInfoMap.value(address).name + "\n");
            w->insert( QString::fromLocal8Bit("合约描述    ") + HXChain::getInstance()->contractInfoMap.value(address).description + "\n");
            if( !HXChain::getInstance()->contractInfoMap.value(address).ownerName.isEmpty())
            {   // 如果有ownername 则显示ownername  否则显示ownerAddress
                w->insert( QString::fromLocal8Bit("合约所有者  ") + HXChain::getInstance()->contractInfoMap.value(address).ownerName + "\n");
            }
            else
            {
                w->insert( QString::fromLocal8Bit("合约所有者  ") + HXChain::getInstance()->contractInfoMap.value(address).ownerAddress + "\n");
            }
            w->insert( QString::fromLocal8Bit("合约状态    ") + HXChain::getInstance()->contractInfoMap.value(address).level + "\n");

            mutexForEditor.unlock();
            continue;
        }

    }
}

//void ContentWidget::contractRegistered(QString path)
//{
//    // 合约注册后  合约改为不可编辑
//    CodeEditor* w = pathCodeEditorMap.value(path);
//    if( w != NULL)
//    {
//        w->setReadOnly(true);
//        w->show();
//        w->raise();
//        currentFilePath = path;
//    }
//}

//void ContentWidget::scriptAdded(QString path)
//{
//    // 脚本添加后  脚本改为不可编辑
//    CodeEditor* w = pathCodeEditorMap.value(path);
//    if( w != NULL)
//    {
//        w->setReadOnly(true);
//        w->show();
//        w->raise();
//        currentFilePath = path;
//    }
//}

void ContentWidget::resizeEvent(QResizeEvent *size)
{
    foreach (QString path, pathAceEditorMap.keys())
    {
        AceEditor* w = pathAceEditorMap.value(path);
        if( w != NULL)
        {
            w->setGeometry(0,0, size->size().width(), size->size().height());
        }
    }
}
