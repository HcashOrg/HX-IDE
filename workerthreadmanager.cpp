#include "workerthreadmanager.h"
#include <QMap>
#include <QDebug>
#include <QCoreApplication>

WorkerThreadManager::WorkerThreadManager(int port)
    : running(true)
    , port(port)
{
    WorkerThread* worker;
    for(int i = 1; i <= NUM_OF_WORKERTHREADS; i++)
    {
        worker = new WorkerThread(i,port);
        workerThreads.insert(i, worker);
        worker->start();
        worker->moveToThread(worker);  // 继承自qthread 只有run函数会在线程中运行
                                       // 想让其他函数在线程中运行 需要movetothread
                                       // 并且通过postevent 调用 (信号未试)
    }

}

WorkerThreadManager::~WorkerThreadManager()
{
    running = false;

    foreach ( WorkerThread* worker, workerThreads)
    {
        worker->deleteLater();
    }

    qDebug() << QThread::currentThreadId() << "~WorkerThreadManager()";
}

// 由HXChain rpcPosted 信号触发 排在事件队列
void WorkerThreadManager::processRPCs(QString cmd)
{
    WorkerThread * worker;

    int i = 1;
    bool processed = false;

retry:
    for(; i <= NUM_OF_WORKERTHREADS; i++)
    {
        worker = workerThreads.value(i);
        if( !worker->isBusy())
        {
            worker->setBusy(true);
            QCoreApplication::postEvent( worker, new TaskEvent( cmd));
            processed = true;
            break;
        }
    }

    if( !processed)   // 如果没有空闲的worker
    {
        QThread::msleep(100);
        i = 1;
        goto retry;
    }
}
