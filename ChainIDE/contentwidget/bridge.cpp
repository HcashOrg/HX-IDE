#include "bridge.h"

#include <mutex>
#include <QDebug>

static std::mutex dataMutex;
bridge* bridge::_instance = nullptr;
bridge::CGarbo bridge::Garbo;

bridge* bridge::instance()
{
    if(nullptr == _instance)
    {
        std::lock_guard<std::mutex> loc(dataMutex);
        if(nullptr == _instance)
        {
            _instance = new bridge();
        }
    }
    return _instance;
}

bridge::bridge(QObject *parent)
{
}

bridge::~bridge()
{
    qDebug()<<"delete javascript bridge";
}

void bridge::onTextChanged()
{
    emit textChanged();
}

void bridge::onMarkChange(int linenumber,bool isadd)
{
    emit markChanged(linenumber,isadd);
}

void bridge::gutterRightClicked()
{
    emit gutterRightClickedSignal();
}
