#ifndef FILEMODEL_H
#define FILEMODEL_H

#include <QFileSystemModel>

class FileModel : public QFileSystemModel
{
    Q_OBJECT
public:
    explicit FileModel(QObject *parent = Q_NULLPTR);
};

#endif // FILEMODEL_H
