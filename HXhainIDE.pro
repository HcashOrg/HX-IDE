#-------------------------------------------------
#
# Project created by QtCreator 2016-09-21T16:05:40
#
#-------------------------------------------------

#lessThan(QT_VERSION, 0x050600)
#{
QT += core gui network webengine webenginewidgets webchannel websockets
#}
#greaterThan(QT_VERSION, 0x050500)
#{
#QT += core gui network webenginewidgets webengine
#}

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = HXChainIDE
TEMPLATE = app

LIBS += -lDbgHelp
QMAKE_CXXFLAGS_RELEASE = $$QMAKE_CFLAGS_RELEASE_WITH_DEBUGINFO
QMAKE_LFLAGS_RELEASE = $$QMAKE_LFLAGS_RELEASE_WITH_DEBUGINFO

SOURCES += main.cpp\
        mainwindow.cpp \
    workerthread.cpp \
    workerthreadmanager.cpp \
    normallogin.cpp \
    rpcthread.cpp \
    filelistwidget.cpp \
    interfacewidget.cpp \
    contentwidget.cpp \
    outputwidget.cpp \
    firstlogin.cpp \
    waitingforsync.cpp \
    dialog/accountlistdialog.cpp \
    dialog/consoledialog.cpp \
    dialog/namedialog.cpp \
    dialog/registercontractdialog.cpp \
    dialog/transferdialog.cpp \
    dialog/upgradedialog.cpp \
    dialog/calldialog.cpp \
    dialog/withdrawdialog.cpp \
    dialog/addscriptdialog.cpp \
    dialog/binddialog.cpp \
    dialog/removescriptdialog.cpp \
    aceeditor.cpp \
    control/singletab.cpp \
    control/tabbarwidget.cpp \
    control/contextmenu.cpp \
    dialog/transfertoaccountdialog.cpp \
    control/editorcontextmenu.cpp \
    outputmessage.cpp \
    dialog/importdialog.cpp \
    hxchain.cpp \
    selectpathwidget.cpp \
    bridge.cpp \
    websocketmanager.cpp \
    ExeManager.cpp \
    commondialog.cpp

HEADERS  += mainwindow.h \
    workerthread.h \
    workerthreadmanager.h \
    normallogin.h \
    rpcthread.h \
    filelistwidget.h \
    interfacewidget.h \
    contentwidget.h \
    outputwidget.h \
    firstlogin.h \
    waitingforsync.h \
    dialog/accountlistdialog.h \
    dialog/consoledialog.h \
    dialog/namedialog.h \
    dialog/registercontractdialog.h \
    dialog/transferdialog.h \
    dialog/upgradedialog.h \
    dialog/calldialog.h \
    dialog/withdrawdialog.h \
    dialog/addscriptdialog.h \
    dialog/binddialog.h \
    sandboxcommandmap.h \
    dialog/removescriptdialog.h \
    aceeditor.h \
    control/singletab.h \
    control/tabbarwidget.h \
    control/contextmenu.h \
    dialog/transfertoaccountdialog.h \
    control/editorcontextmenu.h \
    outputmessage.h \
    dialog/importdialog.h \
    hxchain.h \
    selectpathwidget.h \
    bridge.h \
    websocketmanager.h \
    ExeManager.h \
    commondialog.h

FORMS    += mainwindow.ui \
    normallogin.ui \
    filelistwidget.ui \
    interfacewidget.ui \
    contentwidget.ui \
    outputwidget.ui \
    firstlogin.ui \
    waitingforsync.ui \
    dialog/accountlistdialog.ui \
    dialog/consoledialog.ui \
    dialog/namedialog.ui \
    dialog/registercontractdialog.ui \
    dialog/transferdialog.ui \
    dialog/upgradedialog.ui \
    dialog/calldialog.ui \
    dialog/withdrawdialog.ui \
    dialog/addscriptdialog.ui \
    dialog/binddialog.ui \
    dialog/removescriptdialog.ui \
    aceeditor.ui \
    control/singletab.ui \
    control/tabbarwidget.ui \
    dialog/transfertoaccountdialog.ui \
    dialog/importdialog.ui \
    selectpathwidget.ui \
    commondialog.ui
    
RESOURCES += \
    hxchain.qrc

TRANSLATIONS +=   ide_simplified_Chinese.ts  ide_English.ts
