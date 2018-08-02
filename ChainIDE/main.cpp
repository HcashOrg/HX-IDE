#include "MainWindow.h"
#include "IDEUtil.h"
#include "ChainIDE.h"
#include "LocalPeer.h"

#include <QApplication>
#include <QTextCodec>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //判断是否是第一次启动
    LocalPeer loc;
    if(!loc.IsAlreadyRunning())
    {
        qInstallMessageHandler(IDEUtil::myMessageOutput);
        ChainIDE::getInstance();
        MainWindow w;
        return a.exec();
    }
    else
    {
        return 0;
    }
}
