#include "FileView.h"

#include <QCoreApplication>
#include <QDebug>
#include <QTimer>
#include <QHeaderView>
#include "FileModel.h"
#include "DataDefine.h"
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
    selectionModel()->clearSelection();
    selectionModel()->select(index,QItemSelectionModel::Select);
}

void FileView::IndexClicked(const QModelIndex &index)
{
    if(index.isValid())
    {
        QFileInfo file(_p->fileModel->filePath(index));
        if(file.isFile())
        {
            qDebug()<<_p->fileModel->filePath(index);
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
