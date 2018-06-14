#ifndef TABBARWIDGET_H
#define TABBARWIDGET_H

#include <QWidget>
#include <QMap>
#include <QVector>

namespace Ui {
class TabBarWidget;
}

class SingleTab;

class TabBarWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TabBarWidget(QWidget *parent = 0);
    ~TabBarWidget();

    QMap<QString,SingleTab*>    pathTabMap;  // 保存地址对应的tab
    QVector<QString>            tabVector;   // 保存tab的顺序

    QString getCurrentPath();

    void addTab(QString name, QString path);

    void removeTab(QString path);

    QString getNextTab(QString currentTab);

    void modifyTabPath(QString oldPath, QString newPath);

public slots:
    void setCurrentPath(QString path);

    void onTextChanged();

    void onFileSaved(QString path);

signals:
    void fileClosed(QString path);

    void tabSelected(QString path);

private slots:
    void onCloseFile(QString path);

    void onSelectTab(QString path);

    void on_leftBtn_clicked();

    void on_rightBtn_clicked();

private:
    Ui::TabBarWidget *ui;
    QString currentPath;
    int currentIndex;

    void resizeEvent(QResizeEvent* size);
    void adjustTabs();      // 根据当前tabbarwidget的尺寸和tab的数量调整显示多少个tab
    void forceCurrentTabShow();     // 调整currentIndex 直到当前tab显示出来
};

#endif // TABBARWIDGET_H
