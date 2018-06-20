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
signals:
    void fileClicked(QString filepath);

public slots:
    void SelectFile(const QString &filePath);
private:
    void InitWidget();
private:
    Ui::FileWidget *ui;
};

#endif // FILEWIDGET_H
