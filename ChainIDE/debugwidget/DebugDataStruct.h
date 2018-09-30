#ifndef DEBUGDATASTRUCT_H
#define DEBUGDATASTRUCT_H

#include <QString>
#include <map>
#include <vector>
#include <memory>

//调试数据结构,所有调试需要的数据结构由此定义
namespace DebugDataStruct{
//调试器状态
enum DebuggerState{Available, StartDebug, StepDebug, ContinueDebug, QueryInfo, SetBreakPoint, DeleteBreakPoint};

}

//节点数据结构
class BaseItemData;
typedef std::shared_ptr<BaseItemData> BaseItemDataPtr;
class BaseItemData:std::enable_shared_from_this<BaseItemData>{
public:
    BaseItemData(const QString &k="",const QString &v="",const QString &t="",BaseItemDataPtr p = nullptr)
        :Key(k),Val(v),Type(t),parent(p){
    }
    const QString &getKey()const{return Key;}
    const QString &getVal()const{return Val;}
    const QString &getType()const{return Type;}

    void setKey(const QString &ke){Key = ke;}
    void setVal(const QString &va){Val = va;}
    void setType(const QString &te){Type = te;}

    const std::vector<BaseItemDataPtr> &getChildren()const{return children;}
    void appendChild(BaseItemDataPtr child){children.emplace_back(child);}

    void setParent(BaseItemDataPtr pa){parent = pa;}
    BaseItemDataPtr getParent()const{return parent;}
private:
    QString Key;
    QString Val;
    QString Type;
    std::vector<BaseItemDataPtr> children;
    BaseItemDataPtr parent;
};

#endif // DEBUGDATASTRUCT_H
