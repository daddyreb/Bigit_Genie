#ifndef SVTNETGEARSPECIFICMETHOD_H
#define SVTNETGEARSPECIFICMETHOD_H

#include <QtCore>
#include <QtNetwork>

class SVT_NodePool;
class SVT_TopoMethod;
class SVT_UpnpHelper;


class SVT_NetgearSpecificMethod:public SVT_TopoMethod
{
public:
    SVT_NetgearSpecificMethod(SVT_NodePool *pool);
    void start();
    void stop();
    bool mStarted;
protected:
//    QThreadPool &mCoreThreadPool;
    SVT_NetgearSpecificRunnableRef mNetgearRunableRef_AllResult;
  //  SVT_NetgearSpecificRunnableRef mNetgearRunableRef_Subscribed;
};

#endif // SVTUPNPMETHOD_H
