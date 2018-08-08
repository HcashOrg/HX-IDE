#include <QApplication>
#include <QDebug>
#include "UpdateWidget.h"
#include "DataUtil.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //qInstallMessageHandler(DataUtil::myMessageOutput);
    qDebug()<<argc;
    for(int i = 0;i < argc;++i)
    {
        qDebug()<<argv[i];
    }
    if(3 == argc)
    {
        UpdateWidget wi(argv[1],argv[2]);
        wi.show();
        return a.exec();
    }
    else if(4 == argc)
    {
        UpdateWidget wi(argv[1],argv[2],argv[3]);
        wi.show();
        return a.exec();
    }
    else if(5 == argc)
    {
        UpdateWidget wi(argv[1],argv[2],argv[3],argv[4]);
        wi.show();
        return a.exec();
    }
    else
    {
        UpdateWidget wi;
        wi.show();
        return a.exec();
    }
    return 0;
}
