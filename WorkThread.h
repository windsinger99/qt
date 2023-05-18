#ifndef WORKTHREAD_H
#define WORKTHREAD_H

#include <QThread>

typedef void (*Callbackfun)(const void *callbackObject);
class WorkThread : public QThread
{
public:
    WorkThread(Callbackfun fun, void* obj);
    void run();

private:
    Callbackfun callFoo;
    void* callObj;
};

#endif // WORKTHREAD_H
