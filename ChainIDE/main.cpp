#include <QApplication>

#include "IDEUtil.h"
#include "LocalPeer.h"
#include "ChainIDE.h"
#include "MainWindow.h"

int main(int argc, char *argv[])
{
    //判断是否是第一次启动
    QApplication a(argc, argv);
    LocalPeer loc;
    if(!loc.IsAlreadyRunning())
    {
        //安装debug导出函数
        qInstallMessageHandler(IDEUtil::myMessageOutput);
        //初始化单例配置等
        ChainIDE::getInstance();
        //主窗口
        MainWindow w;
        QObject::connect(&loc,&LocalPeer::newConnection,&w,&MainWindow::activateWindow);
        return a.exec();

    }
    else
    {
        return 0;
    }
}
