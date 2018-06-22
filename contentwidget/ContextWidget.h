#ifndef CONTEXTWIDGET_H
#define CONTEXTWIDGET_H

#include <QWidget>

namespace Ui {
class ContextWidget;
}

class AceEditor;
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
private slots:
    void currentTabChanged(int i);
    void tabCloseRquest(int i);
private:
    bool saveFile(int i);
    bool closeFile(int i);

    int getTabNumber(const QString &path)const;
    const QString &getPathFromNumber(int i)const;
    AceEditor *getCurrentEditor()const;
    AceEditor *getEditor(int i)const;
    AceEditor *getEditor(const QString &path)const;

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
