#include "workerthread.h"


WorkerThread::WorkerThread(QWidget* parent)
{
    w = new QWidget(parent);
    layout = new QHBoxLayout(w);
    pi = new QProgressIndicator(w);
    pi->setAnimationDelay(50);

}
void WorkerThread::run()
{
    pi->startAnimation();
}
