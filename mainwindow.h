#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidget>
#include <QListWidget>
#include <QSplitter>
#include "DataDefine.h"
namespace Ui {
class MainWindow;
}

class NormalLogin;
class FileListWidget;
class InterfaceWidget;
class ContentWidget;
class OutputWidget;
class SelectPathWidget;
class WaitingForSync;
class TabBarWidget;
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void getAccountInfo();

    void getAllAccountsContract();

    void getAllScripts();   // 获取钱包中所有本地脚本

    void showFile(QString path);

    void showContract(QString address);

signals:
    void accountInfoUpdated();

    void contractInfoUpdated(QString addressOrName);

private slots:
    void jsonDataUpdated(QString id);
    void alreadyLogin();
    void startHXChain();
    void startFormalHXChain();
    void contractRegistered(QString path);
    void scriptAdded(QString path);
    void fileSelected(QString path);
    void contractDestroyed(QString address);
    void scriptRemoved(QString scriptId);
    void formalWalletLogin();   // 第一次切换到正式链 open unlock 后
    void autoRefresh();
    void currentAccountChanged(QString account);
    void readTestExe();
    void readFormalExe();

    void on_newContractAction_triggered();
    void on_newScriptAction_triggered();
    void on_saveAction_triggered();
//    void on_accountAction_triggered();
    void on_accountListAction_triggered();
    void on_consoleAction_triggered();
    void on_compileAction_triggered();
    void on_registerAction_triggered();
    void on_upgradeAction_triggered();
    void on_transferAction_triggered();
    void on_undoAction_triggered();
    void on_redoAction_triggered();
    void on_callAction_triggered();
    void on_withdrawAction_triggered();
    void contentWidgetTextChanged();
    void on_addScriptAction_triggered();
    void on_removeScriptAction_triggered();
    void on_bindAction_triggered();
    void on_enterSandboxAction_triggered();
    void on_exitSandboxAction_triggered();
    void on_importAction_triggered();
    void on_exportAction_triggered();
    void on_changeToFormalChainAction_triggered();
    void on_changeToTestChainAction_triggered();
    void on_transferToAccountAction_triggered();
    void on_saveAllAction_triggered();
    void on_closeAction_triggered();
    void on_exitAction_triggered();
    void on_helpAction_triggered();
private slots:
    void CompileFinishSlot(const QString &dstFilePath);
private:
    void SetIDETheme(DataDefine::ThemeStyle theme);
private:
    Ui::MainWindow *ui;

    NormalLogin* normalLogin;
    WaitingForSync* waitingForSync;

    FileListWidget* fileListWidget;
//    InterfaceWidget* interfaceWidget;
    OutputWidget* outputWidget;
    ContentWidget*    contentWidget;
    QSplitter*  horizontalSplitter;
//    QSplitter*  verticalSplitter1;
    QSplitter*  verticalSplitter2;

    bool formalWalletOpened;   // 正式链钱包是否打开过
    bool testImportDelegatesStarted;        // 是否开始导入代理账户了

    QTimer* timerForTestLaunch;
    bool formalExeLaunched;
    QTimer* timerForFormalLaunch;

    QTimer* timerForAutoRefresh;
    void startTimerForAutoRefresh();      // 自动刷新

    void showSelectPathWidget();
    void showWaitingForSyncWidget();

    void importTestDelegateAccounts();    // 如果是第一次启动测试链 执行脚本导入99个代理账户
    void contextMenuEvent(QContextMenuEvent* e)
    {
        return;
    }

    void checkActionEnable(QString path);  // 激活/置灰action
    void checkChangeChainActionEnable(int chainType);    // 激活/置灰changeChainAction   1：当前是测试链  2：当前是正式链
    void checkSandboxActionEnable(bool isInSandbox);     // 激活/置灰sandboxAction

    void specialOperationsWhenAutoRefresh();

    void actionSetIcon();

    void closeEvent(QCloseEvent* e);

private slots:
    void exeStartedSlots();
};

#endif // MAINWINDOW_H
