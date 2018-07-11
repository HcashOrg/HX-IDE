#include "Editor.h"
#include "ui_Editor.h"


#include "control/editorcontextmenu.h"

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

#include "bridge.h"

class Editor::DataPrivate
{
public:
    DataPrivate(const QString &path,const QString &htmlPath)
        :filePath(path),isEditable(true),HTMLPath(htmlPath)
        ,webView(nullptr),isundoAvaliable(false),isredoAvaliable(false),hasSaved(true)
    {

    }
public:
    QString filePath;
    QString HTMLPath;

    bool hasSaved;
    bool isEditable;
    bool isundoAvaliable;
    bool isredoAvaliable;
    QWebEngineView *webView;
};

Editor::Editor(const QString &path,const QString &htmlPath,QWidget *parent)
    : QWidget(parent),
    ui(new Ui::Editor)
    ,_p(new DataPrivate(path,htmlPath))
{
    ui->setupUi(this);
   // InitEditor();
}

Editor::~Editor()
{
    delete _p;
    delete ui;
}

void Editor::TabBreakPoint()
{//todo

}

void Editor::ClearBreakPoint()
{//todo

}

bool Editor::isUndoAvailable()
{
    return _p->isundoAvaliable;
}

bool Editor::isRedoAvailable()
{
    return _p->isredoAvaliable;
}

bool Editor::isEditable()const
{
    return _p->isEditable;
}

void Editor::setEditable(bool is)
{
    _p->isEditable = is;
}

void Editor::setSaved(bool isSaved)
{
    _p->hasSaved = isSaved;
}

bool Editor::isSaved()const
{
    return _p->hasSaved;
}

bool Editor::saveFile()
{
    if(isSaved()) return true;

    QFile file(_p->filePath);
    if( file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
    {
        QTextStream out(&file);
        out << getText();
        file.close();
        setSaved(true);
        emit stateChanged();
        return true;
    }
    else
    {
        setSaved(false);
        emit stateChanged();
        return false;
    }
}

const QString &Editor::getFilePath() const
{
    return _p->filePath;
}

void Editor::setUndoAvaliable(bool is)
{
    _p->isundoAvaliable = is;
}

void Editor::setRedoAvaliable(bool is)
{
    _p->isredoAvaliable = is;
}

void Editor::InitEditor()
{
    // 加载网页
    QDir dir(_p->HTMLPath);
    QEventLoop eventloop;

    QWebEngineView *webView = new QWebEngineView();
    QWebEnginePage *page = new QWebEnginePage(this);  // 定义一个page作为页面管理
    page->setBackgroundColor(Qt::transparent);
    QWebChannel *channel = new QWebChannel(this);     // 定义一个channel作为和JS或HTML交互
    channel->registerObject("qtWidget",(QObject*)bridge::instance());
    page->setWebChannel(channel);                   // 把channel配置到page上，让channel作为其信使
    page->load(QUrl( "file:///" + dir.absolutePath()));                         // page上加载html路径
    webView->setPage(page);                   // 建立page和UI上的webEngine的联系

    connect(webView, &QWebEngineView::loadFinished, &eventloop, &QEventLoop::quit);
    eventloop.exec();

    webView->installEventFilter(this);
    ui->layout->addWidget(webView);
    _p->webView = webView;

    //打开文件
    openFile(_p->filePath);
}

void Editor::openFile(const QString &filePath)
{
    QFile file(filePath);
    if(filePath.endsWith(".gpc") && file.open(QIODevice::ReadOnly))
    {
        QByteArray ba = file.readAll();
        setText(ba.toHex());
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
}

QWebEnginePage *Editor::getPage() const
{
    if(_p->webView)
    {
        return _p->webView->page();
    }
    return nullptr;
}

bool Editor::eventFilter(QObject *watched, QEvent *e)
{
    if(dynamic_cast<QWebEngineView *>(watched))
    {
        if(e->type() == QEvent::ContextMenu)
        {
            //点击行数栏不弹出
            if(this->mapFromGlobal(QCursor::pos()).x() < 50) return true;

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

QVariant Editor::syncRunJavaScript(const QString &javascript, int msec)
{
    QVariant result;
    QSharedPointer<QEventLoop> loop = QSharedPointer<QEventLoop>(new QEventLoop());
    QTimer::singleShot(msec, loop.data(), &QEventLoop::quit);
    if(getPage())
    {
        getPage()->runJavaScript(javascript, [loop, &result](const QVariant &val) {
            if (loop->isRunning()) {
                result = val;
                loop->quit();
            }
        });
    }

    loop->exec();
    return result;
}

void Editor::RunJavaScript(const QString &scriptSource)
{
    if(getPage())
    {
        getPage()->runJavaScript(scriptSource);
    }
}
