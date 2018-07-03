#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "DataDefine.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
private slots:
    void exeStartedSlots();

    //文件菜单
    void on_newContractAction_glua_triggered();

    void on_newContractAction_csharp_triggered();

    void on_newContractAction_java_triggered();

    void on_newContractAtion_kotlin_triggered();

    void on_saveAction_triggered();

    void on_savaAsAction_triggered();

    void on_saveAllAction_triggered();

    void on_closeAction_triggered();

    void on_exitAction_triggered();

    //编辑菜单
    void on_undoAction_triggered();

    void on_redoAction_triggered();

    //合约菜单
    void on_importAction_triggered();

    void on_exportAction_triggered();

    void on_registerAction_triggered();

    void on_transferAction_triggered();

    void on_callAction_triggered();

    void on_upgradeAction_triggered();

    void on_withdrawAction_triggered();

    //调试菜单
    void on_compileAction_triggered();

    void on_debugAction_triggered();

    void on_stopAction_triggered();

    void on_stepAction_triggered();
    //工具菜单
    void on_changeChainAction_triggered();

    void on_enterSandboxAction_triggered();

    void on_exitSandboxAction_triggered();

    void on_accountListAction_triggered();

    void on_consoleAction_triggered();

    void on_transferToAccountAction_triggered();

    //帮助菜单
    void on_helpAction_triggered();

    void on_aboutAction_triggered();

    void on_closeAllAction_triggered();

    void on_tabWidget_currentChanged(int index);


    void ModifyActionState();
private:
    void NewFileCreated(const QString &filePath);
private:
    void InitWidget();
    void showSelectPathWidget();
    void startChain();
    void showWaitingForSyncWidget();

    void startWidget();
private:
    void refreshTitle();
    void refreshStyle();
    void refreshTranslator();
private:
    Ui::MainWindow *ui;
    class DataPrivate;
    DataPrivate *_p;
protected:
    void closeEvent(QCloseEvent *event);
};

#endif // MAINWINDOW_H
