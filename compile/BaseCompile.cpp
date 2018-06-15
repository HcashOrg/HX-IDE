#include "BaseCompile.h"

class BaseCompile::DataPrivate
{
public:
    DataPrivate()
    {

    }
public:
};

BaseCompile::BaseCompile(QObject *parent) : QObject(parent)
  ,_p(new DataPrivate())
{

}

BaseCompile::~BaseCompile()
{
    delete _p;
    _p = nullptr;
}
