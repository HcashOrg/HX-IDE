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
    void startDebug();
private:
    class DataPrivate;
    DataPrivate *_p;
};

#endif // DEBUGMANAGER_H
