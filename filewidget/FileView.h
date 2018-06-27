#ifndef FILEVIEW_H
#define FILEVIEW_H

#include <QTreeView>

class FileView : public QTreeView
{
    Q_OBJECT
public:
    explicit FileView(QWidget *parent = Q_NULLPTR);
    ~FileView();
signals:
    void fileClicked(QString filepath);
public slots:
    void selectFile(const QString &filePath);
private slots:
    void IndexClicked(const QModelIndex &index);
public:
    QString getCurrentFilePath()const;
private:
    void InitWidget();
private:
    class DataPrivate;
    DataPrivate *_p;
};

#endif // FILEVIEW_H
