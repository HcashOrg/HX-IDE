#ifndef DEBUGMANAGER_H
#define DEBUGMANAGER_H

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
    void debugNextStep();
    void debugContinue();
    void stopDebug();

    void fetchBreakPointsFinish(const std::vector<int> &data);
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
