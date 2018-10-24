#include "DebugUtil.h"

#include <QFile>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>

#include <QStringList>
#include "DataDefine.h"

void DebugUtil::getCommentLine(const QString &filePath, std::vector<int> &data)
{
    data.clear();
    QFile file(filePath);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return;
    }
    //设置注释风格问题
    QString lineComment("//"),phaseStart("/*"),phaseEnd("*/");
    if(filePath.endsWith("."+DataDefine::GLUA_SUFFIX))
    {
        lineComment = "--";
        phaseStart = "--[[";
        phaseEnd = "]]";
    }

    bool isCommentStart = false;//标识是否已经进入注释段落
    int currentLine = 0;
    while (!file.atEnd())
    {
        QString line(file.readLine());
        line = line.trimmed();

        if(isCommentStart)
        {
            data.emplace_back(currentLine);
            isCommentLine(line,isCommentStart,lineComment,phaseStart,phaseEnd);
        }
        else if(isCommentLine(line,isCommentStart,lineComment,phaseStart,phaseEnd))
        {
            data.emplace_back(currentLine);
        }

        ++currentLine;
    }
}

bool DebugUtil::isCommentLine(const QString &lineInfo, bool &isCommentStart,
                              const QString &lineComment,const QString &phaseCommentStart,const QString &phaseCommentEnd)
{
    if(lineInfo.isEmpty())
    {
        return true;
    }
    else if(lineInfo.startsWith(phaseCommentEnd) && lineInfo.length() == phaseCommentEnd.length())
    {
        isCommentStart = false;
        return true;
    }
    else if(lineInfo.startsWith(phaseCommentStart))
    {
        if(lineInfo.length() == phaseCommentStart.length() || !lineInfo.endsWith(phaseCommentEnd))
        {
            return isCommentStart = true;
        }
    }
    else if(lineInfo.startsWith(lineComment))
    {
        return true;
    }


    int index = lineInfo.indexOf(phaseCommentStart);
    if(-1 != index)
    {
        isCommentStart = true;
    }

    if(isCommentStart)
    {
        index = lineInfo.indexOf(phaseCommentEnd);
        if (-1 != index)
        {
            isCommentStart = false;
            if (isCommentLine(lineInfo.mid(phaseCommentEnd.length()).trimmed(), isCommentStart,lineComment,phaseCommentStart,phaseCommentEnd))
            {
                return true;
            }
        }
    }
    return false;
}


void DebugUtil::ParseDebugData(const QString &info, BaseItemDataPtr &root)
{
    if(nullptr == root)
    {
        root = std::make_shared<BaseItemData>();
    }
    QJsonParseError json_error;
    QJsonDocument parse_doucment = QJsonDocument::fromJson(info.toUtf8(), &json_error);

    if(json_error.error != QJsonParseError::NoError || !parse_doucment.isObject()) return ;

    QJsonArray arr = parse_doucment.object().value("locals").toArray();

    ParseArrayData(arr,root);

//    QStringList data = info.split("\r\n");
//    QRegExp rx("(.*)=(.*)");

//    foreach (QString eachdata, data) {

//        if(rx.indexIn(eachdata) < 0 || rx.cap(1).isEmpty() || rx.cap(2).isEmpty()) continue;
//        //开始构造显示内容
//        BaseItemDataPtr pa = std::make_shared<BaseItemData>(rx.cap(1),rx.cap(2),"unknow",root);
//        root->appendChild(pa);
//    }

}

void DebugUtil::ParseArrayData(const QJsonArray &arr, BaseItemDataPtr parent)
{
    if(!parent) return;
    foreach (QJsonValue val, arr) {
        if(!val.isObject()) continue;
        QJsonObject obj = val.toObject();
        BaseItemDataPtr data = std::make_shared<BaseItemData>(obj.value("valName").toString(),"",obj.value("valType").toString(),parent);
        parent->appendChild(data);

        QJsonValue typeVal = obj.value("valValue");
        if(typeVal.isString())
        {
            data->setVal(typeVal.toString());
        }
        else if(typeVal.isBool())
        {
            data->setVal(typeVal.toBool()?"true":"false");
        }
        else if(typeVal.isDouble())
        {
            data->setVal(QString::number(typeVal.toDouble()));
        }
        else if(typeVal.isArray())
        {
            ParseArrayData(typeVal.toArray(),data);
        }
    }
}

DebugUtil::DebugUtil()
{

}

DebugUtil::~DebugUtil()
{

}
