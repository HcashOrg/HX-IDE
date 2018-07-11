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
    datamanager/DataManagerHX.cpp \
    datamanager/DataManagerUB.cpp \
    custom/AccountWidgetHX.cpp \
    custom/NameDialogHX.cpp \
    popwidget/MoveableDialog.cpp \
    custom/RegisterContractDialogHX.cpp \
    custom/TransferWidgetHX.cpp \
    custom/CallContractWidgetHX.cpp \
    ConvenientOp.cpp \
    contractwidget/ContractWidgetHX.cpp \
    contractwidget/FunctionWidgetHX.cpp \
    compile/kotlinCompile.cpp \
    popwidget/ConfigWidget.cpp \
    contentwidget/Editor.cpp \
    contentwidget/codeeditor.cpp \
    custom/PasswordVerifyWidgetHX.cpp \
    custom/ImportDialogHX.cpp \
    custom/AccountWidgetUB.cpp \
    custom/CallContractWidgetUB.cpp \
    custom/ImportDialogUB.cpp \
    custom/NameDialogUB.cpp \
    custom/RegisterContractDialogUB.cpp \
    custom/TransferWidgetUB.cpp \
    contractwidget/ContractWidgetUB.cpp \
    contractwidget/FunctionWidgetUB.cpp

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
    datamanager/DataManagerHX.h \
    datamanager/DataManagerUB.h \
    custom/AccountWidgetHX.h \
    custom/NameDialogHX.h \
    popwidget/MoveableDialog.h \
    custom/RegisterContractDialogHX.h \
    custom/TransferWidgetHX.h \
    custom/CallContractWidgetHX.h \
    ConvenientOp.h \
    contractwidget/ContractWidgetHX.h \
    contractwidget/FunctionWidgetHX.h \
    compile/kotlinCompile.h \
    popwidget/ConfigWidget.h \
    contentwidget/Editor.h \
    contentwidget/codeeditor.h \
    custom/PasswordVerifyWidgetHX.h \
    custom/ImportDialogHX.h \
    custom/AccountWidgetUB.h \
    custom/CallContractWidgetUB.h \
    custom/ImportDialogUB.h \
    custom/NameDialogUB.h \
    custom/RegisterContractDialogUB.h \
    custom/TransferWidgetUB.h \
    contractwidget/ContractWidgetUB.h \
    contractwidget/FunctionWidgetUB.h

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
    custom/AccountWidgetHX.ui \
    custom/NameDialogHX.ui \
    custom/RegisterContractDialogHX.ui \
    custom/TransferWidgetHX.ui \
    custom/CallContractWidgetHX.ui \
    contractwidget/ContractWidgetHX.ui \
    contractwidget/FunctionWidgetHX.ui \
    popwidget/ConfigWidget.ui \
    contentwidget/Editor.ui \
    contentwidget/codeeditor.ui \
    custom/PasswordVerifyWidgetHX.ui \
    custom/ImportDialogHX.ui \
    custom/AccountWidgetUB.ui \
    custom/CallContractWidgetUB.ui \
    custom/ImportDialogUB.ui \
    custom/NameDialogUB.ui \
    custom/RegisterContractDialogUB.ui \
    custom/TransferWidgetUB.ui \
    contractwidget/ContractWidgetUB.ui \
    contractwidget/FunctionWidgetUB.ui

RESOURCES += \
    chain.qrc

TRANSLATIONS +=   IDE_Simplified_Chinese.ts  IDE_English.ts


QMAKE_CXXFLAGS_RELEASE = $$QMAKE_CFLAGS_RELEASE_WITH_DEBUGINFO
QMAKE_LFLAGS_RELEASE = $$QMAKE_LFLAGS_RELEASE_WITH_DEBUGINFO

win32{
    RC_FILE += logo.rc
}
