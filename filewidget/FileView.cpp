#include "FileView.h"

#include <QCoreApplication>
#include <QDebug>
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
    QModelIndex index = _p->fileModel->index(filePath);
    if(!index.isValid()) return;
//    selectionModel()->clearSelection();
//    selectionModel()->select(index,QItemSelectionModel::Select);
    setCurrentIndex(index);
}

void FileView::deleteFileSlots()
{
    QString filePath = _p->fileModel->filePath(this->currentIndex());
    if(QFileInfo(filePath).isFile())
    {
        QFile::remove(filePath);
    }
    else if(QFileInfo(filePath).isDir())
    {
        IDEUtil::deleteDir(filePath);
    }
}

void FileView::importContractSlots()
{
    QString dirpath = _p->fileModel->filePath(this->currentIndex());
    QString suffix = ConvenientOp::GetContractSuffixByDir(dirpath);
    if(suffix.isEmpty()) return;

    QString file = QFileDialog::getOpenFileName(this, tr("Choose your contract file."),"",QString("(*.%1)").arg(suffix));
    if(!QFileInfo(file).exists()) return;
    //正式导入文件
    QString dstFilePath = dirpath + "/" + QFileInfo(file).fileName();
    if(QFileInfo(dstFilePath).exists())
    {
        //提示存在,是否覆盖
        if(QMessageBox::Yes == QMessageBox::question(nullptr,tr("question"),tr("already exists,yes to override!")))
        {
            QFile::remove(dstFilePath);
            QFile::copy(file, dstFilePath);
        }
    }
    else
    {
        QFile::copy(file, dstFilePath);
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
        menu = new ContextMenu(ContextMenu::Import,this);
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
            menu = new ContextMenu(ContextMenu::Delete | ContextMenu::Import,this);
        }
    }


    if(menu)
    {
        connect(menu,&ContextMenu::compileTriggered,this,&FileView::compileFile);
        connect(menu,&ContextMenu::deleteTriggered,this,&FileView::deleteFileSlots);
        connect(menu,&ContextMenu::importTriggered,this,&FileView::importContractSlots);
        menu->exec(QCursor::pos());
    }

}

void FileView::retranslator()
{
    //ui->retranslateUi(this);
}
