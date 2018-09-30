#ifndef DEBUGMANAGER_H
#define DEBUGMANAGER_H

#include "DebugDataStruct.h"
#include <QObject>
//调试管理
class DebugManager : public QObject
{
    Q_OBJECT
public:
    explicit DebugManager(QObject *parent = nullptr);
    ~DebugManager();
public:
    void setOutFile(const QString &outFile);
    void startDebug(const QString &filePath,const QString &api,const QStringList &param);

    void debugNextStep();//调试到下一行
    void debugContinue();//调试到下一个断点
    void stopDebug();//停止调试
    void getVariantInfo();//获取变量信息

    void fetchBreakPointsFinish(const std::vector<int> &data);

    DebugDataStruct::DebuggerState getDebuggerState()const;
    void setDebuggerState(DebugDataStruct::DebuggerState state);

    void ReadyClose();//准备关闭

    const QString &getCurrentDebugFile()const;
private slots:
    void OnProcessStateChanged();

    void readyReadStandardOutputSlot();
    void readyReadStandardErrorSlot();
private:
    void InitDebugger();
    void ResetDebugger();

    //解析查询变量返回情况
    void ParseQueryInfo(const QString &info);
    //解析断点停顿信息
    void ParseBreakPoint(const QString &info);

    //下一次断点
    void SetBreakPoint(const QString &file,int lineNumber);
    //取消断点
    void CancelBreakPoint();

    //当前断点行
    void SetCurrentBreakLine(int li);
    int GetCurrentBreakLine()const;
    //获取下一个断点
    int getNextBreakPoint(int currentBreak,const std::vector<int> &lineVec);
signals:
    void fetchBreakPoints(const QString &filePath);
    void debugOutput(const QString &message);
    void debugStarted();
    void debugFinish();
    void debugBreakAt(const QString &file,int lineNumber);
    void debugError();

    void showVariant(BaseItemDataPtr data);
private:
    class DataPrivate;
    DataPrivate *_p;

    void testData();
};

#endif // DEBUGMANAGER_H
