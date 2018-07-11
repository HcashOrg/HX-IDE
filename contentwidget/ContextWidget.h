#ifndef CONTEXTWIDGET_H
#define CONTEXTWIDGET_H

#include <QWidget>

namespace Ui {
class ContextWidget;
}

class Editor;
class ContextWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ContextWidget(QWidget *parent = 0);
    ~ContextWidget();
public:
    void undo();

    void redo();

    bool isUndoAvailable();

    bool isRedoAvailable();

    bool hasFileUnsaved();

    bool currentFileUnsaved();

    const QString &getCurrentFilePath()const;

    void TabBreakPoint();

    void ClearBreakPoint();
signals:
    void textChanged();

    void fileSelected(QString path);

    void contentStateChange();
public slots:
    void showFile(QString path);

    bool closeFile(QString path);       // 取消了就返回false

    bool closeAll();                    // 取消了就返回false

    void saveFile();

    void saveFile(QString path);

    void saveAll();

    void onTextChanged();

    void gutterRightClickedSignal();

    void CheckDeleteFile();//文件被删除后使用，用于检查是否有被删除的文件正在被打开，是则关闭
private slots:
    void currentTabChanged(int i);
    void tabCloseRquest(int i);
private:
    bool saveFile(int i);
    bool closeFile(int i);

    int getTabNumber(const QString &path)const;
    const QString &getPathFromNumber(int i)const;
    Editor *getCurrentEditor()const;
    Editor *getEditor(int i)const;
    Editor *getEditor(const QString &path)const;

    void contextUpdate();
private:
    void InitWidget();
private:
    Ui::ContextWidget *ui;
private:
    class DataPrivate;
    DataPrivate *_p;
};

#endif // CONTEXTWIDGET_H
