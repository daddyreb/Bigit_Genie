#ifndef SVTAFPMETHOD_H
#define SVTAFPMETHOD_H

class SVT_AfpMethod:public SVT_TopoMethod
{
public:
    SVT_AfpMethod(SVT_NodePool *pool);
    void start();
    void stop();
protected:

//    QThreadPool &mCoreThreadPool;
    SVT_AfpRunnableRef mAfpRunnables[AFP_BASIC_RUNNABLE_NUM];
    SVT_AfpRunnableRef mAfpSubscribedRunnable[AFP_SUBSCRIB_RUNNABLE_NUM];


};

#endif // SVTAFPMETHOD_H
