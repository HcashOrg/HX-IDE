#include "DebugManager.h"

class DebugManager::DataPrivate
{
public:
    DataPrivate()
    {

    }
public:
};
DebugManager::DebugManager(QObject *parent)
    : QObject(parent)
    ,_p(new DataPrivate())
{

}

DebugManager::~DebugManager()
{
    delete _p;
    _p = nullptr;
}

void DebugManager::startDebug()
{

}
