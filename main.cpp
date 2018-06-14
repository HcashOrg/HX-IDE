#include <windows.h>
#include <QDir>
#include <QApplication>
#include <QTranslator>
#include <QDebug>
#include "mainwindow.h"
#include "hxchain.h"
#include "DbgHelp.h"
#include "tchar.h"

LONG WINAPI TopLevelExceptionFilter(struct _EXCEPTION_POINTERS *pExceptionInfo)
{
    qDebug() << "Enter TopLevelExceptionFilter Function" ;
    HANDLE hFile = CreateFile(L"project.dmp",GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
    MINIDUMP_EXCEPTION_INFORMATION stExceptionParam;
    stExceptionParam.ThreadId    = GetCurrentThreadId();
    stExceptionParam.ExceptionPointers = pExceptionInfo;
    stExceptionParam.ClientPointers    = FALSE;
    MiniDumpWriteDump(GetCurrentProcess(),GetCurrentProcessId(),hFile,MiniDumpWithFullMemory,&stExceptionParam,NULL,NULL);
    CloseHandle(hFile);

    qDebug() << "End TopLevelExceptionFilter Function" ;
    return EXCEPTION_EXECUTE_HANDLER;
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QDir::setCurrent(QCoreApplication::applicationDirPath());

//    qInstallMessageHandler(outputMessage);
//    SetUnhandledExceptionFilter(TopLevelExceptionFilter);


//    QTranslator translator;
//    translator.load("language/ide_simplified_Chinese.qm");
//    a.installTranslator(&translator);

    QTranslator translatorForTextBrowser;   // QTextBrowser的右键菜单翻译
    translatorForTextBrowser.load("language/widgets.qm");
    a.installTranslator(&translatorForTextBrowser);

    QTranslator menuTranslator;
    menuTranslator.load(QString(":/qm/qt_zh_cn"));
    a.installTranslator(&menuTranslator);

    a.setStyleSheet("QScrollBar:vertical{width:13px;background:rgb(255,255,255);padding:19px 5px 19px 0px;}"
                    "QScrollBar::handle:vertical{width:8px;background:rgba(173,173,179);border-radius:4px;min-height:20;}"
                    "QScrollBar::handle:vertical:hover{background:rgb(130,137,143);}"
                    "QScrollBar::add-line:vertical{height:0px;}"
                    "QScrollBar::sub-line:vertical{height:0px;}"
                    "QScrollBar:horizontal{height:8px;background:rgb(255,255,255);padding:0px 19px 0px 19px;}"
                    "QScrollBar::handle:horizontal{height:8px;background:rgba(173,173,179);border-radius:4px;min-width:20;}"
                    "QScrollBar::handle:horizontal:hover{background:rgb(130,137,143);}"
                    "QScrollBar::add-line:horizontal{width:0px;}"
                    "QScrollBar::sub-line:horizontal{width:0px;}"
                    );

    HXChain::getInstance();
    MainWindow w;
   // w.show();

    int result = a.exec();
    HXChain::getInstance()->quit();

    return result;
}
