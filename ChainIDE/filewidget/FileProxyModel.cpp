#include "FileProxyModel.h"

#include <QFileSystemModel>
#include <QCoreApplication>
#include "DataDefine.h"

FileProxyModel::FileProxyModel(QObject *parent)
    :QSortFilterProxyModel(parent)
{

}

FileProxyModel::~FileProxyModel()
{

}

bool FileProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    QFileSystemModel *fileModel = dynamic_cast<QFileSystemModel*>(sourceModel());
    if(!fileModel) return true;

    QFileInfo fileInfo = fileModel->fileInfo(fileModel->index(source_row,0,source_parent));

    if(fileInfo.isDir())
    {
        if(source_parent == fileModel->index(fileModel->rootPath()) &&
           (fileInfo.absoluteFilePath().endsWith(DataDefine::JAVA_DIR) || fileInfo.absoluteFilePath().endsWith(DataDefine::CSHARP_DIR) ||
            fileInfo.absoluteFilePath().endsWith(DataDefine::KOTLIN_DIR) || fileInfo.absoluteFilePath().endsWith(DataDefine::GLUA_DIR)))
        {
            return true;
        }
        else if(source_parent != fileModel->index(fileModel->rootPath()))
        {
            return true;
        }
    }
    else if(fileInfo.isFile() )
    {
        if(fileInfo.suffix() == DataDefine::GLUA_SUFFIX || fileInfo.suffix() == DataDefine::JAVA_SUFFIX ||
           fileInfo.suffix() == DataDefine::KOTLIN_SUFFIX || fileInfo.suffix() == DataDefine::CSHARP_SUFFIX ||
           fileInfo.suffix() == DataDefine::CONTRACT_SUFFIX)
        {
            return true;
        }
    }
    return false;
}

bool FileProxyModel::filterAcceptsColumn(int source_column, const QModelIndex &source_parent) const
{
    if(0 == source_column)
    {
        return true;
    }
    return false;
}
