#ifndef DEBUGDATASTRUCT_H
#define DEBUGDATASTRUCT_H

#include <QString>
#include <map>
#include <vector>

//调试数据结构,所有调试需要的数据结构由此定义
namespace DebugDataStruct{
//调试器状态
enum DebugerState{Available, PauseAtBreak, InDebug, Unavailable};//空闲，停在断点，运行中，不可用

}

#endif // DEBUGDATASTRUCT_H
