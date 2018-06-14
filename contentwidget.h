#ifndef CONTENTWIDGET_H
#define CONTENTWIDGET_H

#include <QWidget>
#include <QPlainTextEdit>
#include <QMap>

namespace Ui {
class ContentWidget;
}

class AceEditor;
class TabBarWidget;

class ContentWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ContentWidget(QWidget *parent = 0);
    ~ContentWidget();

    QMap<QString,AceEditor*>  pathAceEditorMap;   // 记录合约文件路径/链上合约地址 与窗口的对应关系

    QString currentFilePath;

    void undo();

    void redo();

    bool isUndoAvailable();

    bool isRedoAvailable();

    bool hasFileUnsaved();

    bool currentFileUnsaved();

    void closeSandboxFile();

public slots:
    void showFile(QString path);

    bool closeFile(QString path);       // 取消了就返回false

    bool closeAll();                    // 取消了就返回false

    void showContract(QString address);

    void saveFile();

    void saveFile(QString path);

    void saveAll();

    void onFileClosed(QString path);

    void onTabSelected(QString path);

    void onTextChanged();

    void onFileDeleted(QString path);

    void onChainChanged();

    void onAccountChanged();

    void contractInfoUpdated(QString address);

//    void contractRegistered(QString path);

//    void scriptAdded(QString path);

signals:
    void newFileSaved(QString oldPath, QString newPath);

    void textChanged();

    void fileSelected(QString path);

private:
    Ui::ContentWidget *ui;  
    TabBarWidget* tabBarWidget;

    void resizeEvent(QResizeEvent* size);
};

#endif // CONTENTWIDGET_H
