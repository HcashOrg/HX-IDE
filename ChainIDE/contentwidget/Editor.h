#ifndef EDITOR_H
#define EDITOR_H

#include <QWidget>

namespace Ui {
class Editor;
}
class QWebEnginePage;
class Editor : public QWidget
{
    Q_OBJECT
public:
    explicit Editor(const QString &path,const QString &htmlPath = "",QWidget *parent = 0);
    virtual ~Editor();
signals:
    void stateChanged();
public:
    virtual void checkState() = 0;

    virtual void setText(QString text) = 0;
    virtual QString getText() = 0;

    virtual QString getSelectedText() = 0;

    virtual void TabBreakPoint();
    virtual void ClearBreakPoint();

    bool isUndoAvailable();
    bool isRedoAvailable();

    bool isEditable()const;
    virtual void setEditable(bool);

    void setSaved(bool isSaved) ;
    bool isSaved()const;

    bool saveFile() ;
    const QString &getFilePath()const;
protected:
    void setUndoAvaliable(bool);
    void setRedoAvaliable(bool);
public slots:
    virtual void undo() = 0;
    virtual void redo() = 0;
    virtual void cut() = 0;
    virtual void copy() = 0;
    virtual void paste() = 0;
    virtual void deleteText() = 0;
    virtual void selectAll() = 0;
protected:
    void InitEditor();
    void openFile(const QString &filePath);

protected:
    bool eventFilter(QObject *watched, QEvent *e);
    QWebEnginePage *getPage()const;
    QVariant syncRunJavaScript(const QString &javascript, int msec = 3000);
    void RunJavaScript(const QString &scriptSource);
private:
    Ui::Editor *ui;
    class DataPrivate;
    DataPrivate *_p;
};

#endif // EDITOR_H
