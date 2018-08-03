#include "InterfaceWidget.h"
#include "ui_InterfaceWidget.h"

#include <QFileInfo>
#include <QDir>
#include <QTextCodec>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QToolButton>
#include <QDebug>
#include <QTabBar>

#include "IDEUtil.h"
#include "ConvenientOp.h"

class InterfaceWidget::DataPrivate
{
public:
    DataPrivate()
        :data(std::make_shared<DataDefine::ApiEvent>())
    {

    }
public:
    QString currentFilePath;
    ApiEventPtr data;
};

InterfaceWidget::InterfaceWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::InterfaceWidget),
    _p(new DataPrivate())
{
    ui->setupUi(this);
    InitWidget();
}

InterfaceWidget::~InterfaceWidget()
{
    delete _p;
    delete ui;
}

void InterfaceWidget::ShowInterface(const QString &filePath)
{
    _p->currentFilePath = filePath;
    InitData();
}

void InterfaceWidget::retranslator()
{
    ui->retranslateUi(this);
}

void InterfaceWidget::InitData()
{
    ui->treeWidget_event->clear();
    ui->treeWidget_function->clear();
    if(_p->currentFilePath.isEmpty()) return;
    //寻找对应的.meta.json，，glua文件寻找自身名字的.meta.json文件，，java文件寻找自身所在目录的.meta.json文件
    QFileInfo file(_p->currentFilePath);
    QString filePath;
//    if(_p->currentFilePath.endsWith("."+DataDefine::GLUA_SUFFIX))
//    {//查找同样的文件名即可
//        filePath = _p->currentFilePath+".meta.json";
//    }
//    else
//    {//查找文件夹对应的名称的文件
//        filePath = file.absoluteDir().absolutePath()+QDir::separator()+file.dir().dirName() + ".meta.json";
//    }
    if(_p->currentFilePath.endsWith("."+DataDefine::GLUA_SUFFIX)||
       _p->currentFilePath.endsWith("."+DataDefine::JAVA_SUFFIX)||
       _p->currentFilePath.endsWith("."+DataDefine::CSHARP_SUFFIX)||
       _p->currentFilePath.endsWith("."+DataDefine::KOTLIN_SUFFIX)
       )
    {
        filePath = ConvenientOp::GetMetaJsonFile(_p->currentFilePath);
    }
    else if(_p->currentFilePath.endsWith("."+DataDefine::CONTRACT_SUFFIX))
    {
        filePath = file.absoluteDir().absolutePath()+QDir::separator()+file.dir().dirName() + "."+DataDefine::META_SUFFIX;
    }

    if(readApiFromPath(filePath,_p->data))
    {
        QStringList apis = _p->data->getAllApiName();
        foreach (QString api, apis) {
            QTreeWidgetItem *item = new QTreeWidgetItem(QStringList()<<api);
            ui->treeWidget_function->addTopLevelItem(item);
        }

        QStringList events = _p->data->getAllEventName();
        foreach (QString api, events) {
            QTreeWidgetItem *item = new QTreeWidgetItem(QStringList()<<api);
            ui->treeWidget_event->addTopLevelItem(item);
        }

    }
}

void InterfaceWidget::InitWidget()
{
    //隐藏左右控制按钮
    ui->tabWidget->tabBar()->setUsesScrollButtons(false);

    ui->treeWidget_event->header()->setVisible(false);
    ui->treeWidget_function->header()->setVisible(false);
    ui->title_Label->setVisible(false);
    ui->tabWidget->setCurrentIndex(0);
}

bool InterfaceWidget::readApiFromPath(const QString &filePath, ApiEventPtr &results)
{
    //解析json文档
    if (!results) results = std::make_shared<DataDefine::ApiEvent>();
    results->clear();

    QFile contractFile(filePath);
    if(!contractFile.open(QIODevice::ReadOnly)) return false;

    QString jsonStr(contractFile.readAll());
    contractFile.close();

    QTextCodec* utfCodec = QTextCodec::codecForName("UTF-8");
    QByteArray ba = utfCodec->fromUnicode(jsonStr);

    QJsonParseError json_error;
    QJsonDocument parse_doucment = QJsonDocument::fromJson(ba, &json_error);

    if(json_error.error != QJsonParseError::NoError || !parse_doucment.isObject()) return false;

    QJsonObject jsonObject = parse_doucment.object();

    QJsonArray Apis = jsonObject.value("api").toArray();
    foreach (QJsonValue obj, Apis) {
        results->addApi(obj.toString());
    }

    QJsonArray Events = jsonObject.value("event").toArray();
    foreach (QJsonValue obj, Events) {
        results->addEvent(obj.toString());
    }

    return true;
}
