/*
 * WorkThread.cpp
 */
#include "WorkThread.h"

WorkThread::WorkThread(Callbackfun foo, void* obj)
{
    this->callFoo = foo;
    this->callObj = obj;
}

void WorkThread::run()
{
    this->callFoo(callObj);
    return;
}
/* end of file */
