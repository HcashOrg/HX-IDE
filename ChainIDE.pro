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
    compile/BaseCompile.cpp \
    compile/CompileManager.cpp \
    compile/gluaCompile.cpp \
    popwidget/commondialog.cpp \
    selectpathwidget.cpp \
    waitingforsync.cpp \
    filewidget/FileWidget.cpp \
    filewidget/FileModel.cpp \
    filewidget/FileView.cpp \
    outputwidget.cpp \
    contentwidget/aceeditor.cpp \
    control/contextmenu.cpp \
    control/editorcontextmenu.cpp \
    contentwidget/bridge.cpp \
    popwidget/NewFileDialog.cpp \
    filewidget/InterfaceWidget.cpp \
    compile/javaCompile.cpp \
    compile/csharpCompile.cpp \
    popwidget/consoledialog.cpp \
    datarequire/RequireBase.cpp\
    datarequire/DataRequireManager.cpp \
    datarequire/websocketRequire.cpp \
    datarequire/tcpsocketRequire.cpp \
    datarequire/httpRequire.cpp \
    contentwidget/ContextWidget.cpp \
    backstage/BackStageBase.cpp \
    backstage/LinkBackStage.cpp \
    backstage/UbtcBackStage.cpp \
    DataManager.cpp \
    popwidget/AccountWidget.cpp \
    popwidget/namedialog.cpp \
    popwidget/MoveableDialog.cpp \
    popwidget/registercontractdialog.cpp \
    popwidget/TransferWidget.cpp \
    popwidget/CallContractWidget.cpp \
    ConvenientOp.cpp \
    contractwidget/ContractWidget.cpp \
    contractwidget/FunctionWidget.cpp \
    compile/kotlinCompile.cpp \
    popwidget/ConfigWidget.cpp \
    contentwidget/Editor.cpp \
    contentwidget/codeeditor.cpp

HEADERS  += MainWindow.h \
    ChainIDE.h \
    DataDefine.h \
    IDEUtil.h \
    compile/BaseCompile.h \
    compile/CompileManager.h \
    compile/gluaCompile.h \
    popwidget/commondialog.h \
    selectpathwidget.h \
    waitingforsync.h \
    filewidget/FileWidget.h \
    filewidget/FileModel.h \
    filewidget/FileView.h \
    outputwidget.h \
    contentwidget/aceeditor.h \
    control/contextmenu.h \
    control/editorcontextmenu.h \
    contentwidget/bridge.h \
    popwidget/NewFileDialog.h \
    filewidget/InterfaceWidget.h \
    compile/javaCompile.h \
    compile/csharpCompile.h \
    popwidget/consoledialog.h \
    datarequire/RequireBase.h\
    datarequire/DataRequireManager.h \
    datarequire/websocketRequire.h \
    datarequire/tcpsocketRequire.h \
    datarequire/httpRequire.h \
    contentwidget/ContextWidget.h \
    backstage/BackStageBase.h \
    backstage/LinkBackStage.h \
    backstage/UbtcBackStage.h \
    DataManager.h \
    popwidget/AccountWidget.h \
    popwidget/namedialog.h \
    popwidget/MoveableDialog.h \
    popwidget/registercontractdialog.h \
    popwidget/TransferWidget.h \
    popwidget/CallContractWidget.h \
    ConvenientOp.h \
    contractwidget/ContractWidget.h \
    contractwidget/FunctionWidget.h \
    compile/kotlinCompile.h \
    popwidget/ConfigWidget.h \
    contentwidget/Editor.h \
    contentwidget/codeeditor.h

FORMS    += MainWindow.ui \
    popwidget/commondialog.ui \
    selectpathwidget.ui \
    waitingforsync.ui \
    filewidget/FileWidget.ui \
    outputwidget.ui \
    contentwidget/aceeditor.ui \
    popwidget/NewFileDialog.ui \
    filewidget/InterfaceWidget.ui \
    popwidget/consoledialog.ui \
    contentwidget/ContextWidget.ui \
    popwidget/AccountWidget.ui \
    popwidget/namedialog.ui \
    popwidget/registercontractdialog.ui \
    popwidget/TransferWidget.ui \
    popwidget/CallContractWidget.ui \
    contractwidget/ContractWidget.ui \
    contractwidget/FunctionWidget.ui \
    popwidget/ConfigWidget.ui \
    contentwidget/Editor.ui \
    contentwidget/codeeditor.ui

RESOURCES += \
    chain.qrc

TRANSLATIONS +=   IDE_Simplified_Chinese.ts  IDE_English.ts


QMAKE_CXXFLAGS_RELEASE = $$QMAKE_CFLAGS_RELEASE_WITH_DEBUGINFO
QMAKE_LFLAGS_RELEASE = $$QMAKE_LFLAGS_RELEASE_WITH_DEBUGINFO

win32{
    RC_FILE += logo.rc
}
