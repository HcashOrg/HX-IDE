#ifndef SINGLETAB_H
#define SINGLETAB_H

#include <QWidget>

namespace Ui {
class SingleTab;
}

class SingleTab : public QWidget
{
    Q_OBJECT

public:
    explicit SingleTab(QString name, QString path, QWidget *parent = 0);
    ~SingleTab();

    void setSelected(bool isSelected);

    void setSaved(bool hasSaved);

    void modifyPath(QString newPath);

    bool saved;     // 是否保存了

signals:
    void closeFile(QString path);

    void tabSelected(QString path);

private slots:
    void on_closeBtn_clicked();

    void on_nameBtn_clicked();

    void on_savedBtn_clicked();

private:
    Ui::SingleTab *ui;
    QString fileName;
    QString filePath;

    void paintEvent(QPaintEvent*);
    void mousePressEvent(QMouseEvent*);
};

#endif // SINGLETAB_H
