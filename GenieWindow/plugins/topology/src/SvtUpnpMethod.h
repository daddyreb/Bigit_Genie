#ifndef SVTUPNPMETHOD_H
#define SVTUPNPMETHOD_H

#include <QtCore>
#include <QtNetwork>

class SVT_NodePool;
class SVT_TopoMethod;
class SVT_UpnpHelper;


class SVT_UpnpMethod:public SVT_TopoMethod
{
public:
    SVT_UpnpMethod(SVT_NodePool *pool);
    void start();
    void stop();
    bool mStarted;
protected:
   // QThreadPool &mCoreThreadPool;
    SVT_UpnpRunnableRef mUpnpRunableRef;

};

#endif // SVTUPNPMETHOD_H
