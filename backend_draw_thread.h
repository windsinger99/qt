#ifndef BACKEND_DRAW_THREAD_H
#define BACKEND_DRAW_THREAD_H

#include <QThread>
#include <QWaitCondition>
#include <QMutex>
#include <QKeyEvent>

#ifdef __cplusplus
extern "C" {
#endif

extern int BG_debug_draw_flag;
#define IS_DEBUG_DRAW_FLAG    if(BG_debug_draw_flag)

extern pthread_mutex_t m_drawMutex;
extern pthread_cond_t m_drawCond;
extern pthread_mutex_t m_waitMutex;
extern pthread_cond_t m_waitCond;
extern int draw_pause;
extern int polgon_draw_idx;
extern int wakeUpForWaitThread;
extern int waitStatusForWaitThread;
extern int stopWaitThread;

#ifdef __cplusplus
}
#endif

class Backend_Draw_Thread: public QThread
{
    Q_OBJECT

public:
    explicit Backend_Draw_Thread(QObject *parent = 0);
    ~Backend_Draw_Thread();

    void startDrawThread();
    void stopDrawThread();

    bool threadStop;
    int drawRemove;

private:
    void run();

signals:
    void threadSigDrawOutPoly(float x0, float y0, float x1, float y1, unsigned long color__, int wakeStatus);
    void threadSigDrawOutOnePoly(float x0, float y0, float x1, float y1, unsigned long color__, int wakeStatus);

public slots:

};
class Backend_Wait_Thread: public QThread
{
    Q_OBJECT

public:
    explicit Backend_Wait_Thread(QObject *parent = 0);
    ~Backend_Wait_Thread();

    void startWaitThread();
    void stopWaitThread();

    bool threadStop;

private:
    void run();
};
#endif // BACKEND_DRAW_THREAD_H
