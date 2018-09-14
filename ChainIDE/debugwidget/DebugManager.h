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
    void startDebug(const QString &filePath);
    void debugNextStep();//调试到下一行
    void debugContinue();//调试到下一个断点
    void stopDebug();

    void fetchBreakPointsFinish(const std::vector<int> &data);

    DebugDataStruct::DebuggerState getDebuggerState()const;
    void setDebuggerState(DebugDataStruct::DebuggerState state);
private slots:
    void OnProcessStateChanged();

    void readyReadStandardOutputSlot();
    void readyReadStandardErrorSlot();
private:
    void InitDebugger();
    void ResetDebugger();
signals:
    void fetchBreakPoints(const QString &filePath);
    void debugOutput(const QString &message);
    void debugFinish();
    void debugError(const QString &error = "");
private:
    class DataPrivate;
    DataPrivate *_p;
};

#endif // DEBUGMANAGER_H
