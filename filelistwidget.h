#ifndef FILELISTWIDGET_H
#define FILELISTWIDGET_H

#include <QWidget>
#include <QSplitter>

class InterfaceWidget;

namespace Ui {
class FileListWidget;
}

class QTreeWidgetItem;

class FileListWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FileListWidget(QWidget *parent = 0);
    ~FileListWidget();

    void getContractsList();
    void getForeverContracts();
    void getScriptsList();
    void getAddedScripts();

    void addFile( int tabIndex, int index, QString fileName);
    void treeWidgetSetCurrentItem( int tabIndex, int index, QString text);
    void setSelectionEmpty();       // 如果文件都关闭了 当前选择清空

    void setCurrentIndex(int index);
    int currentIndex;   // 合约=0  脚本=1

    InterfaceWidget* interfaceWidget;
    QTreeWidgetItem* contractItem;
    QTreeWidgetItem* registeredItem;
    QTreeWidgetItem* onChainItem;
    QTreeWidgetItem* scriptItem;
    QTreeWidgetItem* addedScriptItem;

signals:
    void showFile(QString path);

    void showContract(QString address);   // 显示链上合约

    void newContractFile();
    void newScriptFile();

    void fileDeleted(QString path);

    void importTriggered();
    void exportTriggered();
    void compileTriggered();
    void registerContractTriggered();
    void callTriggered();
    void upgradeTriggered();
    void withdrawTriggered();
    void transferTriggered();
    void addScriptTriggered();
    void removeTriggered();
    void bindTriggered();

public slots:
    void getRegisteredContracts();

    void newFileSaved(QString oldPath, QString newPath);

    void contractInfoUpdated(QString addressOrName);

    QStringList treeWidgetItemGetAllChildrenText(QTreeWidgetItem* item);

private slots:
    void fileChanged(QTreeWidgetItem* item, int column);

    void scriptChanged(QTreeWidgetItem* item, int column);

    void on_tabWidget_currentChanged(int index);

    void jsonDataUpdated(QString id);

    void treeWidgetItemRemoveAllChildren(QTreeWidgetItem* item);

    void on_fileTreeWidget_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);

    void on_scriptsTreeWidget_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);

    void on_contractBtn_clicked();

    void on_scriptBtn_clicked();

    void on_fileTreeWidget_customContextMenuRequested(const QPoint &pos);

    void on_scriptsTreeWidget_customContextMenuRequested(const QPoint &pos);

    void copy();

    void paste();

    void deleteFile();

private:
    Ui::FileListWidget *ui;
    QSplitter* verticalSplitter;

    bool fileUpdating;   // 防止刷新的时候 触发fileChanged
    bool scriptUpdating;   // 防止刷新的时候 触发scriptChanged

};

#endif // FILELISTWIDGET_H
