#include "aceeditor.h"
#include "ui_aceeditor.h"
#include "control/editorcontextmenu.h"
#include "bridge.h"
#include <QTimer>
#include <QUrl>
#include <QLayout>
#include <QDir>
#include <QDebug>
#include <QTextCodec>
#include <QtWebEngineWidgets/QWebEnginePage>       // HTML页面
#include <QtWebChannel/QWebChannel>          // C++和JS/HTML双向通信，代替了已淘汰的QtWebFrame的功能

#include <QClipboard>
#include <QMimeData>

AceEditor::AceEditor(QString path, QWidget *parent) :
    QWidget(parent),
    filePath(path),
    loaded(false),
    editable(true),
    ui(new Ui::AceEditor)
{
    ui->setupUi(this);

    QVBoxLayout* vbLayout = new QVBoxLayout;
    vbLayout->addWidget(ui->webView);
    setContentsMargins(0,0,0,25);
    this->setLayout( vbLayout);

    // 加载网页
    QDir dir("ace/editor.html");
    QEventLoop eventloop;
    ui->webView->page()->load(QUrl( "file:///" + dir.absolutePath()));

    QWebEnginePage *page = new QWebEnginePage(this);  // 定义一个page作为页面管理
    QWebChannel *channel = new QWebChannel(this);     // 定义一个channel作为和JS或HTML交互
    channel->registerObject("qtWidget",(QObject*)bridge::instance());
    page->setWebChannel(channel);                   // 把channel配置到page上，让channel作为其信使
    page->load(QUrl( "file:///" + dir.absolutePath()));                         // page上加载html路径
    ui->webView->setPage(page);                   // 建立page和UI上的webEngine的联系

    ui->webView->installEventFilter(this);


    connect(ui->webView, SIGNAL(loadFinished(bool)), &eventloop, SLOT(quit()));
    eventloop.exec();
    setSaved(true);
}

AceEditor::~AceEditor()
{
    delete ui;
}


void AceEditor::hide()
{
    QWidget::hide();

}

void AceEditor::show()
{
    QWidget::show();
}

void AceEditor::setText(QString text)
{
    text.replace("\n","\\n");
    text.replace("\'","\\'");    // 由于参数使用''包裹， \'需要显式的传入
    ui->webView->page()->runJavaScript( QString("editor.setValue('%1', 1);").arg(text));
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
    qDebug() << "AceEditor::setMode " << mode;
    ui->webView->page()->runJavaScript( QString("editor.session.setMode('ace/mode/%1')").arg(mode));
}

void AceEditor::setReadOnly(bool bIs)
{
    ui->webView->page()->runJavaScript( QString("setReadOnly(%1)").arg(bIs));
}

void AceEditor::insert(QString text)
{
    text.replace("\\","\\\\");
    text.replace("\n","\\n");
    text.replace("\'","\\'");    // 由于参数使用''包裹， \'需要显式的传入

    ui->webView->page()->runJavaScript( QString("insert('%1')").arg(text));
}

void AceEditor::removeLines(int firstRow, int lastRow)
{
    ui->webView->page()->runJavaScript( QString("removeLines(%1,%2)").arg(firstRow).arg(lastRow));
}

void AceEditor::moveCursorTo(int row, int column)
{
    ui->webView->page()->runJavaScript( QString("editor.moveCursorTo(%1,%2)").arg(row).arg(column));
}

void AceEditor::initFinish()
{
    // calling reset immediately doesn't work, because changes are added to undoManager after a timeout
    ui->webView->page()->runJavaScript( "setTimeout(function() { editor.getSession().getUndoManager().reset(); inited = true;}, 700);");

    setSaved(true);
}

void AceEditor::undo()
{
    ui->webView->page()->runJavaScript( "editor.session.getUndoManager().undo()");
}

void AceEditor::redo()
{
    ui->webView->page()->runJavaScript( "editor.session.getUndoManager().redo()");
}

void AceEditor::cut()
{
    copy();
    deleteText();
}

bool AceEditor::isUndoAvailable()
{
    return this->syncRunJavaScript("editor.session.getUndoManager().hasUndo();").toBool();
}

bool AceEditor::isRedoAvailable()
{
    return this->syncRunJavaScript("editor.session.getUndoManager().hasRedo();").toBool();
}

bool AceEditor::isEditable()
{
    return editable;
}

void AceEditor::setEditable(bool bIs)
{
    editable = bIs;
}

void AceEditor::setSaved(bool isSaved)
{
    hasSaved = isSaved;
}

bool AceEditor::isSaved()
{
    if( isEditable())
    {
        return hasSaved;
    }
    else
    {
        return true;
    }

}

