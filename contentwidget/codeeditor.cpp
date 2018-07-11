#include "codeeditor.h"
#include "ui_codeeditor.h"

#include <QDir>
#include <QCoreApplication>
#include <QDebug>
#include <QtWebEngineWidgets/QWebEnginePage>       // HTML页面

#include <QClipboard>
#include <QMimeData>
#include "DataDefine.h"

static const std::map<QString,QString> SUFFIX_MAP= {{DataDefine::CONTRACT_SUFFIX,"lua"},{DataDefine::GLUA_SUFFIX,"lua"},
                                                    {DataDefine::JAVA_SUFFIX,"java"},{DataDefine::CSHARP_SUFFIX,"csharp"},
                                                    {DataDefine::KOTLIN_SUFFIX,"kotlin"}};

codeeditor::codeeditor(const QString &path,QWidget *parent) :
    Editor(path,QCoreApplication::applicationDirPath()+QDir::separator()+"codemirror/editor.html",parent),
    ui(new Ui::codeeditor)
{
    ui->setupUi(this);
    InitEditor();

    setMode(SUFFIX_MAP.at(QFileInfo(path).suffix()));
    if(QFileInfo(path).suffix() == DataDefine::CONTRACT_SUFFIX)
    {
        setEditable(false);
    }
    initFinish();
    setSaved(true);
}

codeeditor::~codeeditor()
{
    delete ui;
}

void codeeditor::checkState()
{
    if(getPage())
    {
        getPage()->runJavaScript("getHistory(0)",[this](const QVariant &val){
            this->setUndoAvaliable(val.toBool());
            emit stateChanged();
        });
        getPage()->runJavaScript("getHistory(1)",[this](const QVariant &val){
            this->setRedoAvaliable(val.toBool());
            emit stateChanged();
        });
    }


}

void codeeditor::setText(QString text)
{
    text.replace("\n","\\n");
    text.replace("\'","\\'");    // 由于参数使用''包裹， \'需要显式的传入
    syncRunJavaScript(QString("editor.setValue('%1')").arg(text));

}

QString codeeditor::getText()
{
    QString val = syncRunJavaScript("editor.getValue()").toString();
    val.replace("\\","\\\\");
    return val;
}

QString codeeditor::getSelectedText()
{
    QString val = syncRunJavaScript("editor.getSelection()").toString();
    val.replace('\\',"\\\\");
    return val;
}

void codeeditor::setMode(QString mode)
{
    syncRunJavaScript(QString("editor.setOption('mode','text/x-%1')").arg(mode));
}

void codeeditor::initFinish()
{
    // calling reset immediately doesn't work, because changes are added to undoManager after a timeout
    this->syncRunJavaScript( "setTimeout(function() { editor.clearHistory(); inited = true;}, 700);");

}

void codeeditor::setTheme(bool black)
{

}

void codeeditor::setEditable(bool bIs)
{
    Editor::setEditable(bIs);

    RunJavaScript( QString("setReadOnly('%1')").arg(!bIs));
}

void codeeditor::TabBreakPoint()
{
    RunJavaScript("TabBreakpoint()");
}

void codeeditor::ClearBreakPoint()
{
    RunJavaScript("ClearBreakpoint()");
}

void codeeditor::undo()
{
    RunJavaScript("editor.undo()");
}

void codeeditor::redo()
{
    RunJavaScript("editor.redo()");
}

void codeeditor::cut()
{
    copy();
    deleteText();

}

void codeeditor::copy()
{
    QClipboard* cb = QApplication::clipboard();
    cb->setText(getSelectedText());
}

void codeeditor::paste()
{
    QClipboard* cb = QApplication::clipboard();
    QString text = cb->text();
    text.replace("\\","\\\\");
    text.replace("\n","\\n");
    text.replace("\'","\\'");    // 由于参数使用''包裹， \'需要显式的传入
    RunJavaScript(QString("editor.replaceSelection('%1')").arg(text));
}

void codeeditor::deleteText()
{
    RunJavaScript("editor.replaceSelection('')");
}

void codeeditor::selectAll()
{
    qDebug()<<"selectall";
    RunJavaScript("editor.selectAll()");
}
