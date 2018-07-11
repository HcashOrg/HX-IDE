#include "aceeditor.h"
#include "ui_aceeditor.h"

#include <QDir>
#include <QCoreApplication>
#include <QDebug>
#include <QtWebEngineWidgets/QWebEnginePage>       // HTML页面

#include <QClipboard>
#include <QMimeData>
#include "DataDefine.h"


//编辑器后缀转换
static const std::map<QString,QString> SUFFIX_MAP= {{DataDefine::CONTRACT_SUFFIX,"text"},{DataDefine::GLUA_SUFFIX,"glua"},
                                                    {DataDefine::JAVA_SUFFIX,"java"},{DataDefine::CSHARP_SUFFIX,"csharp"},
                                                    {DataDefine::KOTLIN_SUFFIX,"kotlin"}};

AceEditor::AceEditor(const QString &path, QWidget *parent) :
    Editor(path,QCoreApplication::applicationDirPath()+QDir::separator()+"ace/editor.html",parent),
    ui(new Ui::AceEditor)
{
    ui->setupUi(this);

    InitEditor();

    setTheme(true);
    setMode(SUFFIX_MAP.at(QFileInfo(path).suffix()));
    if(QFileInfo(path).suffix() == DataDefine::CONTRACT_SUFFIX)
    {
        setEditable(false);
    }

//设置是否可编辑、模式
    initFinish();
    setSaved(true);
}

AceEditor::~AceEditor()
{
    delete ui;
}

void AceEditor::checkState()
{
    if(getPage())
    {//判断是否可撤回、redo
        setSaved(false);
        getPage()->runJavaScript("editor.session.getUndoManager().hasUndo();",[this](const QVariant &val){
            this->setUndoAvaliable(val.toBool());
            emit stateChanged();
        });
        getPage()->runJavaScript("editor.session.getUndoManager().hasRedo();",[this](const QVariant &val){
            this->setRedoAvaliable(val.toBool());
            emit stateChanged();
        });

    }
}

void AceEditor::setText(QString text)
{
    text.replace("\n","\\n");
    text.replace("\'","\\'");    // 由于参数使用''包裹， \'需要显式的传入
    syncRunJavaScript( QString("editor.setValue('%1', 1);").arg(text));
}

QString AceEditor::getText()
{
    QString result = syncRunJavaScript("editor.getValue();").toString();
    result.replace('\\',"\\\\");
    return result;
}

QString AceEditor::getSelectedText()
{
    return this->syncRunJavaScript("editor.getSelectedText()").toString();
}

void AceEditor::setMode(QString mode)
{
//    qDebug() << "AceEditor::setMode " << mode;
    RunJavaScript( QString("editor.session.setMode('ace/mode/%1')").arg(mode));
}

void AceEditor::initFinish()
{
    // calling reset immediately doesn't work, because changes are added to undoManager after a timeout
    this->syncRunJavaScript( "setTimeout(function() { editor.getSession().getUndoManager().reset(); inited = true;}, 700);");
}

void AceEditor::setTheme(bool black)
{
    RunJavaScript( QString("setTheme(%1)").arg(black));
}

void AceEditor::undo()
{
    RunJavaScript( "editor.session.getUndoManager().undo()");
}

void AceEditor::redo()
{
    RunJavaScript( "editor.session.getUndoManager().redo()");
}

void AceEditor::cut()
{
    copy();
    deleteText();
}

void AceEditor::setEditable(bool bIs)
{
    Editor::setEditable(bIs);
    RunJavaScript( QString("setReadOnly(%1)").arg(!bIs));
}

void AceEditor::copy()
{
    QClipboard* cb = QApplication::clipboard();
    cb->setText(getSelectedText());
}

void AceEditor::paste()
{
    QClipboard* cb = QApplication::clipboard();
    QString text = cb->text();
    text.replace("\\","\\\\");
    text.replace("\n","\\n");
    text.replace("\'","\\'");    // 由于参数使用''包裹， \'需要显式的传入

    RunJavaScript( QString("insert('%1')").arg(text));
}

void AceEditor::deleteText()
{
    RunJavaScript( "var range = editor.selection.getRange();editor.session.remove(range);");
}

void AceEditor::selectAll()
{
    RunJavaScript( "editor.selection.selectAll()");
}

