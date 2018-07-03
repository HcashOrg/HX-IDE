#ifndef FILEWIDGET_H
#define FILEWIDGET_H

#include <QWidget>

namespace Ui {
class FileWidget;
}

class FileWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FileWidget(QWidget *parent = 0);
    ~FileWidget();
public:
    QString getCurrentFile()const;
signals:
    void fileClicked(QString filepath);

    void compileFile();
    void deleteFile();
    void importContract();
public slots:
    void SelectFile(const QString &filePath);

    void retranslator();
private:
    void InitWidget();
private:
    Ui::FileWidget *ui;
};

#endif // FILEWIDGET_H
