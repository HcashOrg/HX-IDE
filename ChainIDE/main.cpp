#include "MainWindow.h"
#include "ChainIDE.h"
#include <QApplication>
#include <QTextCodec>
#include <QDateTime>
#include <QFile>
#include <QFileInfo>
#include <QMutex>

void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    // 加锁
    static QMutex mutex;
    mutex.lock();

    QByteArray localMsg = msg.toLocal8Bit();

    QString strMsg("");
    switch(type)
    {
    case QtDebugMsg:
        strMsg = QString("Debug:");
        break;
    case QtWarningMsg:
        strMsg = QString("Warning:");
        break;
    case QtCriticalMsg:
        strMsg = QString("Critical:");
        break;
    case QtFatalMsg:
        strMsg = QString("Fatal:");
        break;
    }

    // 设置输出信息格式
    QString strDateTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss ddd");
    QString strMessage = QString("DateTime:%1 Message:%2").arg(strDateTime).arg(localMsg.constData());

    // 输出信息至文件中（读写、追加形式）
    QFileInfo info("log.txt");
    if(info.size() > 1024*1024*50) QFile::remove("log.txt");
    QFile file("log.txt");
    file.open(QIODevice::ReadWrite | QIODevice::Append);
    QTextStream stream(&file);
    stream << strMessage << "\r\n";
    file.flush();
    file.close();

    // 解锁
    mutex.unlock();
}

int main(int argc, char *argv[])
{
    //QTextCodec::setCodecForLocale(QTextCodec::codecForName("system"));
    //qInstallMessageHandler(myMessageOutput);
    QApplication a(argc, argv);
    ChainIDE::getInstance();
    MainWindow w;
//    w.show();

    return a.exec();
}
