#include "FileView.h"

#include <QCoreApplication>
#include <QTimer>
#include <QHeaderView>
#include <QFileDialog>
#include <QMessageBox>

#include "FileModel.h"
#include "DataDefine.h"
#include "IDEUtil.h"
#include "ConvenientOp.h"

#include "control/contextmenu.h"
class FileView::DataPrivate
{
public:
    DataPrivate()
        :fileModel(new FileModel())
    {

    }
    ~DataPrivate()
    {
        delete fileModel;
        fileModel = nullptr;
    }

public:
    FileModel *fileModel;
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
    QModelIndex index = _p->fileModel->index(filePath);
    if(!index.isValid()) return;
    setCurrentIndex(index);
}

void FileView::deleteFileSlots()
{
    QString filePath = _p->fileModel->filePath(this->currentIndex());
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
    QString dirpath = _p->fileModel->filePath(this->currentIndex());

    ConvenientOp::ImportContractFile(dirpath);
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
        QFileInfo file(_p->fileModel->filePath(index));
        if(file.isFile())
        {
            emit fileClicked(_p->fileModel->filePath(index));
        }
    }
}

QString FileView::getCurrentFilePath() const
{
    return _p->fileModel->filePath(this->currentIndex());
}

void FileView::InitWidget()
{
    setModel(_p->fileModel);
    _p->fileModel->setNameFilterDisables(false);
    _p->fileModel->setNameFilters(QStringList()<<"*."+DataDefine::GLUA_SUFFIX<<"*."+DataDefine::JAVA_SUFFIX
                                               <<"*."+DataDefine::KOTLIN_SUFFIX<<"*."+DataDefine::CSHARP_SUFFIX
                                               <<"*.gpc");
    _p->fileModel->setRootPath("/");
    setRootIndex(_p->fileModel->index(QCoreApplication::applicationDirPath()+QDir::separator()+"contracts"));

    setSelectionMode(QAbstractItemView::SingleSelection);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    header()->setVisible(false);

    //setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    for(int i = 1;i < _p->fileModel->columnCount();++i)
    {
        hideColumn(i);
    }

    QTimer::singleShot(100,this,&QTreeView::expandAll);

    connect(this,&QTreeView::clicked,this,&FileView::IndexClicked);
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
        QString path = _p->fileModel->filePath(ind);
        QFileInfo info(path);
        if(info.isFile())
        {
            if(info.suffix() == DataDefine::GLUA_SUFFIX || info.suffix() == DataDefine::JAVA_SUFFIX ||
                   info.suffix() == DataDefine::CSHARP_SUFFIX || info.suffix() == DataDefine::KOTLIN_SUFFIX)
            {
                menu = new ContextMenu(ContextMenu::Delete | ContextMenu::Compile,this);
            }
            else
            {
                menu = new ContextMenu(ContextMenu::Delete ,this);
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
        connect(menu,&ContextMenu::newFileTriggered,this,&FileView::newFileSlots);
        menu->exec(QCursor::pos());
    }

}

void FileView::retranslator()
{
    //ui->retranslateUi(this);
}
