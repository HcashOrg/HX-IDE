#include "aceeditor.h"
#include "ui_aceeditor.h"
#include "control/editorcontextmenu.h"
#include "bridge.h"
#include <QTimer>
#include <QUrl>
#include <QLayout>
#include <QDir>
#include <QCoreApplication>
#include <QDebug>
#include <QTextCodec>
#include <QWebEngineView>
#include <QtWebEngineWidgets/QWebEnginePage>       // HTML页面
#include <QtWebChannel/QWebChannel>          // C++和JS/HTML双向通信，代替了已淘汰的QtWebFrame的功能

#include <QClipboard>
#include <QMimeData>
#include "DataDefine.h"

class AceEditor::DataPrivate
{
public:
    DataPrivate(QString path,bool iseditable)
        :filePath(path),editable(iseditable),webView(nullptr)
    {

    }
public:
    bool editable;
    QString filePath;

    bool hasSaved;
    QWebEngineView *webView;
};

AceEditor::AceEditor(QString path, bool iseditable,QWidget *parent) :
    QWidget(parent),
    _p(new DataPrivate(path,iseditable)),
    ui(new Ui::AceEditor)
{
    ui->setupUi(this);

    initEditor();
    openFile(path);

    initFinish();
    setSaved(true);
}

AceEditor::~AceEditor()
{
    delete _p;
    delete ui;
}

void AceEditor::setText(QString text)
{
    text.replace("\n","\\n");
    text.replace("\'","\\'");    // 由于参数使用''包裹， \'需要显式的传入
    this->syncRunJavaScript( QString("editor.setValue('%1', 1);").arg(text));
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
    this->syncRunJavaScript( QString("editor.session.setMode('ace/mode/%1')").arg(mode));
}

void AceEditor::insert(QString text)
{
    text.replace("\\","\\\\");
    text.replace("\n","\\n");
    text.replace("\'","\\'");    // 由于参数使用''包裹， \'需要显式的传入

    this->syncRunJavaScript( QString("insert('%1')").arg(text));
}

void AceEditor::removeLines(int firstRow, int lastRow)
{
    this->syncRunJavaScript( QString("removeLines(%1,%2)").arg(firstRow).arg(lastRow));
}

void AceEditor::moveCursorTo(int row, int column)
{
    this->syncRunJavaScript( QString("editor.moveCursorTo(%1,%2)").arg(row).arg(column));
}

void AceEditor::initFinish()
{
    // calling reset immediately doesn't work, because changes are added to undoManager after a timeout
    this->syncRunJavaScript( "setTimeout(function() { editor.getSession().getUndoManager().reset(); inited = true;}, 700);");
}

void AceEditor::setTheme(bool black)
{
    this->syncRunJavaScript( QString("setTheme(%1)").arg(black));
}

void AceEditor::undo()
{
    this->syncRunJavaScript( "editor.session.getUndoManager().undo()");
}

void AceEditor::redo()
{
    this->syncRunJavaScript( "editor.session.getUndoManager().redo()");
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
    return _p->editable;
}

void AceEditor::setEditable(bool bIs)
{
    syncRunJavaScript( QString("setReadOnly(%1)").arg(!bIs));
    _p->editable = bIs;
}

void AceEditor::setSaved(bool isSaved)
{
    _p->hasSaved = isSaved;
}

bool AceEditor::isSaved()
{
    if( isEditable())
    {
        return _p->hasSaved;
    }
    else
    {
        return true;
    }

}

bool AceEditor::saveFile()
{
    if(isSaved()) return true;

    QFile file(_p->filePath);
    if( file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
    {
        QTextStream out(&file);
        out << getText();
        file.close();
        setSaved(true);
        return true;
    }
    else
    {
        setSaved(false);
        return false;
    }
}

const QString &AceEditor::getFilePath() const
{
    return _p->filePath;
}

void AceEditor::copy()
{
    QClipboard* cb = QApplication::clipboard();
    cb->setText(getSelectedText());
}

void AceEditor::paste()
{
    QClipboard* cb = QApplication::clipboard();
    insert( cb->text());
}

void AceEditor::deleteText()
{
    this->syncRunJavaScript( "var range = editor.selection.getRange();editor.session.remove(range);");
}

void AceEditor::selectAll()
{
    this->syncRunJavaScript( "editor.selection.selectAll()");
}

bool AceEditor::eventFilter(QObject *watched, QEvent *e)
{
    if(dynamic_cast<QWebEngineView *>(watched))
    {
        //qDebug() << "event type: " << e->type();
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
    if(_p->webView)
    {
        _p->webView->page()->runJavaScript(javascript, [loop, &result](const QVariant &val) {
            if (loop->isRunning()) {
                result = val;
                loop->quit();
            }
        });
    }

    loop->exec();
    qDebug() << "result: " << result.toString();
    return result;
}

void AceEditor::initEditor()
{
    // 加载网页
    QDir dir(QCoreApplication::applicationDirPath()+QDir::separator()+"ace/editor.html");
    QEventLoop eventloop;

    QWebEngineView *webView = new QWebEngineView();
    QWebEnginePage *page = new QWebEnginePage(this);  // 定义一个page作为页面管理
    page->setBackgroundColor(Qt::transparent);
    QWebChannel *channel = new QWebChannel(this);     // 定义一个channel作为和JS或HTML交互
    channel->registerObject("qtWidget",(QObject*)bridge::instance());
    page->setWebChannel(channel);                   // 把channel配置到page上，让channel作为其信使
    page->load(QUrl( "file:///" + dir.absolutePath()));                         // page上加载html路径
    webView->setPage(page);                   // 建立page和UI上的webEngine的联系

    connect(webView, SIGNAL(loadFinished(bool)), &eventloop, SLOT(quit()));
    eventloop.exec();

    webView->installEventFilter(this);
    ui->layout->addWidget(webView);
    _p->webView = webView;
}

void AceEditor::checkFile(const QString &filePath)
{
    QFileInfo testfile(filePath);
    if(testfile.exists()) return;
    //不存在就创建模板
    //根据文件类型，判定模板种类

    QFile templete(QString(":/template/initTemplate.%1").arg(filePath.mid(filePath.lastIndexOf("."))));

    if( templete.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QByteArray ba = templete.readAll();
        templete.close();
        //写入文件
        QFile file(filePath);
        if(file.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            file.write(ba);
        }
        file.close();
    }
}

void AceEditor::openFile(const QString &filePath)
{
    //checkFile(filePath);

    QFile file(filePath);
    if(filePath.endsWith(".gpc") && file.open(QIODevice::ReadOnly))
    {
        QByteArray ba = file.readAll();
        setText(ba.toHex());
        setEditable(false);
    }
    else if(file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QByteArray ba = file.readAll();
        QTextCodec::ConverterState state;
        QTextCodec *codec = QTextCodec::codecForName("UTF-8");
        QString text = codec->toUnicode( ba.constData(), ba.size(), &state);
        if (state.invalidChars > 0)
        {
            text = QTextCodec::codecForName( "GBK" )->toUnicode(ba);
        }
        else
        {
            text = ba;
        }

        setText(text);
    }
    file.close();

    setMode(DataDefine::SUFFIX_MAP.at(QFileInfo(filePath).suffix()));
}
