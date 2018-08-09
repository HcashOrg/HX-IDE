#include "FileView.h"

#include <QCoreApplication>
#include <QTimer>
#include <QHeaderView>
#include <QFileDialog>
#include <QMessageBox>
#include <QFileSystemModel>

#include "FileProxyModel.h"
#include "DataDefine.h"
#include "IDEUtil.h"
#include "ConvenientOp.h"

#include "control/contextmenu.h"

class FileView::DataPrivate
{
public:
    DataPrivate()
        :fileModel(new QFileSystemModel())
        ,proxy(new FileProxyModel())
    {
        fileModel->setRootPath(QCoreApplication::applicationDirPath()+QDir::separator()+DataDefine::CONTRACT_DIR);
        proxy->setSourceModel(fileModel);
    }
    ~DataPrivate()
    {
        delete fileModel;
        fileModel = nullptr;
        delete proxy;
        proxy = nullptr;
    }

public:
    QFileSystemModel *fileModel;
    FileProxyModel *proxy;
};

FileView::FileView(QWidget *parent)
    :QTreeView(parent)
    ,_p(new DataPrivate())
{
    InitWidget();
}

FileView::~FileView()
{
    delete _p;
}

void FileView::selectFile(const QString &filePath)
{
    if(filePath.isEmpty()) return;
    QModelIndex index = getProxyIndexFromPath(filePath);
    if(!index.isValid()) return;
    setCurrentIndex(index);
}

void FileView::deleteFileSlots()
{
    QString filePath = getFilePathFromIndex(this->currentIndex());
    if(QFileInfo(filePath).isFile())
    {
        if(QFileInfo(filePath).suffix() == DataDefine::CONTRACT_SUFFIX)
        {
            QString meta = filePath;
            meta.replace(QRegExp(DataDefine::CONTRACT_SUFFIX+"$"),DataDefine::META_SUFFIX);
            QFile::remove(meta);
        }
        QFile::remove(filePath);
    }
    else if(QFileInfo(filePath).isDir())
    {
        IDEUtil::deleteDir(filePath);
    }

    emit deleteFile();
}

void FileView::importContractSlots()
{
    QString dirpath = getFilePathFromIndex(this->currentIndex());

    ConvenientOp::ImportContractFile(dirpath);
}

void FileView::exportContractSlots()
{
    QString dirpath = getFilePathFromIndex(this->currentIndex());

    ConvenientOp::ExportContractFile(dirpath);
}

void FileView::newFileSlots()
{
    QStringList suffixs = ConvenientOp::GetContractSuffixByDir(getCurrentFilePath());
    if(!suffixs.empty())
    {
        emit newFile(suffixs.front(),getCurrentFilePath());
    }
}

void FileView::IndexClicked(const QModelIndex &index)
{
    if(index.isValid())
    {
        QFileInfo file = getFileInfoFromIndex(index);
        if(file.isFile())
        {
            emit fileClicked(getFilePathFromIndex(index));
        }
    }
}

QString FileView::getCurrentFilePath() const
{
    return getFilePathFromIndex(this->currentIndex());
}

void FileView::InitWidget()
{
    setModel(_p->proxy);

    setRootIndex(_p->proxy->mapFromSource(_p->fileModel->index(_p->fileModel->rootPath())));

    setSelectionMode(QAbstractItemView::SingleSelection);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    header()->setVisible(false);

    QTimer::singleShot(100,this,&QTreeView::expandAll);

    connect(this,&QTreeView::clicked,this,&FileView::IndexClicked);
}

QString FileView::getFilePathFromIndex(const QModelIndex &index) const
{
    return _p->fileModel->filePath(_p->proxy->mapToSource(index));
}

QFileInfo FileView::getFileInfoFromIndex(const QModelIndex &index) const
{
    return _p->fileModel->fileInfo(_p->proxy->mapToSource(index));
}

QModelIndex FileView::getFileIndexFromPath(const QString &filePath) const
{
    return _p->fileModel->index(filePath);
}

QModelIndex FileView::getProxyIndexFromPath(const QString &filePath) const
{
    return _p->proxy->mapFromSource(_p->fileModel->index(filePath));
}

void FileView::contextMenuEvent(QContextMenuEvent *e)
{
    ContextMenu *menu = nullptr;

    QModelIndex ind = this->indexAt(this->mapFromGlobal(QCursor::pos()));
    if(!ind.isValid()) return;

    if(ind.parent() == this->rootIndex())
    {
        menu = new ContextMenu(ContextMenu::Import | ContextMenu::NewFile,this);
    }
    else
    {
        QFileInfo info = getFileInfoFromIndex(ind);
        if(info.isFile())
        {
            if(info.suffix() == DataDefine::GLUA_SUFFIX || info.suffix() == DataDefine::JAVA_SUFFIX ||
               info.suffix() == DataDefine::CSHARP_SUFFIX || info.suffix() == DataDefine::KOTLIN_SUFFIX)
            {
                menu = new ContextMenu(ContextMenu::Delete | ContextMenu::Compile,this);
            }
            else if(info.suffix() == DataDefine::CONTRACT_SUFFIX)
            {
                menu = new ContextMenu(ContextMenu::Delete | ContextMenu::Export,this);
            }
        }
        else if(info.isDir())
        {
            menu = new ContextMenu(ContextMenu::Delete | ContextMenu::Import | ContextMenu::NewFile,this);
        }
    }


    if(menu)
    {
        connect(menu,&ContextMenu::compileTriggered,this,&FileView::compileFile);

        connect(menu,&ContextMenu::deleteTriggered,this,&FileView::deleteFileSlots);

        connect(menu,&ContextMenu::importTriggered,this,&FileView::importContractSlots);
        connect(menu,&ContextMenu::exportTriggered,this,&FileView::exportContractSlots);
        connect(menu,&ContextMenu::newFileTriggered,this,&FileView::newFileSlots);
        menu->exec(QCursor::pos());
    }

}

void FileView::retranslator()
{
    //ui->retranslateUi(this);
}
