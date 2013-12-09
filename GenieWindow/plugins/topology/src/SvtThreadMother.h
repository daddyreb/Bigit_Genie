#ifndef SVTTHREADMOTHER_H
#define SVTTHREADMOTHER_H

#include <QThread>
#include <QtCore>
#include "SvtCore.h"
class SVT_Core;
class SVT_NodePool;
class SVT_ThreadMother : public QThread
{
public:
    explicit SVT_ThreadMother(SVT_Core *core);
    ~SVT_ThreadMother();
    void stop();
    void setPool(SVT_NodePoolRef ref);
    QThreadPool mThreadPool;
protected:
    void run();

    SVT_Core *mCore;
    QMutex mMutex;

    SVT_NodePoolRef mNodePoolWaited;
    SVT_NodePoolRef mNodePool;

};

#endif // SVTTHREADMOTHER_H
