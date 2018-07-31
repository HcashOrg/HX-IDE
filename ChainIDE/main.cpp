#include "MainWindow.h"
#include "IDEUtil.h"
#include "ChainIDE.h"
#include "LocalPeer.h"

#include <QApplication>
#include <QTextCodec>

int main(int argc, char *argv[])
{
    LocalPeer loc;
    if(!loc.IsAlreadyRunning())
    {
        QApplication a(argc, argv);
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