void AceEditor::setInitTemplate()
{
    QString text(R"SMARTCONTRACT(
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using static UvmCoreLib.UvmCoreFuncs;
using UvmCoreLib;

// This is a demo. Please remove the unused code
// and add your customized code.
namespace DemoContract1
{
    public class Storage
    {
        public string Name { get; set; }
        public int Age { get; set; }
        public string Country; // field test
        public bool IsMale { get; set; }
        public UvmArray<string> ArrayDemo { get; set; }
    }

    public class MyEventEmitteer : IUvmEventEmitter
    {
        public static void EmitHello(string eventArg)
        {
            Console.WriteLine("event Hello emited, arg is " + eventArg);
        }
        public static void EmitHello2(string eventArg)
        {
            Console.WriteLine("event Hello2 emited, arg is " + eventArg);
        }
    }

    public class MyContract : UvmContract<Storage>
    {
        public MyContract() : base(new Storage())
        {
        }
        public override void init()
        {
            print("contract initing");
            this.storage.Age = 100;
            this.storage.Country = "China";
            this.storage.Name = "C#";
            this.storage.IsMale = true;
            this.storage.ArrayDemo = UvmArray<string>.Create();
            this.storage.ArrayDemo.Add("hello");
            pprint(this);
            print("this is contract init api");
        }
        public string GetAge(string arg)
        {
            print("this is contract getAge api");
            return "" + this.storage.Age;
        }
        public string OfflineGetAge(string arg)
        {
            print("this is contract OfflineGetAge api");
            print("age is " + this.storage.Age);
            return "" + this.storage.Age;
        }
        public void TestHello(string arg)
        {
            print("this is contract hello api with argument " + arg);
        }
    }

    public class ExampleLibClass
    {
        public MyContract Main()
        {
            print("start of demo C# contract");
            var contract = new MyContract();
            print("end main");
            return contract;
        }
    }

}
)SMARTCONTRACT");

//    QTextCodec* gbkCodec = QTextCodec::codecForName("GBK");
    setText(text);
    initFinish();
}

void AceEditor::copy()
{
    QClipboard* cb = QApplication::clipboard();
    cb->setText(getSelectedText());

//   qDebug() << "cccccc " << ui->webView->page()->runJavaScript( "var target_obj = document.getElementsByClassName('ace_scroller')[0]; getComputedStyle(target_obj).backgroundColor").toString();

}

void AceEditor::paste()
{
    QClipboard* cb = QApplication::clipboard();
    insert( cb->text());
}

void AceEditor::deleteText()
{
    ui->webView->page()->runJavaScript( "var range = editor.selection.getRange();editor.session.remove(range);");
}

void AceEditor::selectAll()
{
    ui->webView->page()->runJavaScript( "editor.selection.selectAll()");
}

bool AceEditor::eventFilter(QObject *watched, QEvent *e)
{
    if(watched == ui->webView)
    {
        qDebug() << "event type: " << e->type();
        if(e->type() == QEvent::ContextMenu)
        {
            QString selectedText = getSelectedText();

            EditorContextMenu* menu = NULL;
            if( !this->isEditable())
            {
                // 如果不可编辑
                menu = new EditorContextMenu(false,false,false,!selectedText.isEmpty(),false,false);
            }
            else
            {
                const QClipboard* cb = QApplication::clipboard();
                const QMimeData *mimeData = cb->mimeData();
                menu = new EditorContextMenu(isUndoAvailable(),isRedoAvailable(),
                                             !selectedText.isEmpty(),!selectedText.isEmpty(),
                                             mimeData->hasText(),!selectedText.isEmpty());
            }

            connect(menu,SIGNAL(undoTriggered()),this,SLOT(undo()));
            connect(menu,SIGNAL(redoTriggered()),this,SLOT(redo()));
            connect(menu,SIGNAL(cutTriggered()),this,SLOT(cut()));
            connect(menu,SIGNAL(copyTriggered()),this,SLOT(copy()));
            connect(menu,SIGNAL(pasteTriggered()),this,SLOT(paste()));
            connect(menu,SIGNAL(deleteTriggered()),this,SLOT(deleteText()));
            connect(menu,SIGNAL(selectAllTriggered()),this,SLOT(selectAll()));

            menu->exec(QCursor::pos());
            delete menu;

            return true;
        }
    }

    return QWidget::eventFilter(watched,e);
}

QVariant AceEditor::syncRunJavaScript(const QString &javascript, int msec)
{
    QVariant result;
    QSharedPointer<QEventLoop> loop = QSharedPointer<QEventLoop>(new QEventLoop());
    QTimer::singleShot(msec, loop.data(), &QEventLoop::quit);
    qDebug() << "AceEditor::syncRunJavaScript: " << javascript;
    ui->webView->page()->runJavaScript(javascript, [loop, &result](const QVariant &val) {
        if (loop->isRunning()) {
            result = val;
            loop->quit();
        }
    });
    loop->exec();
    qDebug() << "result: " << result.toString();
    return result;
}
