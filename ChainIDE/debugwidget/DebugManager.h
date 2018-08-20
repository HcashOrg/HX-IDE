#ifndef DEBUGMANAGER_H
#define DEBUGMANAGER_H

#include <QObject>

class DebugManager : public QObject
{
    Q_OBJECT
public:
    explicit DebugManager(QObject *parent = nullptr);

signals:

public slots:
};

#endif // DEBUGMANAGER_H