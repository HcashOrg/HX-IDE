#ifndef FILEVIEW_H
#define FILEVIEW_H

#include <QTreeView>
class QModelIndex;
class QFileInfo;

class FileView : public QTreeView
{
    Q_OBJECT
public:
    explicit FileView(QWidget *parent = Q_NULLPTR);
    ~FileView();
signals:
    void fileClicked(QString filepath);

    void compileFile();
    void deleteFile();
    void importContract();
    void exportContract();
    void newFile(const QString &type,const QString &defalutPath = "");

public slots:
    void selectFile(const QString &filePath);

    void deleteFileSlots();
    void importContractSlots();
    void exportContractSlots();
    void newFileSlots();
    void retranslator();
private slots:
    void IndexClicked(const QModelIndex &index);
public:
    QString getCurrentFilePath()const;

private:
    void InitWidget();

    QString getFilePathFromIndex(const QModelIndex &index)const;
    QFileInfo getFileInfoFromIndex(const QModelIndex &index)const;
    QModelIndex getFileIndexFromPath(const QString &filePath)const;
    QModelIndex getProxyIndexFromPath(const QString &filePath)const;

protected:
    void contextMenuEvent(QContextMenuEvent *);
private:
    class DataPrivate;
    DataPrivate *_p;
};

#endif // FILEVIEW_H
