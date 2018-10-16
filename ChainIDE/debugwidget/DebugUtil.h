#ifndef DEBUGUTIL_H
#define DEBUGUTIL_H

#include <QString>
#include <vector>

#include "DebugDataStruct.h"
class DebugUtil
{
public:
    static void getCommentLine(const QString &filePath,std::vector<int> &data);

    static void ParseDebugData(const QString &info,BaseItemDataPtr &root);

private:
    static bool isCommentLine(const QString &lineInfo,bool &isCommentStart,
                              const QString &lineComment,const QString &phaseCommentStart,const QString &phaseCommentEnd);
private:
    DebugUtil();
    ~DebugUtil();
};

#endif // DEBUGUTIL_H
