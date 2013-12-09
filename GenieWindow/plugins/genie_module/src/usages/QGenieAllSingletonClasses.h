#ifndef QGENIEALLSINGLETONCLASSES_H
#define QGENIEALLSINGLETONCLASSES_H

//class QGenieSingleProcessHandler ;
//class QGenieStyleManager ;
//class QGenieStackManager ;
//class QGenieBigButtonManager ;

//interface singletons
//class QGenieInterfaceClickManager ;
//class QGenieInterfaceInfoManager  ;
//class QGenieInterfaceToyManager   ;
//class QGenieInterfaceTrayManager ;
//class QGenieInterfaceManagerHandler ;
//class QGenieTrayManager;
//class QGenieInterfaceUiManager;
class QGeniePluginProxyManager;
//class QGenieLanguageManager;
//class QGenieInterfaceLanguageManager;
//class QGenieFrameDebug;
//class QGenieFrameDbgPluginInfo;
class QGenieInterface2Helper;
class QGenieIface2ShareObject;
class QGenieHandlerInf;
class QGenieInterface2SubscribedMessageManager;
class QGenieInterface2LogManager;
class QGenieNetgearInterface;






class QGenieAllSingletonClasses:public Singleton<QGenieAllSingletonClasses>
{
public:
    QGenieAllSingletonClasses();
    ~QGenieAllSingletonClasses();
public:
//    QGenieSingleProcessHandler *mProcessHandler;
//    QGenieStyleManager *mStyleManager;
//    QGenieStackManager *mStackManager;
//    QGenieBigButtonManager *mBigButtonManager;

    //interface singletons
//    QGenieInterfaceClickManager *mInfClickManager;
//    QGenieInterfaceInfoManager  *mInfInfoManager;
//    QGenieInterfaceToyManager   *mInfToyManager;
//    QGenieInterfaceTrayManager *mInfTrayManager;
//    QGenieInterfaceManagerHandler *mInfManagerHandler;
//    QGenieInterfaceLogManager *mInfLog;

//    QGenieTrayManager *mTrayManager;
//    QGenieInterfaceUiManager *mUiManager;
//    QGenieInterfaceSoapManager *mSoapManager;
    QGeniePluginProxyManager *mPluginProxyManager;
    QGenieSettings *mSettings;
//    QGenieLanguageManager *mLanManager;
//    QGenieInterfaceLanguageManager *mInfLanManager;
    QGenieInterface2LanguageManager *mLan2Manager;

//    QGenieFrameDbgPluginInfo *mDBGShowPluginInfo;
#ifdef GENIE_MM
    QGenieInterfaceMobileManager *mMobileManager;
#endif
#ifdef INTERFACE2
    QGenieInterface2Helper *mInterface2Helper;
    QGenieIface2ShareObject *mInterface2ShareWidget;
    QGenieHandlerInf *mHandlerInf;
    QGenieInterface2SubscribedMessageManager *mInterface2SubscribM;
    QGenieInterface2LogManager *mLogM;
    QGenieNetgearInterface *mNetgearInfoManager;
#endif
//    friend class QGenieInterfaceUiManager;




};

#endif // QGENIEALLSINGLETONCLASSES_H
