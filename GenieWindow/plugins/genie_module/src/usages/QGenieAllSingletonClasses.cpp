
#include "stable.h"
#include "QGenieNetgearInterface.h"


QGenieAllSingletonClasses::QGenieAllSingletonClasses()
    :mInterface2Helper(NULL)
    ,mSettings(NULL)
    ,mLan2Manager(NULL)
    ,mInterface2ShareWidget(NULL)
    ,mHandlerInf(NULL)
    ,mInterface2SubscribM(NULL)
    ,mLogM(NULL)
	,mNetgearInfoManager(NULL)
{
    mInterface2Helper=new QGenieInterface2Helper();
    if(!mInterface2Helper->mbConflictWithOtherProcess)
    {
        mSettings=new QGenieSettings();
        mLan2Manager=new QGenieInterface2LanguageManager();
        mInterface2ShareWidget=new QGenieIface2ShareObject();
        mHandlerInf=new QGenieHandlerInf();
        mLogM=new QGenieInterface2LogManager();
        mInterface2SubscribM=new QGenieInterface2SubscribedMessageManager();

        mInterface2ShareWidget->mHandler=mHandlerInf;

        mSettings->mSettingHelper->tryRecoverIniFile();
        bool dbg_mode=QCoreApplication::arguments().contains("-dbg");
        mNetgearInfoManager=new QGenieNetgearInterface(NULL,dbg_mode);
        mInterface2ShareWidget->mNetgearInfoM=mNetgearInfoManager;

        qDebug()<<"QGenieAllSingletonClasses::QGenieAllSingletonClasses()";
    }
}

QGenieAllSingletonClasses::~QGenieAllSingletonClasses()

{  
    qDebug()<<"QGenieAllSingletonClasses::~QGenieAllSingletonClasses()";
    GENIE2_SAFEDELETE(mInterface2Helper);
    GENIE2_SAFEDELETE(mLan2Manager);
    GENIE2_SAFEDELETE(mSettings);
    GENIE2_SAFEDELETE(mInterface2ShareWidget);
    GENIE2_SAFEDELETE(mHandlerInf);
    GENIE2_SAFEDELETE(mInterface2SubscribM);
    GENIE2_SAFEDELETE(mLogM);
    GENIE2_SAFEDELETE(mNetgearInfoManager);
}
