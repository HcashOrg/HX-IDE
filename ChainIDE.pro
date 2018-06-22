#-------------------------------------------------
#
# Project created by QtCreator 2018-06-19T11:31:05
#
#-------------------------------------------------

QT       += core gui webengine webenginewidgets webchannel websockets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ChainIDE
TEMPLATE = app


SOURCES += main.cpp\
        MainWindow.cpp \
    ChainIDE.cpp \
    DataDefine.cpp \
    IDEUtil.cpp \
    ExeManager.cpp \
    compile/BaseCompile.cpp \
    compile/CompileManager.cpp \
    compile/gluaCompile.cpp \
    commondialog.cpp \
    selectpathwidget.cpp \
    waitingforsync.cpp \
    filewidget/FileWidget.cpp \
    filewidget/FileModel.cpp \
    filewidget/FileView.cpp \
    contentwidget/SingleContextWidget.cpp \
    contentwidget/contentwidget.cpp \
    outputwidget.cpp \
    contentwidget/aceeditor.cpp \
    control/contextmenu.cpp \
    control/editorcontextmenu.cpp \
    control/singletab.cpp \
    control/tabbarwidget.cpp \
    contentwidget/bridge.cpp \
    NewFileDialog.cpp \
    filewidget/InterfaceWidget.cpp \
    compile/javaCompile.cpp \
    compile/csharpCompile.cpp \
    consoledialog.cpp \
    datarequire/RequireBase.cpp\
    datarequire/DataRequireManager.cpp \
    datarequire/websocketRequire.cpp \
    datarequire/tcpsocketRequire.cpp \
    datarequire/httpRequire.cpp \
    contentwidget/ContextWidget.cpp

HEADERS  += MainWindow.h \
    ChainIDE.h \
    DataDefine.h \
    IDEUtil.h \
    ExeManager.h \
    compile/BaseCompile.h \
    compile/CompileManager.h \
    compile/gluaCompile.h \
    commondialog.h \
    selectpathwidget.h \
    waitingforsync.h \
    filewidget/FileWidget.h \
    filewidget/FileModel.h \
    filewidget/FileView.h \
    contentwidget/SingleContextWidget.h \
    contentwidget/contentwidget.h \
    outputwidget.h \
    contentwidget/aceeditor.h \
    control/contextmenu.h \
    control/editorcontextmenu.h \
    control/singletab.h \
    control/tabbarwidget.h \
    contentwidget/bridge.h \
    NewFileDialog.h \
    filewidget/InterfaceWidget.h \
    compile/javaCompile.h \
    compile/csharpCompile.h \
    consoledialog.h \
    datarequire/RequireBase.h\
    datarequire/DataRequireManager.h \
    datarequire/websocketRequire.h \
    datarequire/tcpsocketRequire.h \
    datarequire/httpRequire.h \
    contentwidget/ContextWidget.h

FORMS    += MainWindow.ui \
    commondialog.ui \
    selectpathwidget.ui \
    waitingforsync.ui \
    filewidget/FileWidget.ui \
    contentwidget/contentwidget.ui \
    outputwidget.ui \
    contentwidget/aceeditor.ui \
    control/singletab.ui \
    control/tabbarwidget.ui \
    NewFileDialog.ui \
    filewidget/InterfaceWidget.ui \
    consoledialog.ui \
    contentwidget/ContextWidget.ui

RESOURCES += \
    chain.qrc

QMAKE_CXXFLAGS_RELEASE = $$QMAKE_CFLAGS_RELEASE_WITH_DEBUGINFO
QMAKE_LFLAGS_RELEASE = $$QMAKE_LFLAGS_RELEASE_WITH_DEBUGINFO
