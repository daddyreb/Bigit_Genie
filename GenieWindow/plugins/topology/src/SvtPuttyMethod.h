#ifndef __SVTPUTTY____H__
#define __SVTPUTTY____H__
#ifdef USE_PUTTY
class SVT_PuttyMethod:public SVT_TopoMethod
{
public:
    SVT_PuttyMethod(SVT_NodePool *pool);
    void start();
    void stop();
protected:
//    QThreadPool &mCoreThreadPool;
//    SVT_PuttyRunnableRef mPuttyRunnable_AllResults;
    SVT_PuttyRunnableRef mPuttyRunnable;


};
#endif

#endif // SVTNETBIOSMETHOD_H
