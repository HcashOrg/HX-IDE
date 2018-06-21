#include "MainWindow.h"
#include <QApplication>
#include <QTextCodec>

int main(int argc, char *argv[])
{
    //QTextCodec::setCodecForLocale(QTextCodec::codecForName("system"));
    QApplication a(argc, argv);
    MainWindow w;
//    w.show();

    return a.exec();
}
