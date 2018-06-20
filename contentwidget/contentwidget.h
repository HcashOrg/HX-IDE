#ifndef CONTENTWIDGET_H
#define CONTENTWIDGET_H

#include <QWidget>
#include <QPlainTextEdit>
#include <QMap>

namespace Ui {
class ContentWidget;
}

class AceEditor;

class ContentWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ContentWidget(QWidget *parent = 0);
    ~ContentWidget();

    void undo();

    void redo();

    bool isUndoAvailable();

    bool isRedoAvailable();

    bool hasFileUnsaved();

    bool currentFileUnsaved();

    void closeSandboxFile();

    const QString &getCurrentFilePath()const;
    const QMap<QString,AceEditor*> &getContentMap()const;
public slots:
    void showFile(QString path);

    bool closeFile(QString path);       // 取消了就返回false

    bool closeAll();                    // 取消了就返回false

    void saveFile();

    void saveFile(QString path);

    void saveAll();

    void onFileClosed(QString path);

    void onTabSelected(QString path);

    void onTextChanged();

    void onFileDeleted(QString path);

signals:
    void textChanged();

    void fileSelected(QString path);

    void contentStateChange();
private:
    QMap<QString,AceEditor*>  pathAceEditorMap;   // 记录合约文件路径/链上合约地址 与窗口的对应关系

    QString currentFilePath;

private:
    Ui::ContentWidget *ui;
};

#endif // CONTENTWIDGET_H
