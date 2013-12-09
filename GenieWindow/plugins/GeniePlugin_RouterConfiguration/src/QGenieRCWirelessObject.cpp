#include "QGenieRCWirelessObject.h"
#include "genie2_interface.h"
#include "genie2_netgear.h"
#include "language_RouterConfig.h"
#include "QProgressIndicator.h"
#include "QGenieGaccObject.h"
#include "QGenieFupdateObject.h"
#include "pcpform.h"
#include "QPiObject.h"
#include "QGenieQRCodeManager.h"
//#include "QGenieTabCanLockHeader.h"
#include <QtGui>
#include <QUiLoader>
#ifdef USE_READYSHARE
#include "pcpform.h"
#endif
#ifdef USE_TRAFFICMETER
#include "QGenieTrafficMeterObject.h"
#endif
#ifdef USE_QOSFUNC
#include "QGenieQosObject.h"
#endif
#include "QGenieRebootObject.h"
#include "QGenieTimelineLocker.h"
#include "QGenieTimeGenerator.h"
#include "QGenieDelayProcessLogic.h"
#include "QGenieRCTopologyObject.h"
#include "SvtFcmlError.h"
#include "genie2_plugincommunication.h"
#include "QGenieRCLackDataGetter.h"
#define ERROR_MODE "<center><font color='red'>%1</font>"\
"<a href=\"http://www.netgear.com/landing/en-us/netgear-genie-faq.aspx\"><img src=\":/images/faq.png\" /></a></center>"
#define SM_COB_MOD "%1(%2) %3"


quint64 sRCFlags=0;
#define REFRESH_FLAGS_CLEARER   (RCF_MUST_REFRESH_GACC|RCF_MUST_REFRESH_TM |RCF_MUST_REFRESH_WIRELESS \
                                           |RCF_MUST_REFRESH_TOPO \
                                           |RCF_MUST_REFRESH_WIRELESS_5G|RCF_MUST_REFRESH_GACC_5G)

//lenovot61p
const int cRouterModelRole=Qt::UserRole+1;
const int cOnlineRole=Qt::UserRole+2;
const int cFirmwareRole=Qt::UserRole+3;
const int cSerialRole=Qt::UserRole+4;
const int cErrorLabGeometry[]={10,20,400,20};
extern QString autoWrap(const QString &s,int limit=34);

static const int c5GChannels[]={/*"Auto"*/40,44,48,149,153,157,161};



QTellLPCHelper::QTellLPCHelper(const QMap<QString, QPair<QString, QString> > &map)
    :mCpSecureRef(map)
    ,mRemote(false)
{

}

void QTellLPCHelper::whenLoginOk(bool remote, const QString &usr, const QString &pwd)
{
    if(mRemote && !remote)
    {
        GENIE2_TELL_OTHERPLUGIN(PLUGIN_UUID_LPC,EPCC_SMARTNETWORK_DATA,QVariantList()<<false);
    }
    mRemote=remote;
    mLoginUser=usr;
    mLoginPwd=pwd;
    mCpName="";
}

void QTellLPCHelper::tryTellLPC()
{

    QGenieRCWirelessObject *rcwo=QGenieRCWirelessObject::GetSingletonPtr();
    Q_ASSERT(rcwo);
    QString cpname=rcwo->getCpName();
 //   QMessageBox::warning(NULL,"","tryTellLPC "+mCpName+" "+cpname);

    if(mCpName != cpname)
    {
        mCpName=cpname;
        if(mCpSecureRef.contains(cpname))
        {
        GENIE2_TELL_OTHERPLUGIN(PLUGIN_UUID_LPC,EPCC_SMARTNETWORK_DATA
                                         ,QVariantList()<<true<<mLoginUser<<mLoginPwd<<mCpName
                                <<mCpSecureRef[cpname].first<<mCpSecureRef[cpname].second);
        }
    }
}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
int getSecureIdx(const QString &secure)
{
    int secure_idx=1;
    if(0==secure.compare("none",Qt::CaseInsensitive))
    {
        secure_idx=0;
    }
    else if(
            (0==secure.compare("wep",Qt::CaseInsensitive) )
            ||(0==secure.compare("wpa2-psk",Qt::CaseInsensitive) )
          //  || (0==secure.compare("wep64",Qt::CaseInsensitive) )
          //  || (0==secure.compare("wep128",Qt::CaseInsensitive) )
            )
    {
        secure_idx=1;

    }
    else if(
            (0==secure.compare("wpa-psk/wpa2-psk",Qt::CaseInsensitive) )
            || (0==secure.compare("wpa-psk",Qt::CaseInsensitive) )
            || (0==secure.compare("mixed wpa",Qt::CaseInsensitive) )
            )
    {
        secure_idx=2;
    }
    return secure_idx;
}


void SerializeCpSecure(QMap<QString,QPair<QString,QString> > &map,bool load)
{
    QFile f;
    //QString fname=QStandardPaths::writableLocation(QStandardPaths::DataLocation)+"/NETGEARGenie/"
    QString fname=QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/"
                  +"cp_secure.save";
    f.setFileName(fname);

    if(f.open(load?QIODevice::ReadOnly:QIODevice::WriteOnly))
    {
        QDataStream stream(&f);
        if(load)
        {
            stream >> map;
        }
        else
        {
            stream <<map;
        }

    }
}

class _LazyLoader
{
    enum LOAD_STEPS
    {
        LOAD_STEP1
        ,LOAD_STEP2
        ,LOAD_STEP3
        ,LOAD_STEP4
        ,LOAD_STEP5
        ,LOAD_STEP6
        ,LOAD_STEP7
        ,LOAD_STEP8 //kai.yan add for Qos
        ,LOAD_STEP9 //kai.yan add for reboot
        ,LOAD_FINISH
    };

    _LazyLoader(QGenieRCWirelessObject *w)
        :mW(w)
        ,mCurStep(LOAD_STEP1)
    {
    }
    bool loadStep()
    {
        QGenieGaccObject *  gaccObj;
        QGenieTrafficMeterObject *tmobj;
        QGenieFupdateObject *fupo;
		QGenieQosObject *qoso;
        QGenieRebootObject *rebootObj;
        QVariant v;
       // QWidget *w;

        switch(mCurStep)
        {

        case LOAD_STEP1:
#ifdef USE_READYSHARE
            if(!mW->mPcpForm)
            {
                mW->mPcpForm=new PCPForm(mW->mWirelessW);
                mW->mPcpForm->setupUi0();
                //        QTabWidget *tab=qFindChild<QTabWidget *>(mWirelessW,"tab_logedin");
            }
#endif
            break;
        case LOAD_STEP2:
#ifdef USE_READYSHARE
            mW->mPcpForm->setupUi1();
            Q_ASSERT(mW->mTab);
//            mW->mTab->insertTab(mW->mTab->count(),mW->mPcpForm,GENIE2_GET_TEXT(L_READY_SHARE_LIVE));

#endif
            break;
        case LOAD_STEP3:
            if(!mW->mGaccW)
            {
                gaccObj=QGenieGaccObject::GetSingletonPtr();
                Q_ASSERT(gaccObj);

                mW->mGaccW=gaccObj->getGaccUi();
//                Q_ASSERT(gaccObj);
        //        QTabWidget *mTab=qFindChild<QTabWidget *>(mWirelessW,"tab_logedin");
//                Q_ASSERT(mW->mTab);
//                mW->mTab->insertTab(mW->mTab->count(),mW->mGaccW,GENIE2_GET_TEXT(L_GACC_BIGBT));
            }
            break;
       case LOAD_STEP4:
#ifdef USE_TRAFFICMETER
            tmobj=QGenieTrafficMeterObject::GetSingletonPtr();
            if(!mW->mTMW)
            {
                mW->mTMW=tmobj->getUi();
//                mW->mTab->insertTab(mW->mTab->count(),mW->mTMW,GENIE2_GET_TEXT(L_TRAFFICMETER_BIGBT)/*"Traffic Meter"*/);
            }


#endif
            break;
       case LOAD_STEP5:
//#ifdef USE_SUBPLUGINS
            Q_ASSERT(!mW->mSubpluginUpdateW);
            fupo=QGenieFupdateObject::GetSingletonPtr();
            Q_ASSERT(fupo);
            mW->mSubpluginUpdateW=fupo->getUi();
//            mW->mTab->insertTab(mW->mTab->count()
//                                ,mW->mSubpluginUpdateW
//                                ,GENIE2_GET_TEXT(L_UPGRADE_MAIN_NAME));


//#endif
            break;
       case LOAD_STEP6:
#ifdef USE_SUBPLUGINS
            if(!mW->mWExportW0 && !mW->mWExportW1)
            {
                if(mW->mSubpluginExportUuid<0)
                {
                    mW->mSubpluginExportUuid=GENIE2_HANDLER_CMD(HCMD_LOADPLUGIN,PLUGIN_FILE_WEXPORT).toInt();
                }
                if(mW->mSubpluginExportUuid>=0)
                {
                    v=GENIE2_TELL_OTHERPLUGIN(mW->mSubpluginExportUuid,EPCC_ROUTERCONFIGUI,0);
                    mW->mWExportW0=(QWidget *)v.value<void *>();
                    if(mW->mWExportW0)
                    {
                        mW->mStackWC->insertWidget(mW->mStackWC->count()
                                                   ,mW->mWExportW0);
                    }
                }
            }
            mW->mExport->setVisible(mW->mWExportW0);
#endif
            break;
        case LOAD_STEP7:
            if(!mW->mTpW)
            {
                mW->mTpW=QGenieRCTopologyObject::GetSingleton().getUi();
                Q_ASSERT(mW->mTpW);
            }
            break;
		case LOAD_STEP8:
			if(!mW->mQosW)
			{
				mW->mQosW = QGenieQosObject::GetSingleton().getQosUi();
				Q_ASSERT(mW->mQosW);
			}
			break;
        case LOAD_STEP9:
            if(!mW->mRebootW)
            {
                mW->mRebootW = QGenieRebootObject::GetSingleton().getRebootUi();
                Q_ASSERT(mW->mRebootW);
            }
            break;
        }

        if(mW->mIndicatorLogin && mW->mIndicatorLogin->mW->isVisible())
        {
            mW->mIndicatorLogin->timerEvent(NULL);
            mW->mIndicatorLogin->mW->repaint();
        }
        if(mCurStep <LOAD_FINISH)
        {
            mCurStep++;
            return true;
        }
        return false;
    }

    QGenieRCWirelessObject *mW;
    int mCurStep;
    friend class QGenieRCWirelessObject;

};

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
class LockerWhenAllCPOffline:public QObject
{
public:
    LockerWhenAllCPOffline(QWidget *w)
        :QObject(w)
    {
        QList<QPushButton *> lst_buts=w->findChildren<QPushButton *>();
        foreach(QPushButton *but,lst_buts)
        {
            if(but->objectName()!="but_logout")
            {
                mLockedUi<<but;
                but->setEnabled(false);
            }

        }
    }

    ~LockerWhenAllCPOffline()
    {
        foreach(QWidget *w,mLockedUi)
        {
            w->setEnabled(true);
        }
    }
    QList<QWidget *> mLockedUi;
};


///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
QGenieRCWirelessObject::tRCData::tRCData()
{
    mSavedRdt=NULL;

}

QGenieRCWirelessObject::tRCData::~tRCData()
{

}

QGenieRCWirelessObject::QGenieRCWirelessObject(QObject *parent)
    :QObject(parent)
    ,mWirelessW(NULL)
    ,mGaccW(NULL)
    ,mTpW(NULL)
    ,mSessionLogin(-1)
    ,mSessionSMLogin(-1)
    ,mSessionWireless(-1)
    ,mSessionWireless5G(-1)
    ,mSessionFirmware(-1)
//    ,mSession5G(-1)
    ,mIndicatorLogin(NULL)
    ,mIndicatorWireless(NULL)
    ,mIndicatorWirelessSetting(NULL)
    ,mStackTotal(NULL)
    ,mEdtUsr(NULL)
    ,mEdtPwd(NULL)
    ,mEdtSMUsr(NULL)
    ,mEdtSMPwd(NULL)
    ,mStackWC(NULL)
    ,mEdtSsid(NULL)
    ,mEdtKey(NULL)
    ,mRdtNone(NULL)
    ,mRdtAES(NULL)
    ,mRdtMix(NULL)
    ,mCobChannel(NULL)
    ,mSubpluginUpdateW(NULL)
#ifdef USE_5GFUNC
    ,m5GTab(NULL)
    ,m5GFlagLabel(NULL)
#endif
//    ,mFakeStack(NULL)
//    ,mFakeCanvas(NULL)
//    ,mFakeLogoutBut(NULL)
#ifdef USE_SUBPLUGINS

   // ,mSubpluginUpdateUuid(-1)
    ,mWExportW0(NULL)
    ,mWExportW1(NULL)
    ,mSubpluginExportUuid(-1)
#endif
#ifdef USE_TRAFFICMETER
    ,mTMW(NULL)
#endif
    ,mLanLoginErrorLocal(-1)
    ,mLanLoginErrorRemote(-1)

#ifdef USE_READYSHARE
    ,mPcpForm(NULL)
#endif
#ifdef USE_QOSFUNC
	,mQosW(NULL)
#endif
    ,mRebootW(NULL)
    ,lab_login_error(NULL)
//    ,mSavedRdt(NULL)
    ,mErrorLabel(NULL)
    ,mLanError(-1)
	,mExport(NULL)
    ,mButSMLogin(NULL)
	,mLogout(NULL)
	,mModify(NULL)
        ,mLazyLoader(new _LazyLoader(this))
        ,cob_remote_model(NULL)
        ,mReloginDlg(NULL)
        ,stack_login(NULL)
        ,cob_loginmode(NULL)
    ,mSavedTabW(NULL)
    ,mTellLpcHelper(mCpSecure)
    ,mCompleter(NULL)
    ,mCompleteModel(NULL)
    ,mSavedCpIdx(-1)
    ,mLockerWhenAllCPOffline(NULL)
	,m_QosState(0)
{
    mTimelineLocker=new QGenieTimelineLocker();
    mQRCM=new QGenieQRCodeManager();
    mLDGetter=new QGenieRCLackDataGetter(this);


//#ifdef USE_SUBPLUGINS
//    if(mSubpluginUpdateUuid<0)
//    {
//        mSubpluginUpdateUuid=GENIE2_HANDLER_CMD(HCMD_LOADPLUGIN,PLUGIN_FILE_UPDATE).toInt();
//    }
//#endif
}

QGenieRCWirelessObject::~QGenieRCWirelessObject()
{
//    GENIE2_SAFEDELETE(mWirelessW);
    GENIE2_SAFEDELETE(mTimelineLocker);
    GENIE2_SAFEDELETE(mLazyLoader);
    GENIE2_SAFEDELETE(mQRCM);
    GENIE2_SAFEDELETE(mLDGetter);
	mSavedData.clear();
	mSavedData_5G.clear();
}

void QGenieRCWirelessObject::logOut(bool remote)
{
    sRCFlags |= RCF_IN_LOGOUT_FUNCTION;
    if(sRCFlags &RCF_RELOGIN_DIALOG_ON)
    {
        if(mReloginDlg)
        {
            mReloginDlg->reject();
        }
    }
    if(remote)
    {
        sRCFlags &=~(RCF_IN_REMOTE_LOGIN_PROCESS|RCF_REMOTE_LOGEDIN|RCF_IN_REMOTE_REFRESHING
                     |RCF_ALL_CP_OFFLINE);
        sRCFlags &=~(RCF_REMOTE_GACC_5GUNLOCKED|RCF_REMOTE_WIRELESS_5GUNLOCKED);
        mSessionSMLogin=-1;
        mSessionFirmware=-1;
        mSessionWireless=-1;
        mSessionWireless5G=-1;

        mCpSecure.clear();
    }
    else
    {
        sRCFlags &= ~(RCF_IN_LOCAL_LOGIN_PROCESS|RCF_LOCAL_LOGEDIN|RCF_IN_LOCAL_REFRESHING
                      |RCF_FUPDATE_DATA_OK|RCF_FUPDATE_IN_REFRESHING);
        sRCFlags &= ~(RCF_LOCAL_GACC_5GUNLOCKED|RCF_LOCAL_WIRELESS_5GUNLOCKED);
        mSessionLogin=-1;
        mSessionWireless=-1;
        mSessionWireless5G=-1;
    }

    Q_ASSERT(mStackTotal);
    mStackTotal->setCurrentIndex(0);

    QGenieGaccObject::GetSingleton().whenLogout();

//    mSessionWireless=-1;
//    mSession5G=-1;
    QGenieDelayProcessLogic::GetSingleton().whenLogout(remote);
//    mWirelessW->setEnabled(true);
    GENIE2_LOG("logout");

    QGenieTimelineLocker *locker=QGenieTimelineLocker::GetSingletonPtr();
    Q_ASSERT(locker);
    locker->forceStop();
    _updateLockState();
    if(remote)
    {
        QGenieIface2ShareObject::sSingleInstance->mNetgearInfoM
                ->netgearCommand(INetgearInterface::IC_SM_LOGOUT,QStringList(),NULL,NULL);

    }
    if(!(sRCFlags &(RCF_LOCAL_LOGEDIN|RCF_REMOTE_LOGEDIN)))
    {
		mSavedData.clear();
		mSavedData_5G.clear();
    }
    m5GTab->setCurrentIndex(0);
    sRCFlags &=~RCF_IN_LOGOUT_FUNCTION;


}


QWidget *QGenieRCWirelessObject::getWirelessUi()
{
    Q_ASSERT(!mWirelessW);

    bool btemp;
    QUiLoader loader;
    QFile f(GENIE2_RES("ui/wirelesssetting.ui"));
    btemp=f.open(QFile::ReadOnly);
    Q_ASSERT(btemp);


#ifdef USE_WIRELESS_PARENT_UI
    QWidget *pw=new QWidget(QGenieIface2ShareObject::sSingleInstance->mMainFrame);
    QHBoxLayout *layout=new QHBoxLayout(pw);
    layout->setContentsMargins(0,0,0,0);
    QWidget *w=loader.load(&f,pw);
    layout->addWidget(w);
#else
    QWidget *w=loader.load(&f,QGenieIface2ShareObject::sSingleInstance->mMainFrame);
    w->hide();
    Q_ASSERT(w);
#endif

    lab_model=w->findChild<QLabel *>("lab_model");
    lab_local_model=w->findChild<QLabel *>("lab_local_model");
    lab_firmware=w->findChild<QLabel *>("lab_firmware");
    lab_remote_firmware=w->findChild<QLabel *>("lab_remote_firmware");
    Q_ASSERT(lab_model && lab_firmware && lab_local_model
             && lab_remote_firmware);

    translateWirelessWidget(w);
    deployIndicators(w);

    QList<QPushButton *> bts=w->findChildren<QPushButton *>();
    int c=bts.count();
    for(int i=0;i<c;i++)
    {
        connect(bts[i],SIGNAL(clicked()),this,SLOT(slot_buttonClicked()));
    }

    mStackTotal=w->findChild<QStackedWidget *>("stack_total");
    mEdtUsr=w->findChild<QLineEdit *>("edt_usr");
    mEdtPwd=w->findChild<QLineEdit *>("edt_pwd");
    mEdtSMUsr=w->findChild<QLineEdit *>("edt_email");
    mEdtSMPwd=w->findChild<QLineEdit *>("edt_smpwd");
    mEdtSsid=w->findChild<QLineEdit *>("sedt_ssid");
	mEdtSsid->setText(QString("N/A"));
	mEdtIP = w->findChild<QLineEdit *>("edit_ip");
	Q_ASSERT(mEdtIP);

    mEdtKey=w->findChild<QLineEdit *>("sedt_key");

    mRdtNone=w->findChild<QRadioButton *>("rdt_none");
    mRdtAES=w->findChild<QRadioButton *>("rdt_aes");
    mRdtMix=w->findChild<QRadioButton *>("rdt_mix");
    Q_ASSERT(mRdtNone && mRdtAES && mRdtMix);
    //        QRadioButton *rdt_mix=qFindChild<QRadioButton *>(mWirelessW,"rdt_mix");
    mCobChannel=w->findChild<QComboBox *>("cob_channel");
    mStackWC=w->findChild<QStackedWidget *>("stack_wc");
    connectStateCheckSignalSlots(mStackWC);
    mTab=w->findChild<QTabWidget *>("tab_logedin");
    Q_ASSERT(mTab);
    mTab->setFocusPolicy(Qt::NoFocus);

    QList<QTabBar *> bars=mTab->findChildren<QTabBar*>();
    Q_ASSERT(bars.count()==1);
    QList<QWidget*> childs=bars[0]->findChildren<QWidget*>();
    foreach(QWidget *c,childs)
    {
        c->setFocusPolicy(Qt::NoFocus);
    }

//    bars[0]->setFocusPolicy(Qt::NoFocus);

    connect(mTab,SIGNAL(currentChanged(int)),this,SLOT(slot_TabIdxChanged(int)));
    mPage1=mTab->widget(0);
    Q_ASSERT(mPage1);

#ifdef USE_5GFUNC
    m5GTab=new QTabBar(w);
    m5GTab->setShape(QTabBar::TriangularWest);
    m5GTab->setFocusPolicy(Qt::NoFocus);
//    bar->setStyleSheet("background-color:red");
    m5GTab->setDrawBase(false);
    m5GTab->addTab("2.4G");
    m5GTab->addTab("5G");

   m5GTab->setFixedSize(16,200);
   m5GTab->move(0,200);
    m5GTab->setCurrentIndex(0);
    connect(m5GTab,SIGNAL(currentChanged(int)),this,SLOT(slot_5GTabIndexChanged(int)));
    m5GTab->raise();

    m5GFlagLabel=new QLabel(w);
    m5GFlagLabel->move(15,42);
    QPixmap pm=QPixmap(GENIE2_RES("images/conner_24.png"));
    m5GFlagLabel->setPixmap(pm);
    m5GFlagLabel->resize(pm.size());
    connect(mStackWC,SIGNAL(currentChanged(int)),this,SLOT(slot_Refresh5GUiWhenModifyModeChanged()));
#endif


    login_frame=w->findChild<QFrame *>("frame_login");
    Q_ASSERT(login_frame);
    chk_remmember=login_frame->findChild<QCheckBox *>("chk_remember");
	chk_inputIP = login_frame->findChild<QCheckBox *>("chk_enterIP");
	frame_ip = login_frame->findChild<QFrame*>("frame_ip");
    chk_smremember=login_frame->findChild<QCheckBox *>("chk_smremember");
    cob_remote_model=w->findChild<QComboBox *>("cob_remote_model");
    connect(cob_remote_model,SIGNAL(currentIndexChanged(int)),this,SLOT(slot_CpNameChaged(int)));
	
    Q_ASSERT(chk_remmember && cob_remote_model && chk_inputIP && frame_ip);
//    connect(chk_remmember,SIGNAL(clicked(bool)),this,SLOT(slot_ChkRememberClicked(bool)));
    connect(chk_remmember,SIGNAL(clicked(bool)),this,SLOT(slot_RemmemberClicked(bool)));
    connect(chk_smremember,SIGNAL(clicked(bool)),this,SLOT(slot_RemmemberClicked(bool)));
	connect(chk_inputIP, SIGNAL(clicked(bool)), this, SLOT(slot_InputIPCheckBoxChanged(bool)));///kai.yan add

    chk_remmember->setChecked(GENIE2_GET_SETTING(PLUGIN_ROUTECONF_SAVEPWD).toBool());
	chk_inputIP->setChecked(GENIE2_GET_SETTING(PLUGIN_ROUTECONF_SAVEIP).toBool());///kai.yan add
    chk_smremember->setChecked(GENIE2_GET_SETTING(PLUGIN_SMARTNETWORK_SAVEPWD).toBool());
    mEdtPwd->setText(chk_remmember->isChecked()?GENIE2_GET_SETTING(PLUGIN_ROUTECONF_PWD).toString():"");

	frame_ip->setVisible(chk_inputIP->isChecked());///kai.yan add
	slot_InputIPCheckBoxChanged(chk_inputIP->isChecked());

    QString savedUser=GENIE2_GET_SETTING(PLUGIN_SMARTNETWORK_USER).toString();
    mCompleter=new QCompleter(this);
    mCompleteModel=new QStandardItemModel(1,1,this);
    mCompleter->setModel(mCompleteModel);
    mEdtSMUsr->setCompleter(mCompleter);
    if(!savedUser.isEmpty())
    {
        mCompleteModel->insertRow(mCompleteModel->rowCount());
        QModelIndex index=mCompleteModel->index(mCompleteModel->rowCount()-1,0);
        mCompleteModel->setData(index,savedUser);
        mEdtSMUsr->setText(savedUser);
        sRCFlags |=RCF_SMUSER_IS_SAVEDUSER;
    }
    else
    {
        mEdtSMUsr->clear();
        sRCFlags &= ~RCF_SMUSER_IS_SAVEDUSER;
    }


    mEdtSMPwd->setText(chk_smremember->isChecked()?GENIE2_GET_SETTING(PLUGIN_SMARTNETWORK_PWD).toString():"");
    connect(mEdtSMUsr,SIGNAL(textEdited(QString)),this,SLOT(slot_SMUserNameChanged(QString)));
    connect(mEdtSMUsr,SIGNAL(textChanged(QString)),this,SLOT(slot_SMUserNameChanged(QString)));


    lab_login_error=w->findChild<QLabel *>("lab_login_error");
    Q_ASSERT(lab_login_error);
    but_apply=w->findChild<QPushButton *>("but_apply");
    fr_pwd=w->findChild<QFrame *>("fr_pwd");
    Q_ASSERT(but_apply && fr_pwd);

    bool isUnlocked=GENIE2_GET_SETTING(PLUGIN_SMARTNETWORK_UNLOCKED).toBool();

    bool isRemote=isUnlocked?GENIE2_GET_SETTING(PLUGIN_SMARTNETWROK_ISREMOTE).toBool():false;

    cob_loginmode=w->findChild<QComboBox *>("cob_loginmode");
    stack_login=login_frame->findChild<QStackedWidget *>("stack_login");
    Q_ASSERT(stack_login &&cob_loginmode);
    cob_loginmode->setCurrentIndex(isRemote?1:0);
    connect(cob_loginmode,SIGNAL(currentIndexChanged(int)),this,SLOT(slot_LoginModeChaged(int)));

    stack_login->setCurrentIndex(isRemote?1:0);
    changeRemoteState(isRemote);
    if(!isUnlocked)
    {
        QLabel *lab_lgfrom=login_frame->findChild<QLabel *>("lab_lgfrom");
        Q_ASSERT(lab_lgfrom);
        cob_loginmode->setVisible(false);
        lab_lgfrom->setVisible(false);
    }


    //mErrorLabel=new QLabel(mTab->widget(0));
    //mErrorLabel->setGeometry(cErrorLabGeometry[0],cErrorLabGeometry[1],cErrorLabGeometry[2],cErrorLabGeometry[3]);
    //mErrorLabel->show();
    mErrorLabel = w->findChild<QLabel *>("label_info_error");
    Q_ASSERT(mErrorLabel);
    mModify = w->findChild<QPushButton *>("but_modify");
    mLogout = w->findChild<QPushButton *>("but_logout");
    mButSMLogin=w->findChild<QPushButton *>("but_smlogin");
    mExport = w->findChild<QPushButton *>("but_export");
    Q_ASSERT(mModify && mLogout && mExport && mButSMLogin);

    QLabel *lab_forgot=login_frame->findChild<QLabel *>("lab_forgot");
    QLabel *lab_signup=login_frame->findChild<QLabel *>("lab_signup");

    Q_ASSERT(lab_forgot &&lab_signup);
    connect(lab_forgot,SIGNAL(linkActivated(QString)),this,SLOT(slot_buttonClicked()));
    connect(lab_signup,SIGNAL(linkActivated(QString)),this,SLOT(slot_buttonClicked()));



    mWirelessW=w;
    logOut(true);
    logOut(false);
    updateUi();
    return w;
    //    mStackWidget=qFindChild<QStackedWidget *>(w,"stack");
    //    Q_ASSERT(mStackWidget);

    //    mLeftPan=qFindChild<QWidget *>(w,"leftpan");
    //    Q_ASSERT(mLeftPan);

    //    mAdvW=qFindChild<QWidget *>(w,"adv");
    //    Q_ASSERT(mAdvW);
    //    mAdvW->installEventFilter(this);

}

void QGenieRCWirelessObject::slot_CpNameChaged(int idx)
{ 
    if((sRCFlags &RCF_INCHANGE_CPIDX) ||!(sRCFlags &RCF_REMOTE_LOGEDIN))
    {
        return;
    }
    if(!cob_remote_model->itemData(idx,cOnlineRole).toBool())
    {
        GENIE2_UI_COMMAND(UICMD_DIALOG_WARRING
                          ,QStringList()<<GENIE2_GET_TEXT(L_RTCF_TAB_TITLE)
                          <<GENIE2_GET_TEXT(L_RTCT_MSG_ALL_OFFLINE));
        sRCFlags |=RCF_INCHANGE_CPIDX;
        cob_remote_model->setCurrentIndex(mSavedCpIdx);
        sRCFlags &=~RCF_INCHANGE_CPIDX;
        return;
    }
    mSavedCpIdx=idx;
    qDebug()<<"QGenieRCWirelessObject::slot_CpNameChaged()";
    GENIE2_SET_SETTING(PLUGIN_SMARTNETWORK_CURCP,getCpName());

    QGenieGaccObject *gacco=QGenieGaccObject::GetSingletonPtr();
    Q_ASSERT(gacco);
    QGenieTrafficMeterObject *tmo=QGenieTrafficMeterObject::GetSingletonPtr();
    Q_ASSERT(tmo);

    sRCFlags |=REFRESH_FLAGS_CLEARER;
    mSessionFirmware=-1;

    QString firmware=getCpFirmware();
    QString remote_firmware=(!getCpOnline()||firmware.isEmpty())?"":QString("%1 %2").arg(GENIE2_GET_TEXT(L_RTCF_FIRMWARE_VERSION),firmware);
    lab_remote_firmware->setText(remote_firmware);

//    if(mTab->currentWidget()==mPage1)
//    {
//        slot_RefreshWirelessSM();
//    }
//    else if(mTab->currentWidget()==gacco->getGaccUi())
//    {
//        gacco->slot_RefreshGaccInfo();
//    }
//    else if(mTab->currentWidget()==tmo->getUi())
//    {
//        tmo->slot_RefreshData();
//    }
//    updateUi();
    tryUpdateData();
}

void QGenieRCWirelessObject::slot_RememberCpCheckBoxChanged(bool b)
{
    Q_ASSERT(mReloginDlg);
//    QLineEdit *edt_pwd=qFindChild<QLineEdit *>(mReloginDlg,"edt_pwd");
//    if(!b)
//    {
////        edt_pwd->setText("");
//        mCpSecure.remove(getCpName());
//        SerializeCpSecure(mCpSecure,false);
//    }
    GENIE2_SET_SETTING(PLUGIN_SMARTNETWORK_SAVECPSECUR,b);
}

void QGenieRCWirelessObject::slot_SMUserNameChanged(const QString &text)
{
    Q_ASSERT(mButSMLogin);
    mButSMLogin->setEnabled(!text.isEmpty());
    if(text==GENIE2_GET_SETTING(PLUGIN_SMARTNETWORK_USER).toString())
    {
        sRCFlags |=RCF_SMUSER_IS_SAVEDUSER;
        mEdtSMPwd->setText(GENIE2_GET_SETTING(PLUGIN_SMARTNETWORK_PWD).toString());
    }
    else if(sRCFlags & RCF_SMUSER_IS_SAVEDUSER)
    {
        mEdtSMPwd->setText("");
        sRCFlags &= ~RCF_SMUSER_IS_SAVEDUSER;
    }
}

bool QGenieRCWirelessObject::lazyLoad()
{
    return mLazyLoader->loadStep();
}

void QGenieRCWirelessObject::translateWirelessWidget(QWidget *w)
{
    Q_ASSERT(w);

    //0.label   1.button    2.checkbox  3.radiobutton
    const struct _{const char *name;int lan;int type;}
    cPara[]={
        {"lab_title", L_RTCF_LOGIN_TITLE, 0}
        ,{"lab_usr",L_RTCF_LOGIN_USER,0}
        ,{"lab_pwd",L_RTCF_PASSWORD,0}
        ,{"lab_default",L_RTCF_DEFAULT,0}
        ,{"lab_summary",L_RTCF_LOGIN_LIST,0}
        ,{"title_ssid",L_RTCF_SETTING_SSID,0}
        ,{"title_channel",L_RTCF_SETTING_CHANNEL,0}
        ,{"title_key",L_RTCF_INFO_PASSWOED,0}
        ,{"stitle_ssid",L_RTCF_SETTING_SSID,0}
        ,{"stitle_channel",L_RTCF_SETTING_CHANNEL,0}
        ,{"stitle_secure",L_RTCF_SETTING_SECURITY,0}
        ,{"stitle_key",L_RTCF_INFO_PASSWOED,0}
        ,{"lab_words_limit",L_RTCF_SETTING_NOTE,0}
		,{"label_list_title", L_RTCF_LIST_TITLE, 0}
		,{"lab_ip", L_RTCF_IPADDRESS, 0}

        ,{"but_login_ok",L_RTCF_IMSGBOX_OK,1}
        ,{"but_login_cancel",L_RTCF_CANCEL,1}
        ,{"but_refresh1",L_RTCF_INFO_REFRESH,1}
        ,{"but_refresh2",L_RTCF_INFO_REFRESH,1}
		,{"but_modify", L_RTCF_INFO_MODIFY, 1}
		,{"but_logout", L_RTCF_INFO_LOGINOUT, 1}
		,{"but_export", L_RTCF_INFO_EXPORT, 1}
		,{"but_setting_cancel", L_RTCF_CANCEL, 1}
		,{"but_apply", L_RTCF_SETTING_APPLY, 1}

        ,{"chk_remember",L_RTCF_LOGIN_REMEMBER,2}
		,{"chk_enterIP", L_RTCF_MANUALINPUTIP, 2}
        ,{"lab_lgfrom",L_RTCF_LOGIN_FROM,0}
        ,{"lab_email",L_RTCF_LOGIN_EMAIL,0}
        ,{"lab_smpwd",L_RTCF_PASSWORD,0}
        ,{"but_smlogin",L_RTCF_IMSGBOX_OK,1}
        ,{"but_smcancel",L_RTCF_CANCEL,1}
        ,{"lab_forgot",L_RTCF_LOGIN_FOGOT,0}
        ,{"lab_signup",L_RTCF_LOGIN_SIGNUP,0}
        ,{"chk_smremember",L_RTCF_LOGIN_REMEMBER_ID,2}

        //        ,{"rdt_none"}
        //        ,{"rdt_aes"}
        //        ,{"rdt_mix"}
    };
    for(int i=0;i<sizeof(cPara)/sizeof(cPara[0]);i++)
    {
        if(0==cPara[i].type)
        {
            GENIE2_TRANSLATE_CHILD_LABEL(w,cPara[i].name,cPara[i].lan);
        }
        if(1==cPara[i].type)
        {
            GENIE2_TRANSLATE_CHILD_BUTTON(w,cPara[i].name,cPara[i].lan);
        }
        if(2==cPara[i].type)
        {
            GENIE2_TRANSLATE_CHILD_CHECKBOX(w,cPara[i].name,cPara[i].lan);
        }
    }



    translateSMLoginDialog();


}

void QGenieRCWirelessObject::translateSMLoginDialog()
{
    if(!mReloginDlg)
    {
        return;
    }
    const struct _{const char *name;int lan;int type;}
    cPara[]={
        {"lab_title",L_RTCF_LOGIN_TITLE,0}
        ,{"lab_usr",L_RTCF_LOGIN_USER,0}
        ,{"lab_pwd",L_RTCF_PASSWORD,0}
        ,{"but_accept",L_RTCF_IMSGBOX_OK,1}
        ,{"but_cancel",L_RTCF_CANCEL,1}
        ,{"chk_remember",L_RTCF_LOGIN_REMEMBER,2}
        ,{"lab_default",L_RTCF_DEFAULT,0}
    };
    for(int i=0;i<sizeof(cPara)/sizeof(cPara[0]);i++)
    {
        if(0==cPara[i].type)
        {
            GENIE2_TRANSLATE_CHILD_LABEL(mReloginDlg,cPara[i].name,cPara[i].lan);
        }
        if(1==cPara[i].type)
        {
            GENIE2_TRANSLATE_CHILD_BUTTON(mReloginDlg,cPara[i].name,cPara[i].lan);
        }
        if(2==cPara[i].type)
        {
            GENIE2_TRANSLATE_CHILD_CHECKBOX(mReloginDlg,cPara[i].name,cPara[i].lan);
        }
    }


}

void QGenieRCWirelessObject::whenLanChaned()
{
//    QString s=(mLanLoginErrorLocal<0)?"":QString(ERROR_MODE).arg(GENIE2_GET_TEXT(mLanLoginErrorLocal));
//    if(lab_login_error)
//    {
//        lab_login_error->setText(s);
//    }
    sRCFlags |=RCF_INCHANGELANGUAGE;

    Q_ASSERT(mErrorLabel);
    if(mErrorLabel)
    {
        mErrorLabel->setText((mLanError<0)?"":GENIE2_GET_TEXT(mLanError));
    }

    QGenieFupdateObject *fupo=QGenieFupdateObject::GetSingletonPtr();
    QGenieTrafficMeterObject *tmo=QGenieTrafficMeterObject::GetSingletonPtr();
    QGenieGaccObject *gacco=QGenieGaccObject::GetSingletonPtr();
	QGenieRebootObject*rebootO = QGenieRebootObject::GetSingletonPtr();
    Q_ASSERT(tmo && fupo&& gacco && rebootO);

    const struct _{QWidget *w;int lan_idx;} cParas[]=
    {
        {mPage1,L_RTCF_TAB_TITLE}
        ,{fupo->getUi(),L_UPGRADE_MAIN_NAME}
        ,{gacco->getGaccUi(),L_GACC_BIGBT}
        ,{tmo->getUi(),L_TRAFFICMETER_BIGBT}
        ,{mPcpForm,L_READY_SHARE_BIGBT}
		,{rebootO->getRebootUi(),L_OTHERS_TITLE}
    };
    for(int i=0;i<sizeof(cParas)/sizeof(cParas[0]);i++)
    {
        int itemp=mTab->indexOf(cParas[i].w);
        if(itemp >=0)
        {
            mTab->setTabText(itemp,GENIE2_GET_TEXT(cParas[i].lan_idx));
        }
    }

//	///////2012-3-6 Chensi//////////////
//	QString tabFirmUpdate = GENIE2_GET_TEXT(L_UPGRADE_MAIN_NAME);
//	QString tabWiress = GENIE2_GET_TEXT(L_RTCF_TAB_TITLE);
//	QString tabReadyShare = GENIE2_GET_TEXT(L_READY_SHARE_BIGBT);
//	QString tabGACC = GENIE2_GET_TEXT(L_GACC_BIGBT);
//	QString tabTra = GENIE2_GET_TEXT(L_TRAFFICMETER_BIGBT);
	
//	mTab->setTabText(0, tabWiress);
//	mTab->setTabText(1, tabReadyShare);
//	mTab->setTabText(2, tabGACC);
//	mTab->setTabText(3, tabTra);
//	mTab->setTabText(4, tabFirmUpdate);

    Q_ASSERT( lab_model && lab_firmware && lab_local_model
              && lab_remote_firmware);
//	QString strModel = QString("%1 %2").arg(GENIE2_GET_TEXT(L_RTCF_ROUTETYPE)).arg(mModel);
    QString strFirmware=GENIE2_GET_TEXT(L_RTCF_FIRMWARE_VERSION);
    QString local_firmware = QString("%1 %2").arg(strFirmware,mFirmware);
    lab_model->setText(GENIE2_GET_TEXT(L_RTCF_ROUTETYPE));
    lab_firmware->setText(local_firmware);
    QString remote_firmware=getCpFirmware();
    remote_firmware=remote_firmware.isEmpty()?"":QString("%1 %2").arg(strFirmware,remote_firmware);
    lab_remote_firmware->setText(remote_firmware);

    int itemp=cob_loginmode->currentIndex();
    cob_loginmode->clear();
    qDebug()<< GENIE2_GET_TEXT(L_RTCF_LOGIN_LOCAL)<<GENIE2_GET_TEXT(L_RTCF_LOGIN_REMOTE);
    cob_loginmode->addItem(GENIE2_GET_TEXT(L_RTCF_LOGIN_LOCAL));
    cob_loginmode->addItem(GENIE2_GET_TEXT(L_RTCF_LOGIN_REMOTE));
    cob_loginmode->setCurrentIndex(itemp);


    if(cob_remote_model)
    {
        sRCFlags |=RCF_INCHANGE_CPIDX;
        int c=cob_remote_model->count();
        for(int i=0;i<c;i++)
        {
            bool online=cob_remote_model->itemData(i,cOnlineRole).toBool();
            QString model=cob_remote_model->itemData(i,cRouterModelRole).toString();
            QString serial=cob_remote_model->itemData(i,cSerialRole).toString();

            cob_remote_model->setItemText(i,QString(SM_COB_MOD)
                                          .arg(model,serial,online
                                               ?GENIE2_GET_TEXT(L_RTCT_ONLINE)
                                              :GENIE2_GET_TEXT(L_RTCT_OFFLINE))
                                          );

        }
        sRCFlags &=~RCF_INCHANGE_CPIDX;
    }




    sRCFlags &=~RCF_INCHANGELANGUAGE;
    updateUi();
}

void QGenieRCWirelessObject::deployIndicators(QWidget *w)
{
    Q_ASSERT(w);
    const struct _{const char *name;QPiObject**p;}
    cParas[]={
        {"fr_indicator_login",&mIndicatorLogin}
        ,{"fr_indicator",&mIndicatorWireless}
        ,{"fr_indicator1",&mIndicatorWirelessSetting}
    };
    for(int i=0;i<sizeof(cParas)/sizeof(cParas[0]);i++)
    {
        Q_ASSERT(*(cParas[i].p)==NULL);
        QFrame *f=w->findChild<QFrame *>(cParas[i].name);
        Q_ASSERT(f);
        QPiObject *pio=new QPiObject(w,cParas[i].name);
        *(cParas[i].p)=pio;
    }
}
void QGenieRCWirelessObject::slot_RefreshWireless()
{

    bool _b5G=(sRCFlags&RCF_LOCAL_WIRELESS_5GUNLOCKED)&&(sRCFlags&RCF_5G_TABCHECKED);
    mStackWC->setCurrentIndex(0);
    refreshWireless(false,_b5G);
}

void QGenieRCWirelessObject::slot_RefreshWirelessSM()
{
    bool _b5G=(sRCFlags&RCF_REMOTE_WIRELESS_5GUNLOCKED)&&(sRCFlags&RCF_5G_TABCHECKED);
    mStackWC->setCurrentIndex(0);
    refreshWireless(true,_b5G);
}

void QGenieRCWirelessObject::refreshWireless(bool remote,bool _5G)
{
    Q_ASSERT(mWirelessW);
    Q_ASSERT(mStackTotal );
//    Q_ASSERT(mStackTotal && (mStackTotal->currentIndex()==1));
    Q_ASSERT(mIndicatorWireless && mIndicatorWirelessSetting);
    QString na="N/A";
	QString str = mEdtSsid->text();
    mEdtSsid->setText(na);
    mEdtKey->setText(na);
    QList<QStackedWidget *> stacklst=mWirelessW->findChildren<QStackedWidget *>("stack_wc");
    Q_ASSERT(stacklst.count()==1);
    QStackedWidget *stack_wc=stacklst[0];
    Q_ASSERT(stack_wc);
    const char *cPara[]={"lab_ssid","lab_channel","lab_key"};
    //QString na="N/A";
    for(int i=0;i<sizeof(cPara)/sizeof(cPara[0]);i++)
    {
        QLabel *lab=stack_wc->findChild<QLabel*>(cPara[i]);
        Q_ASSERT(lab);
        lab->setText(na);

    }
    mQRCM->hideWidget(QGenieQRCodeManager::QWT_WIRELESS);

    mIndicatorWireless->startAnimation();
    mIndicatorWirelessSetting->startAnimation();
    mSessionWireless=mSessionWireless5G=-1;
    if(remote)
    {
        Q_ASSERT(sRCFlags & RCF_REMOTE);
        Q_ASSERT(sRCFlags &RCF_REMOTE_LOGEDIN);
        QStringList paras;

        paras<<getCpName();

        if(_5G)
        {
            sRCFlags &=~RCF_MUST_REFRESH_WIRELESS_5G;
            mSessionWireless5G=QGenieIface2ShareObject::sSingleInstance->mNetgearInfoM
                    ->netgearCommand(INetgearInterface::IC_SM_GET_5GINFO,paras,this,SLOT(slot_SMDataReturned(int,QStringList)));

        }
        else
        {
            sRCFlags &=~RCF_MUST_REFRESH_WIRELESS;
            mSessionWireless=QGenieIface2ShareObject::sSingleInstance->mNetgearInfoM
                    ->netgearCommand(INetgearInterface::IC_SM_WIRELESS,paras,this,SLOT(slot_SMDataReturned(int,QStringList)));
        }
    }
    else
    {
        Q_ASSERT(!(sRCFlags &RCF_REMOTE));
        Q_ASSERT(sRCFlags & RCF_LOCAL_LOGEDIN);
        if(_5G)
        {
            sRCFlags &=~RCF_MUST_REFRESH_WIRELESS_5G;
            mSessionWireless5G=QGenieIface2ShareObject::sSingleInstance->mNetgearInfoM
                    ->netgearCommand(INetgearInterface::IC_GET_5GINFO,QStringList(),this,SLOT(slot_DataReturned(int,QStringList)));

        }
        else
        {
            sRCFlags &=~RCF_MUST_REFRESH_WIRELESS;
            mSessionWireless=QGenieIface2ShareObject::sSingleInstance->mNetgearInfoM
                    ->netgearCommand(INetgearInterface::IC_GET_WIRELESS,QStringList(),this,SLOT(slot_DataReturned(int,QStringList)));
        }
    }
//    mStackWC->setCurrentIndex(0);
    setError(-1);
    if(remote)
    {
        Q_ASSERT(sRCFlags & RCF_REMOTE);
        sRCFlags |= RCF_IN_REMOTE_REFRESHING;
    }
    else
    {
        Q_ASSERT(!(sRCFlags & RCF_REMOTE));
        sRCFlags |= RCF_IN_LOCAL_REFRESHING;
    }
//    mWirelessW->setEnabled(false);

}

QString QGenieRCWirelessObject::getCpName() const
{
    if(!cob_remote_model)
    {
        return "";
    }
    int idx=cob_remote_model->currentIndex();
    Q_ASSERT(idx>=0);
    return cob_remote_model->itemData(idx).toString();
}

bool QGenieRCWirelessObject::getCpOnline() const
{
    if(!cob_remote_model)
    {
        return false;
    }
    int idx=cob_remote_model->currentIndex();
    Q_ASSERT(idx >=0);
    return cob_remote_model->itemData(idx,cOnlineRole).toBool();
}

QString QGenieRCWirelessObject::getCpFirmware() const
{
    if(!cob_remote_model)
    {
        return "";
    }
    int idx=cob_remote_model->currentIndex();
    if(idx<0)
	{
		return "";
	}
    return cob_remote_model->itemData(idx,cFirmwareRole).toString();
}


QString QGenieRCWirelessObject::getModelName() const
{
    if(!cob_remote_model)
    {
        return "";
    }
    int idx=cob_remote_model->currentIndex();
    Q_ASSERT(idx>=0);
    return cob_remote_model->itemData(idx,cRouterModelRole).toString();
}

void QGenieRCWirelessObject::slot_buttonClicked()
{
    QObject *obj=sender();
    Q_ASSERT(obj);
    Q_ASSERT(mWirelessW);
    QString obj_name=obj->objectName();
    if(("but_login_ok"==obj_name)|| ("but_smlogin"==obj_name))
    {

        QGenieDelayProcessLogic::GetSingleton().whenLoginButton();
        if((mEdtPwd->text()!="password")&& !IS_REMOTE_UI())
        {
            GENIE2_STATISTICS("R1");
        }
        startLogin("but_smlogin"==obj_name);
    }
    else if("but_logout"==obj_name )
    {
        logOut(IS_REMOTE_UI());
    }
    else if("but_refresh2"==obj_name ||"but_refresh1"==obj_name)
    {
        bool _b5G0=!IS_REMOTE_UI()&&(sRCFlags&RCF_LOCAL_WIRELESS_5GUNLOCKED)&&(sRCFlags&RCF_5G_TABCHECKED);
        bool _b5G1=IS_REMOTE_UI()&&(sRCFlags&RCF_REMOTE_WIRELESS_5GUNLOCKED)&&(sRCFlags&RCF_5G_TABCHECKED);
        refreshWireless(IS_REMOTE_UI(),_b5G0||_b5G1);
    }
    else if("but_setting_cancel"==obj_name)
    {
        Q_ASSERT(mStackWC);
        mStackWC->setCurrentIndex(0);
    }
    else if("but_modify"==obj_name)
    {
        Q_ASSERT(mStackWC);
        updateSettingPage();
        if(!mLDGetter->tryUpdateLackedData(QGenieRCLackDataGetter::ER_WIRELESS))
        {
            mStackWC->setCurrentIndex(1);
        }
    }
    else if("but_apply"==obj_name)
    {
        QStringList dialog_lans;
        dialog_lans<<GENIE2_GET_TEXT(L_RTCF_TAB_TITLE)<<GENIE2_GET_TEXT(L_RTCF_MSG_NOTIFYCUT);
        sRCFlags |= RCF_MODAL_DIALOG_ON;
        bool dialogOk=GENIE2_UI_COMMAND(UICMD_DIALOG_MODAL,dialog_lans).toBool();
        sRCFlags &=~RCF_MODAL_DIALOG_ON;
        if(!dialogOk)
        {
            return;
        }
        GENIE2_STATISTICS("R3");
        QStringList para;
//        QLineEdit *sedt_ssid=qFindChild<QLineEdit *>(mWirelessW,"sedt_ssid");
//        QLineEdit *sedt_key=qFindChild<QLineEdit *>(mWirelessW,"sedt_key");
//        QRadioButton *rdt_none=qFindChild<QRadioButton *>(mWirelessW,"rdt_none");
//        QRadioButton *rdt_aes=qFindChild<QRadioButton *>(mWirelessW,"rdt_aes");
////        QRadioButton *rdt_mix=qFindChild<QRadioButton *>(mWirelessW,"rdt_mix");
//        QComboBox *cob=qFindChild<QComboBox *>(mWirelessW,"cob_channel");

        QSharedPointer<tRCData> data=IS_WIRELESS_5G()?mSavedData:mSavedData_5G;
        Q_ASSERT(mEdtSsid&&mEdtKey&&mRdtNone&&mRdtMix&&mRdtAES&&mCobChannel);
        QString ssid=mEdtSsid->text();
        QString pwd=mEdtKey->text();
        QString secure=mRdtNone->isChecked()?"None":
                       (mRdtAES->isChecked()?"WPA2-PSK":"WPA-PSK/WPA2-PSK");
        QString rz=mRdtNone->isChecked()?"open":"WPA2PSK";
        QString jm=mRdtNone->isChecked()?"none":"AES";
        if(IS_REMOTE_UI())
        {
            para<<getCpName();
        }
        para<<ssid<<data->mSavedRegion<<mCobChannel->currentText()<<data->mSavedMod<<secure<<pwd;
        if(!IS_REMOTE_UI())
        {
			QString nullS;
            if(IS_WIRELESS_5G())
            {
                
                mTimelineLocker->setNeedReconnect(nullS,nullS,ssid,pwd);
            }
            else
            {
                mTimelineLocker->setNeedReconnect(ssid,pwd,nullS,nullS);
            }
            mTimelineLocker->setNotifySsidData(ssid,pwd,jm,rz);
        }

        QGenieTimeGenerator gener;
        mTimelineLocker->setNeedSingleFinish(!IS_REMOTE_UI());
        if(IS_REMOTE_UI())
        {
            mTimelineLocker->startTimer(SMARTNETWORK_WAIT_TIME*1000,this,SLOT(slot_RefreshWirelessSM())/*,mIndicatorWirelessSetting*/);

            QGenieIface2ShareObject::sSingleInstance->mNetgearInfoM
                    ->netgearCommand(INetgearInterface::IC_SM_SET_WIRELESS,para,NULL,NULL);

        }
        else
        {
            int t=gener.wirelessSettingWaitTime(mModel);

            mTimelineLocker->startTimer(t*1000,this,SLOT(slot_RefreshWireless())/*,mIndicatorWirelessSetting*/);
            if(IS_WIRELESS_5G())
            {
                qDebug()<<"******modify 5G*********";
                QGenieIface2ShareObject::sSingleInstance->mNetgearInfoM
                        ->netgearCommand(INetgearInterface::IC_SET_5GINFO,para,NULL,NULL);
            }
            else
            {
                qDebug()<<"******modify 2.4G*********";
                QGenieIface2ShareObject::sSingleInstance->mNetgearInfoM
                        ->netgearCommand(INetgearInterface::IC_SET_WIRELESS,para,NULL,NULL);
            }

        }

    }
    else if(("but_login_cancel"==obj_name) || ("but_smcancel"==obj_name))
    {
        if(IS_REMOTE_UI())
        {
            Q_ASSERT("but_smcancel"==obj_name);
            sRCFlags &= ~(RCF_REMOTE_LOGEDIN | RCF_IN_REMOTE_LOGIN_PROCESS|ALL_REMOTE_CMD);
            mSessionSMLogin=-1;
        }
        else
        {
            sRCFlags &=~(RCF_LOCAL_LOGEDIN | RCF_IN_LOCAL_LOGIN_PROCESS|ALL_LOCAL_CMD);
            mSessionLogin=-1;
        }


        setLoginError(-1,IS_REMOTE_UI());
    }
#ifdef USE_SUBPLUGINS
    else if("but_export"==obj_name)
    {
        GENIE2_TELL_OTHERPLUGIN(mSubpluginExportUuid,EPCC_TURNTO_EXPORT,0);
        mStackWC->setCurrentWidget(mWExportW0);
    }
#endif
    else if("but_test"==obj_name)
    {
        sRCFlags |= RCF_MODAL_DIALOG_ON;
        GENIE2_UI_COMMAND(UICMD_DIALOG_MODAL,QStringList()<<"aaaaa"<<"bbbbb");
        sRCFlags &=~RCF_MODAL_DIALOG_ON;

    }
    else if("lab_forgot"==obj_name)
    {
//        mEdtSMPwd->clear();
//        mEdtSMUsr->clear();
//        GENIE2_SET_SETTING(PLUGIN_SMARTNETWORK_USER,"");
//        GENIE2_SET_SETTING(PLUGIN_SMARTNETWORK_PWD,"");

        QString url=GENIE2_GET_SETTING(FOGOT_PWD_LINK).toString();
        QDesktopServices::openUrl(QUrl(url));

    }
    else if("lab_signup"==obj_name)
    {
        QString url=GENIE2_GET_SETTING(SINGNUP_LINK).toString();
        QDesktopServices::openUrl(QUrl(url));
    }
    updateUi();
}

void QGenieRCWirelessObject::startLogin(bool remote)
{
   // Q_ASSERT(login_frame && login_frame->isEnabled());
    if(remote)
    {
      //  Q_ASSERT(!(sRCFlags & (RCF_REMOTE_LOGEDIN|RCF_IN_REMOTE_LOGIN_PROCESS)));
        sRCFlags |= RCF_IN_REMOTE_LOGIN_PROCESS;
    }
    else
    {
        //Q_ASSERT(!(sRCFlags & (RCF_LOCAL_LOGEDIN|RCF_IN_REMOTE_REFRESHING)));
        sRCFlags |= RCF_IN_LOCAL_LOGIN_PROCESS;
    }

//    for(int i=0;i<sizeof(cLoginDisableWs)/sizeof(cLoginDisableWs[0]);i++)
//    {
//        QWidget *w=qFindChild<QWidget *>(login_frame,cLoginDisableWs[i]);
//        Q_ASSERT(w);
//        w->setEnabled(false);
//    }

    if(remote)
    {
        Q_ASSERT(mEdtSMPwd&& mEdtSMUsr);
        QStringList para;
        Q_ASSERT((mEdtSMUsr->text()!="larkvm@gmail.com")||(mEdtSMPwd->text()=="12345678"));
        para<<mEdtSMUsr->text()<<mEdtSMPwd->text();
        mSessionSMLogin=QGenieIface2ShareObject::sSingleInstance->mNetgearInfoM
                      ->netgearCommand(INetgearInterface::IC_SM_LOGIN,para,this,SLOT(slot_SMDataReturned(int,QStringList)));
        if(!chk_smremember->isChecked())
        {
            if(mEdtSMUsr->text()==GENIE2_GET_SETTING(PLUGIN_SMARTNETWORK_USER).toString())
            {
                mCpSecure.clear();
                SerializeCpSecure(mCpSecure,false);
                GENIE2_SET_SETTING(PLUGIN_SMARTNETWORK_PWD,"");
            }
//            mEdtSMPwd->clear();
//            mEdtSMUsr->clear();
        }

    }
    else
    {
        Q_ASSERT(mEdtUsr && mEdtPwd);
        QStringList para;
        para<<mEdtUsr->text()<<mEdtPwd->text();
		if(chk_inputIP->isChecked())
		{
			QString ip = mEdtIP->text().trimmed();
			QString qStrIPRegExp =
				"((([1-9][0-9]?)|(1[0-9]?[0-9]?)|(2[0-4]?[0-9])|(25?[0-5]))\\.){1}"
				"((0|([1-9][0-9]?)|(1[0-9]?[0-9]?)|(2[0-4]?[0-9])|(25?[0-5]))\\.){2}"
				"(0|([1-9][0-9]?)|(1[0-9]?[0-9]?)|(2[0-4]?[0-9])|(25?[0-5])){1}";
			QRegExp ipRegExp(qStrIPRegExp);
			if(!ipRegExp.exactMatch(ip))
			{
				sRCFlags &=~(RCF_LOCAL_LOGEDIN | RCF_IN_LOCAL_LOGIN_PROCESS|ALL_LOCAL_CMD);
				mSessionLogin=-1;
				setLoginError(L_RTCF_MSG_ERROR_IP,false);
				return;
			}else
			{
				para<<ip;
			}
	
		}else
		{
			para<<"routerlogin.net";
		}
        mSessionLogin=QGenieIface2ShareObject::sSingleInstance->mNetgearInfoM
                      ->netgearCommand(INetgearInterface::IC_LOGIN,para,this,SLOT(slot_DataReturned(int,QStringList)));

        if(!chk_remmember->isChecked())
        {
//            mEdtPwd->clear();
            GENIE2_SET_SETTING(PLUGIN_ROUTECONF_PWD,"");
            GENIE2_STATISTICS("R2");
        }



    }
    Q_ASSERT(mIndicatorLogin);
//    mSavedMac="";
//    mIndicatorLogin->startAnimation();
    setLoginError(-1,remote);

}


void QGenieRCWirelessObject::changeRemoteState(bool bremote)
{
    if(bremote)
    {
        sRCFlags|=RCF_REMOTE;
    }
    else
    {
        sRCFlags &=~RCF_REMOTE;
    }
#if 0
    if(bremote)
    {
        GENIE2_UI_COMMAND2(UICMD_LEFTBUTTON_LANGUAGE,"",1);
    }
    else
    {
        GENIE2_UI_COMMAND2(UICMD_LEFTBUTTON_LANGUAGE,"ReadySHARE",1);
    }
#endif
    sRCFlags |=REFRESH_FLAGS_CLEARER;
//    updateUi();
//    tryUpdateData();
    mTab->setCurrentIndex(0);
    mSavedTabW=mPage1;
    GENIE2_SET_SETTING(PLUGIN_SMARTNETWROK_ISREMOTE,bremote);
}

void QGenieRCWirelessObject::updateUi()
{
	if(!mWirelessW)
	{
		return;
	}
    sRCFlags |= RCF_INCHANGE_UI;

    //udpate stack
    Q_ASSERT(mStackTotal);





    //update tabIdx
//    QWidget *wsaved=mTab->currentWidget();
    static enum TAB_CONTENT_MODE{
        TCM_LOCAL_WIRELESS
        ,TCM_REMOTE_WIRELESS
        ,TCM_READYSHARE
        ,TCM_NUM
        ,TCM_INIT
    } sContendMode=TCM_INIT;

    TAB_CONTENT_MODE curMode=(sRCFlags & RCF_READYSHARE_MODE)
            ?TCM_READYSHARE:((sRCFlags&RCF_REMOTE)?TCM_REMOTE_WIRELESS:TCM_LOCAL_WIRELESS);
    if(/*(curMode != sContendMode )&& */(mLazyLoader->mCurStep==_LazyLoader::LOAD_FINISH))///kai.yan modify for QoS
    {
        sContendMode=curMode;
        QGenieGaccObject *gaccObj=QGenieGaccObject::GetSingletonPtr();
        QGenieTrafficMeterObject *tmobj=QGenieTrafficMeterObject::GetSingletonPtr();
        QGenieFupdateObject *fupo=QGenieFupdateObject::GetSingletonPtr();
        QGenieRCTopologyObject *tpo=QGenieRCTopologyObject::GetSingletonPtr();
		QGenieQosObject *qoso = QGenieQosObject::GetSingletonPtr(); ///kai.yan add for qos;
        QGenieRebootObject *rebooto = QGenieRebootObject::GetSingletonPtr();///kai.yan add for reboot
        Q_ASSERT(gaccObj&& tmobj && fupo && tpo && qoso && rebooto);
        struct _{QWidget *w;int show[TCM_NUM];int lan;} cParas[]={
            {mPage1,{1,1,0},L_RTCF_TAB_TITLE}
            ,{mPcpForm,{1,0,1},L_READY_SHARE_LIVE}
	    ,{qoso->getQosUi(),{/*m_QosState*/0,0,0},L_QOSSETING_TITLE}
            ,{gaccObj->getGaccUi(),{1,1,0},L_GACC_BIGBT}
            ,{tmobj->getUi(),{1,1,0},L_TRAFFICMETER_BIGBT}
            ,{fupo->getUi(),{1,0,0},L_UPGRADE_MAIN_NAME}
            ,{rebooto->getRebootUi(),{1,0,0},L_OTHERS_TITLE}
            
        };

        mTab->clear();
        for(int i=0;i<sizeof(cParas)/sizeof(cParas[0]);i++)
        {
            if(cParas[i].show[curMode])
            {
                mTab->insertTab(mTab->count(),cParas[i].w,GENIE2_GET_TEXT(cParas[i].lan));
            }
        }
        int itemp=mTab->indexOf(mSavedTabW);
        if((sRCFlags &RCF_ALL_CP_OFFLINE)
            &&IS_REMOTE_UI())
        {
            mTab->setCurrentIndex(0);
            mStackWC->setCurrentIndex(0);

         }
        else
        {

            mTab->setCurrentIndex((itemp<0)?0:itemp);


            tryUpdateData();
        }
    }//if(((curMode != sContendMode )&& (mLazyLoader->mCurStep==_LazyLoader::LOAD_FINISH))

    QList<QTabBar *> lst_bar=mTab->findChildren<QTabBar *>();
    Q_ASSERT(lst_bar.size()==1);
  //  QTabBar *headtabbar=(lst_bar[0]->shape()==QTabBar::RoundedNorth)?lst_bar[0]:lst_bar[1];
	QTabBar *headtabbar=lst_bar[0];
    if((sRCFlags &RCF_ALL_CP_OFFLINE)
        &&IS_REMOTE_UI()
            &&(sRCFlags & RCF_REMOTE_LOGEDIN))
    {
        Q_ASSERT(mTab->currentIndex()==0);

        headtabbar->setEnabled(false);

        if(!mLockerWhenAllCPOffline)
        {
            mLockerWhenAllCPOffline=new LockerWhenAllCPOffline(mStackWC->widget(0));
        }

    }
    else
    {
        headtabbar->setEnabled(true);
        if(mLockerWhenAllCPOffline)
        {
            mLockerWhenAllCPOffline->deleteLater();
            mLockerWhenAllCPOffline=NULL;
        }

    }






    //update buttom bar
    lab_local_model->setVisible(!IS_REMOTE_UI());
    cob_remote_model->setVisible(IS_REMOTE_UI());
    lab_firmware->setVisible(!IS_REMOTE_UI());
    lab_remote_firmware->setVisible(IS_REMOTE_UI());

    //update mainframe data
    if((sRCFlags & RCF_HAVE_FIRMWARE_UPDATE)
            &&!(sRCFlags&(RCF_REMOTE_LOGEDIN|RCF_IN_REMOTE_LOGIN_PROCESS)))
    {
        GENIE2_UI_COMMAND(UICMD_TITLE_UPDATE,"firm_on");
    }
    else
    {
        GENIE2_UI_COMMAND(UICMD_TITLE_UPDATE,"firm_off");
    }

//    sRCFlags &= ~RCF_INCHANGE_UI;

    QGenieDelayProcessLogic::GetSingleton().tryProcessPendedCmd();

    //update locker
   _updateLockState();

   //update login page
   const char *cLoginDisableWs[]={"edt_pwd"
                                  ,"chk_remember","but_login_ok"
                                  ,"edt_email","edt_smpwd","but_smlogin","chk_smremember"
                                  ,"lab_forgot","lab_signup","cob_loginmode"
								  ,"chk_enterIP", "frame_ip"};
   bool bInLogin=(
           (IS_REMOTE_UI() && (sRCFlags & RCF_IN_REMOTE_LOGIN_PROCESS))
           ||(!IS_REMOTE_UI() && (sRCFlags &RCF_IN_LOCAL_LOGIN_PROCESS))
           );

   for(int i=0;i<sizeof(cLoginDisableWs)/sizeof(cLoginDisableWs[0]);i++)
   {
       QWidget *w=login_frame->findChild<QWidget *>(cLoginDisableWs[i]);
       Q_ASSERT(w);
       w->setEnabled(!bInLogin);
   }
   if(bInLogin)
   {
       mIndicatorLogin->startAnimation();
   }
   else
   {
       mIndicatorLogin->stopAnimation();
   }


   bool bShowLogedIn=(
               (IS_REMOTE_UI()&&(sRCFlags & RCF_REMOTE_LOGEDIN))
               ||(!IS_REMOTE_UI() && (sRCFlags & RCF_LOCAL_LOGEDIN))
               );
   mStackTotal->setCurrentIndex(bShowLogedIn?1:0);

   if(!bShowLogedIn)
   {
       int err=IS_REMOTE_UI()?mLanLoginErrorRemote:mLanLoginErrorLocal;
       QString text=(err <0)?"":QString(ERROR_MODE).arg(GENIE2_GET_TEXT(err));
       lab_login_error->setText(text);
   }
   if(!bShowLogedIn )
   {
       if(sRCFlags & RCF_READYSHARE_MODE)
       {
           cob_loginmode->setCurrentIndex(0);
           cob_loginmode->setEnabled(false);

       }
       else
       {
           cob_loginmode->setCurrentIndex((sRCFlags &RCF_REMOTE)?1:0);
//            cob_loginmode->setEnabled(true);
           if(sRCFlags &RCF_REMOTE)
           {
               mButSMLogin->setEnabled(!mEdtSMUsr->text().isEmpty() && !(sRCFlags &RCF_IN_REMOTE_LOGIN_PROCESS));
           }
       }
   }

  

   bool bBusy=(sRCFlags
               &(RCF_REMOTE_LOGEDIN|RCF_IN_REMOTE_LOGIN_PROCESS|RCF_IN_LOCAL_REFRESHING
                |RCF_TIMELINELOCKER_WORKING)
               );
   GENIE2_NOTIFY_MESSAGE(SCCMD_ROUTERCONFIG_BUSY,bBusy);
#ifdef USE_5GFUNC
    update5GUi();
#endif
    QGenieTimelineLocker *locker=QGenieTimelineLocker::GetSingletonPtr();
    if(locker)
    {
        locker->whenUpdateUi();
    }


   sRCFlags &= ~RCF_INCHANGE_UI;
    mWirelessW->update();
}

#ifdef USE_5GFUNC
void QGenieRCWirelessObject::update5GUi()
{
    QWidget *curTabW=mTab->currentWidget();

    if(!curTabW)
    {
        return;
    }
    QGenieGaccObject *gaccObj=QGenieGaccObject::GetSingletonPtr();
    Q_ASSERT(gaccObj);
    bool showTab=false;
    bool showFlagW=false;

    if(!IS_REMOTE_UI() && (sRCFlags & RCF_LOCAL_LOGEDIN))
    {
        bool b0=(sRCFlags & RCF_LOCAL_WIRELESS_5GUNLOCKED)&&(curTabW==mPage1);
        bool b1=(sRCFlags & RCF_LOCAL_GACC_5GUNLOCKED)&&(curTabW==gaccObj->getGaccUi());
//        showFlagW=b0||b1;
        showFlagW=(curTabW==mPage1)||(curTabW==gaccObj->getGaccUi());
        showTab=(b0&&(0==mStackWC->currentIndex()))
                ||(b1&&(0==gaccObj->stack_gacc->currentIndex()));
        qDebug()<<"RCF_LOCAL_GACC_5GUNLOCKED:"<<(sRCFlags & RCF_LOCAL_GACC_5GUNLOCKED);
        qDebug()<<"update5GUi:"<<b0<<"b1:"<<b1<<"showTab:"<<showTab;
    }
    else if(IS_REMOTE_UI() && (sRCFlags & RCF_REMOTE_LOGEDIN))
    {
        bool b0=(sRCFlags & RCF_REMOTE_WIRELESS_5GUNLOCKED)&&(curTabW==mPage1);
        bool b1=(sRCFlags & RCF_REMOTE_GACC_5GUNLOCKED)&&(curTabW==gaccObj->getGaccUi());
//        showFlagW=b0||b1;
        showFlagW=(curTabW==mPage1)||(curTabW==gaccObj->getGaccUi());
        showTab=(b0&&(0==mStackWC->currentIndex()))
                ||(b1&&(0==gaccObj->stack_gacc->currentIndex()));
    }

    //kai.yan delete 2.4G/2.5G info
//    m5GTab->setVisible(showTab);
     m5GTab->setVisible(false);
//    m5GFlagLabel->setVisible(showFlagW);
     m5GFlagLabel->setVisible(false);

    bool bFlagIs5G=((curTabW==mPage1)&&IS_WIRELESS_5G())
            ||((curTabW==gaccObj->getGaccUi())&&IS_GACC_5G());
    quint64 savedFlags=sRCFlags;
    sRCFlags=bFlagIs5G?(sRCFlags|RCF_IS_SHOWING_5GFLAG):(sRCFlags&~RCF_IS_SHOWING_5GFLAG);
    if(savedFlags!=sRCFlags)
    {
        if(sRCFlags & RCF_IS_SHOWING_5GFLAG)
        {
            m5GFlagLabel->setPixmap(GENIE2_RES("images/conner_5.png"));
        }
        else
        {
            m5GFlagLabel->setPixmap(GENIE2_RES("images/conner_24.png"));
        }
    }

}

void QGenieRCWirelessObject::slot_5GTabIndexChanged(int idx)
{
    qDebug()<<"QGenieRCWirelessObject::slot_5GTabIndexChanged";
    sRCFlags &= ~RCF_5G_TABCHECKED;
    if(idx!=0)
    {
        sRCFlags |=RCF_5G_TABCHECKED;
    }

    if(!(sRCFlags &RCF_IN_LOGOUT_FUNCTION))
    {

        setError(-1);
        updateShowingPage();
        updateSettingPage();
        QGenieGaccObject *gacco=QGenieGaccObject::GetSingletonPtr();
        QWidget *curTabW=mTab->currentWidget();
        if(gacco/*&&(curTabW==gacco->getGaccUi())*/)
        {
            gacco->setError(-1);
            gacco->updateShowingPage();
            gacco->updateSettingPage();
        }
        tryUpdateData();

        updateUi();
    }
}

void QGenieRCWirelessObject::slot_Refresh5GUiWhenModifyModeChanged()
{
    qDebug()<<"QGenieRCWirelessObject::slot_Refresh5GUiWhenModifyModeChanged";
    update5GUi();
}

#endif

void QGenieRCWirelessObject::_updateLockState()
{
    bool bLockUi=!(sRCFlags&RCF_READYSHARE_MODE)&&(
                (IS_REMOTE_UI()&&(sRCFlags &(RCF_IN_REMOTE_REFRESHING)))
                ||(!IS_REMOTE_UI()&&(sRCFlags &RCF_IN_LOCAL_REFRESHING))
                );
    mWirelessW->setEnabled(!bLockUi); 
}

void QGenieRCWirelessObject::loginOk(bool remote)
{

    while(mLazyLoader->loadStep());




//    GENIE2_SET_SETTING(PLUGIN_SMARTNETWROK_ISREMOTE,remote);
    bool bReadyShareLogedin=(!remote)&&(sRCFlags&RCF_REMOTE);
    if(!bReadyShareLogedin)
    {
        QTellLPCHelper *tlpc=QTellLPCHelper::GetSingletonPtr();
        Q_ASSERT(tlpc);
        tlpc->whenLoginOk(remote,mEdtSMUsr->text(),mEdtSMPwd->text());
    }

    Q_ASSERT(chk_remmember && chk_smremember);
    if(remote)
    {
        QGenieIface2ShareObject::sSingleInstance->mNetgearInfoM->setConfiguration
                (INetgearInterface::IS_SM_REMOVE_SECURE,QStringList());

        SerializeCpSecure(mCpSecure,true);
        QMap<QString,QPair<QString,QString> >::const_iterator it=mCpSecure.constBegin();
        while(it!=mCpSecure.constEnd())
        {
            qDebug()<<it.key()<<it.value().first<<it.value().second;
            QGenieIface2ShareObject::sSingleInstance->mNetgearInfoM->setConfiguration
                    (INetgearInterface::IS_SM_CPLOGIN
                     ,QStringList()<<it.key()<<it.value().first<<it.value().second);
            it++;
        }

        QString usrName=mEdtSMUsr->text();
        if(mCompleteModel->findItems(usrName).isEmpty())
        {
            mCompleteModel->insertRow(mCompleteModel->rowCount());
            QModelIndex index=mCompleteModel->index(mCompleteModel->rowCount()-1,0);
            mCompleteModel->setData(index,usrName);
        }


        if(chk_smremember->isChecked())
        {
            QString oldUser=GENIE2_GET_SETTING(PLUGIN_SMARTNETWORK_USER).toString();
            const QString &newUser=usrName;
            if(oldUser!=newUser)
            {
                mCpSecure.clear();
                SerializeCpSecure(mCpSecure,false);
            }
            GENIE2_SET_SETTING(PLUGIN_SMARTNETWORK_USER,newUser);
            GENIE2_SET_SETTING(PLUGIN_SMARTNETWORK_PWD,mEdtSMPwd->text());
        }
        else
        {
            mCpSecure.clear();
            SerializeCpSecure(mCpSecure,false);
            mEdtSMUsr->clear();
            mEdtSMPwd->clear();
            sRCFlags &= ~RCF_SMUSER_IS_SAVEDUSER;
        }
        sRCFlags |= RCF_REMOTE_LOGEDIN;
    }
    else
    {
        if(chk_remmember->isChecked())
        {
            GENIE2_SET_SETTING(PLUGIN_ROUTECONF_PWD,mEdtPwd->text());
        }
        else
        {
            mEdtPwd->clear();
        }
#ifdef USE_READYSHARE
        Q_ASSERT(mEdtUsr && mEdtPwd);
        QString usr=mEdtUsr->text();
        QString pwd=mEdtPwd->text();
        mPcpForm->whenLogin(usr,pwd);

#endif
        sRCFlags |=RCF_LOCAL_LOGEDIN;
    }



    bool bRSLogic=((!remote) && (sRCFlags &RCF_REMOTE));
    if(!bRSLogic)
    {
        sRCFlags |=REFRESH_FLAGS_CLEARER;

        QGenieGaccObject *gaccObj=QGenieGaccObject::GetSingletonPtr();
        Q_ASSERT(gaccObj);
        Q_ASSERT(mGaccW);
        gaccObj->whenLogin();

//#ifdef USE_TRAFFICMETER
//        QGenieTrafficMeterObject *tmobj=QGenieTrafficMeterObject::GetSingletonPtr();
//        tmobj->whenLogin();
//#endif

        QGenieFupdateObject *fupo=QGenieFupdateObject::GetSingletonPtr();
        Q_ASSERT(fupo);
        fupo->whenLogin();

        Q_ASSERT(mStackWC);
        const char *cPara[]={"lab_ssid","lab_channel","lab_key"};
        QWidget *stack_wc=mStackWC->widget(0);
        QString na="N/A";
        for(int i=0;i<sizeof(cPara)/sizeof(cPara[0]);i++)
        {
            QLabel *lab=stack_wc->findChild<QLabel*>(cPara[i]);
            Q_ASSERT(lab);
            lab->setText(na);

        }


      //  refreshWireless(remote);

    }
    mQRCM->hideWidget(QGenieQRCodeManager::QWT_WIRELESS);
    mQRCM->hideWidget(QGenieQRCodeManager::QWT_GACC);

    if(!bReadyShareLogedin)
    {
        mTab->setCurrentIndex(0);
        mSavedTabW=mPage1;
    }

    mStackWC->setCurrentIndex(0);
    setError(-1);

    if(mSavedData == NULL)
    {
        mSavedData=QSharedPointer<tRCData>(new tRCData);
    }
    if(mSavedData_5G == NULL)
    {
        mSavedData_5G=QSharedPointer<tRCData>(new tRCData);
    }

    mStackWC->repaint();
  }

void QGenieRCWirelessObject::tryUpdateData()
{
    if(!mTab)
    {
        return;
    }
    if(sRCFlags & RCF_READYSHARE_MODE)
    {
        return;
    }
    if(IS_REMOTE_UI() && !(sRCFlags & RCF_REMOTE_LOGEDIN))
    {
        return;
    }
    if((!IS_REMOTE_UI()) && !(sRCFlags & RCF_LOCAL_LOGEDIN))
    {
        return;
    }
    if(IS_REMOTE_UI() &&(sRCFlags & RCF_ALL_CP_OFFLINE))
    {
        return;
    }
    QWidget *curTabW=mTab->currentWidget();

    if(mGaccW && (curTabW==mGaccW))
    {
        bool isLocal5G=!(sRCFlags & RCF_REMOTE)&&(sRCFlags & RCF_LOCAL_GACC_5GUNLOCKED)
                &&(sRCFlags & RCF_5G_TABCHECKED);
        bool isRemote5G=(sRCFlags & RCF_REMOTE)&&(sRCFlags & RCF_REMOTE_GACC_5GUNLOCKED)
                &&(sRCFlags & RCF_5G_TABCHECKED);
        QGenieGaccObject *gaccObj=QGenieGaccObject::GetSingletonPtr();
        Q_ASSERT(gaccObj);
        if((isLocal5G || isRemote5G)&&(sRCFlags & RCF_MUST_REFRESH_GACC_5G))
        {
            gaccObj->refreshGaccInfo(sRCFlags & RCF_REMOTE,true);
        }
        else if(sRCFlags & RCF_MUST_REFRESH_GACC)
        {
            gaccObj->refreshGaccInfo(sRCFlags & RCF_REMOTE,false);
        }
    }
    else if(mPage1 &&(mPage1==curTabW))
    {
        bool isLocal5G=!(sRCFlags & RCF_REMOTE)&&(sRCFlags & RCF_LOCAL_WIRELESS_5GUNLOCKED)
                &&(sRCFlags & RCF_5G_TABCHECKED);
        bool isRemote5G=(sRCFlags & RCF_REMOTE)&&(sRCFlags & RCF_REMOTE_WIRELESS_5GUNLOCKED)
                &&(sRCFlags & RCF_5G_TABCHECKED);

        if((isLocal5G || isRemote5G)&&(sRCFlags & RCF_MUST_REFRESH_WIRELESS_5G))
        {
            refreshWireless(sRCFlags & RCF_REMOTE,true);
        }
        else if(sRCFlags & RCF_MUST_REFRESH_WIRELESS)
        {
            refreshWireless(sRCFlags & RCF_REMOTE,false);
        }

    }

#ifdef USE_TRAFFICMETER
    else if(mTMW&&(mTMW==curTabW))
    {
        QGenieTrafficMeterObject *tmobj=QGenieTrafficMeterObject::GetSingletonPtr();
        Q_ASSERT(tmobj);
        if(sRCFlags & RCF_MUST_REFRESH_TM)
        {
    //        mbRefreshAfterLogin=false;
            tmobj->slot_RefreshData();
        }
    }
#endif
    else if(mTpW && (mTpW==curTabW))
    {
        QGenieRCTopologyObject *tpobj=QGenieRCTopologyObject::GetSingletonPtr();
        Q_ASSERT(tpobj);
        if(sRCFlags & RCF_MUST_REFRESH_TOPO)
        {
            tpobj->slot_RefreshData(sRCFlags & RCF_REMOTE);
        }
    }
#ifdef USE_QOSFUNC
	else if(mQosW &&(mQosW == curTabW))
	{
	
	}
#endif
    _updateLockState();
}

void QGenieRCWirelessObject::tryUpdateFocus()
{
    if(IS_REMOTE_UI() && !(sRCFlags & (RCF_IN_REMOTE_LOGIN_PROCESS|RCF_REMOTE_LOGEDIN)))
    {
        if(mEdtSMUsr->text().isEmpty())
        {
            mEdtSMUsr->setFocus();
        }
        else
        {
            mEdtSMPwd->setFocus();
        }
    }
    else if(!IS_REMOTE_UI() && !(sRCFlags &(RCF_IN_LOCAL_LOGIN_PROCESS|RCF_LOCAL_LOGEDIN)))
    {
        mEdtPwd->setFocus();
    }
}

void QGenieRCWirelessObject::tryUpdateFirmware()
{
    if(getCpOnline() && getCpFirmware().isEmpty())
    {
        mSessionFirmware=QGenieIface2ShareObject::sSingleInstance->mNetgearInfoM
                             ->netgearCommand(INetgearInterface::IC_SM_GET_FIRMWARE
                                              ,QStringList()<<getCpName(),this,SLOT(slot_FirmwareDataRetruned(int,QStringList)));
    }

}

void QGenieRCWirelessObject::setLoginError(int lan_idx,bool remote)
{
    if(remote)
    {
        mLanLoginErrorRemote=lan_idx;
    }
    else
    {
        mLanLoginErrorLocal=lan_idx;
    }
//    QString s=(mLanLoginErrorLocal<0)?"":QString(ERROR_MODE).arg(GENIE2_GET_TEXT(mLanLoginErrorLocal));
//    lab_login_error->setText(s);
}

void QGenieRCWirelessObject::setError(int lan_idx)
{
    mLanError=lan_idx;
    QString s=(mLanError<0)?"":QString(ERROR_MODE).arg(GENIE2_GET_TEXT(mLanError));
    mErrorLabel->setText(s);
}

void QGenieRCWirelessObject::slot_DataReturned(int session, const QStringList &lst)
{

    qDebug()<<"QGenieRCWirelessObject::slot_DataRreturned"<<session<<lst;
//    QStackedWidget *stack=qFindChild<QStackedWidget *>(mWirelessW,"stack_total");
    bool btemp;
    int length=lst.count();
    bool requestOk=(length>0 && (0==lst[0].toInt(&btemp)) && btemp);
	quint32 _5GState=(length >7)?lst[7].toUInt():0;
	
    if(session==mSessionLogin)
    {
//        Q_ASSERT(!IS_REMOTE());
        Q_ASSERT(login_frame);
        sRCFlags &= ~(RCF_IN_LOCAL_LOGIN_PROCESS);
        Q_ASSERT(mIndicatorLogin);

        if(requestOk)
        {
            sRCFlags |= RCF_LOCAL_LOGEDIN;

			m_QosState = (length>8)?lst[8].toUInt():0;

            QString strFirmware = QString("%1 %2").arg(GENIE2_GET_TEXT(L_RTCF_FIRMWARE_VERSION)).arg(lst[2]);
            mModel = lst[1];
            mFirmware = lst[2];

            Q_ASSERT( lab_local_model && lab_firmware);
            lab_local_model->setText(mModel);
            lab_firmware->setText((length>2)?strFirmware:"");

            loginOk(false);
            if(length > 6 && (lst[1].compare("WNR3500Lv2",Qt::CaseInsensitive) == 0) && (lst[6].toInt()>0))
            {
                sRCFlags |= RCF_SMARTNETWORK_UNLOCKED;
                GENIE2_SET_SETTING(PLUGIN_SMARTNETWORK_UNLOCKED,true);
                QLabel *lab_lgfrom=login_frame->findChild<QLabel *>("lab_lgfrom");
                Q_ASSERT(lab_lgfrom);
                cob_loginmode->setVisible(true);
                lab_lgfrom->setVisible(true);
                GENIE2_UI_COMMAND(UICMD_UNLOCK_SMARTNETWORK,0); 
                GENIE2_LOG("Smart Network Unlocked!!");
			}
#ifdef USE_READYSHARE
            Q_ASSERT(mPcpForm);
            int flag=(lst.count()>5)?(lst[5].trimmed().toUInt()):0;
            mPcpForm->mFlags=flag;
            QGenieRSManager *rsmanager=QGenieRSManager::GetSingletonPtr();
            if(rsmanager)
            {
                rsmanager->updateTitleButton(flag);
            }
#endif
            const quint32 cUnlock5GWireless=(1<<0);
            const quint32 cUnlock5GGacc=(1<<1);
            if(_5GState & cUnlock5GWireless)
            {
                sRCFlags |= RCF_LOCAL_WIRELESS_5GUNLOCKED;
            }
            if(_5GState & cUnlock5GGacc)
            {
                sRCFlags |= RCF_LOCAL_GACC_5GUNLOCKED;
            }

        }
        else
        {
            if(length>0 &&(401==lst[0].toInt(&btemp))&& btemp)
            {
                setLoginError(L_RTCF_MSG_PWDWRONG,false);
                mEdtPwd->clear();
            }
            else if(length>0 &&(-2==lst[0].toInt(&btemp))&& btemp)
            {
                setLoginError(L_RTCF_MSG_NOTNETGEAR,false);
                //                QCheckBox *chk_remember=qFindChild<QCheckBox *>(login_frame,"chk_remember");
                Q_ASSERT(chk_remmember);
                if(!chk_remmember->isChecked())
                {
                    mEdtPwd->clear();
                }
            }
			////kai.yan add for old netgear router
            else if(length>0 &&(-6==lst[0].toInt(&btemp))&& btemp)
			{
				setLoginError(L_RTCF_MSG_OLDROUTER,false);
				Q_ASSERT(chk_remmember);
				if(!chk_remmember->isChecked())
				{
					mEdtPwd->clear();
				}
			}////////////////////////end//////////////////////
			else
            {
                setLoginError(L_RTCF_MSG_LOGINTIMEOUT,false);
                //                QCheckBox *chk_remember=qFindChild<QCheckBox *>(login_frame,"chk_remember");
                Q_ASSERT(chk_remmember);
                if(!chk_remmember->isChecked())
                {
                    mEdtPwd->clear();
                }
            }
            sRCFlags &= ~ALL_LOCAL_CMD;
            GENIE2_SET_FOCUS(mEdtPwd);

        }
        mIndicatorLogin->stopAnimation();
//        QGenieDelayProcessLogic::GetSingleton().whenLoginEnd(requestOk,false);
    }
    else if(session==mSessionWireless ||session==mSessionWireless5G)
    {
        Q_ASSERT(!(sRCFlags & RCF_REMOTE_LOGEDIN));
        sRCFlags &= ~RCF_IN_LOCAL_REFRESHING;
        mIndicatorWireless->stopAnimation();
        mIndicatorWirelessSetting->stopAnimation();
        QPushButton *but_modify=mStackWC->widget(0)->findChild<QPushButton *>("but_modify");
        QPushButton *but_export=mStackWC->widget(0)->findChild<QPushButton *>("but_export");
        Q_ASSERT(but_modify && but_export);
        but_modify->setEnabled(requestOk);
        but_export->setEnabled(requestOk);
		
        if(requestOk)
        {
// "000" "wireless001" "United States" "Auto" "300Mbps"
//                    "204E7F359BF3" "WPA2-PSK" "netgear";

//            QList<QStackedWidget *> stacklst=qFindChildren<QStackedWidget *>(mWirelessW,"stack_wc");
//            Q_ASSERT(stacklst.count()==1);
//            QStackedWidget *mStackWC=stacklst[0];
            QSharedPointer<tRCData> data=(session==mSessionWireless)?mSavedData:mSavedData_5G;

            Q_ASSERT(mStackWC);
#ifdef USE_SUBPLUGINS
            data->mEncryption="none";
            data->mAuthentation="open";
#endif

            data->mSavedMod=(length>4)?lst[4]:"";
            data->mSavedRegion=(length>2)?lst[2]:"";
            data->mSavedSsid=(length>1)?lst[1]:"";
            data->mSavedPwd=(length>8)?lst[8]:"";
            data->mSavedCannel=(length>3)?lst[3]:"";
            qDebug() << "channel:" << data->mSavedCannel;
//            int cobIdx=data->mSavedCannel.toInt(&btemp);
//            if(!btemp)
//            {
//                cobIdx=0;
//            }
            //add by caiyujun
            //if channel < 10, data would be 0x, need to be changed x;
            if ("Auto" != data->mSavedCannel && "" != data->mSavedCannel && "AUTO" != data->mSavedCannel)
            {
                int iSaveDChanel = data->mSavedCannel.toInt();
                data->mSavedCannel = QString::number(iSaveDChanel);
            }
            //add end
            if(0==data->mSavedRegion.compare("UNITED STATES", Qt::CaseInsensitive)
                    ||0==data->mSavedRegion.compare("USA", Qt::CaseInsensitive))
            {
                data->mSavedRegion="US";
            }

            QString mac=(length>5)?lst[5]:"";
            if((data->mSavedMac!=mac) && !data->mSavedMac.isEmpty())
            {
                logOut(false);
                updateUi();
                return;
            }
            data->mSavedMac=mac;


//            QGenieGaccDisableingLogic *gacclogic=QGenieGaccDisableingLogic::GetSingletonPtr();
//            if(gacclogic)
//            {
//                gacclogic->notifyMac(data->mSavedMac);
//            }

//            mQRCM->setData(data->mSavedSsid,data->mSavedPwd,QGenieQRCodeManager::QWT_WIRELESS);

             int secure_idx=1;
            if(length>6)
            {
                const QString &secure=lst[6];
#ifdef USE_SUBPLUGINS
                if(0==secure.compare("wep",Qt::CaseInsensitive))
                {
                    data->mEncryption="WEP";
                    data->mAuthentation=(length>7)?lst[7]:"";
                }
                else
                {
                    data->mEncryption=(0==secure.compare("WPA-PSK/WPA2-PSK",Qt::CaseInsensitive))
                               ?"TKIP":"AES";
                    data->mAuthentation=secure.toLower();
                    data->mAuthentation.replace("-","");
                    if(0!=data->mAuthentation.compare("WPAPSK",Qt::CaseInsensitive))
                    {
                        data->mAuthentation="WPA2PSK";
                    }
                }
#endif
                secure_idx=getSecureIdx(secure);

                QRadioButton *cRadioButtons[]={mRdtNone,mRdtAES,mRdtMix};
                data->mSavedRdt=cRadioButtons[secure_idx];
            }




            updateShowingPage();
            updateSettingPage();
//            QGenieDelayProcessLogic::GetSingleton().whenGetWirelessInfoEnd(requestOk);


        }//requestOk
        else
        {
            if(session==mSessionWireless)
            {
                sRCFlags |= RCF_MUST_REFRESH_WIRELESS;
            }
            else
            {
                sRCFlags |= RCF_MUST_REFRESH_WIRELESS_5G;
            }
            mStackWC->setCurrentIndex(0);
            setError(L_RTCF_MSG_TIMEOUT);
			mIndicatorWireless->stopAnimation();//kai.yan add remote login timeout
			mWirelessW->setEnabled(true);
			
        }

	}
	if(requestOk || (session==mSessionLogin))
	{
		updateUi();
	}
    if(session==mSessionLogin)
    {
        tryUpdateData();
    }
}

void QGenieRCWirelessObject::slot_SMDataReturned(int session, const QStringList &lst)
{

//    QStackedWidget *stack=qFindChild<QStackedWidget *>(mWirelessW,"stack_total");
    bool btemp=false;
    int length=lst.count();
    int rvalue=(length>0)?lst[0].toInt(&btemp):-1;
    bool requestOk=((0==rvalue) && btemp);
    bool wantReLoginCP=((FE_WRONG_USER_OR_PWD==rvalue) && btemp);
    bool wantReAuthen=((FE_NOT_AUTHENTICATIED==rvalue)&&btemp);

    if(requestOk &&((session==mSessionWireless)||(session==mSessionWireless5G)) )
    {
        QTellLPCHelper::GetSingleton().tryTellLPC();
    }

    if(wantReAuthen)
    {
        setLoginError(L_RTCF_MSG_PLEASERELOGIN,true);
        logOut(true);
        updateUi();
        return;
    }
    if(wantReLoginCP )
    {
        if(tryReloginCP())
        {
            if(session==mSessionWireless)
            {
                mSessionWireless=QGenieIface2ShareObject::sSingleInstance->mNetgearInfoM
                        ->netgearCommand(INetgearInterface::IC_SM_WIRELESS
                                         ,QStringList()<<getCpName(),this
                                         ,SLOT(slot_SMDataReturned(int,QStringList)));
            }
            else if(session==mSessionWireless5G)
            {
                mSessionWireless5G=QGenieIface2ShareObject::sSingleInstance->mNetgearInfoM
                        ->netgearCommand(INetgearInterface::IC_SM_GET_5GINFO
                                         ,QStringList()<<getCpName(),this
                                         ,SLOT(slot_SMDataReturned(int,QStringList)));
            }

        }
        else
        {
            if((session==mSessionWireless)||(session==mSessionWireless5G))
            {
                if(session==mSessionWireless)
                {
                    sRCFlags|=RCF_MUST_REFRESH_WIRELESS;
                }
                else
                {
                    sRCFlags|=RCF_MUST_REFRESH_WIRELESS_5G;
                }
                sRCFlags &= ~RCF_IN_REMOTE_REFRESHING;
                QPushButton *but_modify=mStackWC->widget(0)->findChild<QPushButton *>("but_modify");
                QPushButton *but_export=mStackWC->widget(0)->findChild<QPushButton *>("but_export");
                Q_ASSERT(but_modify && but_export);
                but_modify->setEnabled(requestOk);
                but_export->setEnabled(requestOk);
                mIndicatorWireless->stopAnimation();
                mIndicatorWirelessSetting->stopAnimation();
            }
        }
        updateUi();
        return;
    }

    if(session==mSessionSMLogin)
    {
        Q_ASSERT(sRCFlags &RCF_REMOTE);
        Q_ASSERT(login_frame);
        sRCFlags &= ~RCF_IN_REMOTE_LOGIN_PROCESS;
        Q_ASSERT(mIndicatorLogin);

        if(requestOk)
        {

			sRCFlags |=RCF_INCHANGE_CPIDX;
            cob_remote_model->clear();
            //                mCpList.clear();
            int r=(length > 1)?lst[1].toInt():0;
            int c=(length > 2)?lst[2].toInt():0;
            Q_ASSERT(c==5);

            QString cpname=GENIE2_GET_SETTING(PLUGIN_SMARTNETWORK_CURCP).toString();
            int itemp=-1;
            bool bAllOffline=true;
            for(int i=0;i<r;i++)
            {
                int cpIdx=3+i*c+0;
                int serialIdx=3+i*c+2;
                int modelIdx=3+i*c+1;
                int onlineIdx=3+i*c+4;
                QString sCp=(length>cpIdx)?lst[cpIdx]:"";
                QString sSerial=(length>serialIdx)?lst[serialIdx]:"";
                QString sModel=(length>modelIdx)?lst[modelIdx]:"";
                QString sOnline=(length > onlineIdx)?lst[onlineIdx]:"";
                bool bOnline=(0==sOnline.compare("true",Qt::CaseInsensitive));
                bAllOffline=(bAllOffline &&!bOnline);
                if((sCp==cpname) && bOnline)
                {
                    itemp=i;
                }
                //mCpList<<sCp;
                cob_remote_model->addItem(QString(SM_COB_MOD)
                                          .arg(sModel,sSerial,bOnline?
                                                   GENIE2_GET_TEXT(L_RTCT_ONLINE):GENIE2_GET_TEXT(L_RTCT_OFFLINE))
                                          ,sCp);
                cob_remote_model->setItemData(cob_remote_model->count()-1,sModel,cRouterModelRole);
                cob_remote_model->setItemData(cob_remote_model->count()-1,bOnline,cOnlineRole);
                cob_remote_model->setItemData(cob_remote_model->count()-1,sSerial,cSerialRole);

            }

            cob_remote_model->setCurrentIndex((itemp >=0)?itemp:0);
            mSavedCpIdx=cob_remote_model->currentIndex();
            lab_remote_firmware->clear();

            if(bAllOffline)
            {
                sRCFlags |= RCF_ALL_CP_OFFLINE;
            }
            else
            {
                sRCFlags &= ~RCF_ALL_CP_OFFLINE;
            }
            sRCFlags &=~RCF_INCHANGE_CPIDX;

            loginOk(true);
        }
        else
        {
            if(rvalue==FE_LOGIN_WRONG_PWD)
            {
                setLoginError(L_RTCF_MSG_PWDWRONG,true);
            }
            else if(rvalue==FE_GETCPLIST)
            {
                setLoginError(L_RTCF_MSG_NOCP,true);
            }
            else
            {
                setLoginError(L_RTCF_MSG_SMLOGIN_ERROR,true);
            }
            sRCFlags &= ~ALL_REMOTE_CMD;
        }
        mIndicatorLogin->stopAnimation();
//        QGenieDelayProcessLogic::GetSingleton().whenLoginEnd(requestOk,true);
    }
    else if((session==mSessionWireless)||(session==mSessionWireless5G))
    {
//        Q_ASSERT(sRCFlags & RCF_REMOTE_LOGEDIN);
//        mWirelessW->setEnabled(true);
        sRCFlags &= ~RCF_IN_REMOTE_REFRESHING;

        mIndicatorWireless->stopAnimation();
        mIndicatorWirelessSetting->stopAnimation();
        QPushButton *but_modify=mStackWC->widget(0)->findChild<QPushButton *>("but_modify");
        QPushButton *but_export=mStackWC->widget(0)->findChild<QPushButton *>("but_export");
        Q_ASSERT(but_modify && but_export);
        but_modify->setEnabled(requestOk);
        but_export->setEnabled(requestOk);

        if(requestOk)
        {
//            QList<QStackedWidget *> stacklst=qFindChildren<QStackedWidget *>(mWirelessW,"stack_wc");
//            Q_ASSERT(stacklst.count()==1);
//            QStackedWidget *mStackWC=stacklst[0];
            QSharedPointer<tRCData> data=(session==mSessionWireless)?mSavedData:mSavedData_5G;
            Q_ASSERT(mStackWC);
#ifdef USE_SUBPLUGINS
            data->mEncryption="none";
            data->mAuthentation="open";
#endif

            data->mSavedMod=(length>4)?lst[4]:"";
            data->mSavedRegion=(length>2)?lst[2]:"";
            data->mSavedSsid=(length>1)?lst[1]:"";
            data->mSavedPwd=(length>8)?lst[8]:"";
            data->mSavedCannel=(length>3)?lst[3]:"";
            qDebug() << "channel:" << data->mSavedCannel;
//            int cobIdx=data->mSavedCannel.toInt(&btemp);
//            if(!btemp)
//            {
//                cobIdx=0;
//            }
            //add by caiyujun
            //if channel < 10, data would be 0x, need to be changed x;
            if ("Auto" != data->mSavedCannel && "" != data->mSavedCannel && "AUTO" != data->mSavedCannel)
            {
                int iSaveDChanel = data->mSavedCannel.toInt();
                data->mSavedCannel = QString::number(iSaveDChanel);
            }
            //add end
            data->mSavedMac="";

            if(0==data->mSavedRegion.compare("UNITED STATES", Qt::CaseInsensitive)
                    ||0==data->mSavedRegion.compare("USA", Qt::CaseInsensitive))
            {
                data->mSavedRegion="US";
            }



//            mQRCM->setData(data->mSavedSsid,data->mSavedPwd,QGenieQRCodeManager::QWT_WIRELESS);



            int secure_idx=1;
            if(length>6)
            {
                const QString &secure=lst[6];
#ifdef USE_SUBPLUGINS
                if(0==secure.compare("wep",Qt::CaseInsensitive))
                {
                    data->mEncryption="WEP";
                    data->mAuthentation=(length>7)?lst[7]:"";
                }
                else
                {
                    data->mEncryption=(0==secure.compare("WPA-PSK/WPA2-PSK",Qt::CaseInsensitive))
                               ?"TKIP":"AES";
                    data->mAuthentation=secure.toLower();
                    data->mAuthentation.replace("-","");
                    if(0!=data->mAuthentation.compare("WPAPSK",Qt::CaseInsensitive))
                    {
                        data->mAuthentation="WPA2PSK";
                    }
                }
#endif
                secure_idx=getSecureIdx(secure);

                QRadioButton *cRadioButtons[]={mRdtNone,mRdtAES,mRdtMix};
                data->mSavedRdt=cRadioButtons[secure_idx];
            }




            updateShowingPage();
            updateSettingPage();
//            QGenieDelayProcessLogic::GetSingleton().whenGetWirelessInfoEnd(requestOk);

            tryUpdateFirmware();
        }//requestOk
        else
        {
            if(session==mSessionWireless)
            {
                sRCFlags |= RCF_MUST_REFRESH_WIRELESS;
            }
            else
            {
                sRCFlags |= RCF_MUST_REFRESH_WIRELESS_5G;
            }
            mStackWC->setCurrentIndex(0);
            setError(L_RTCF_MSG_SMLOGIN_ERROR);
        }
		mIndicatorWireless->stopAnimation();//kai.yan add remote login timeout
		mWirelessW->setEnabled(true);
    }
    if(requestOk || session==mSessionSMLogin)
	{
		updateUi();//kai.yan modify
	}
    if(session==mSessionSMLogin)
    {
        tryUpdateData();
    }
}

void QGenieRCWirelessObject::slot_FirmwareDataRetruned(int session, const QStringList &lst)
{
    if(session==mSessionFirmware)
    {
        bool btemp=false;
        int length=lst.count();
        int rvalue=(length>0)?lst[0].toInt(&btemp):-1;
        bool requestOk=((0==rvalue) && btemp);
        if(requestOk)
        {
            QString firmware=(length>1)?lst[1]:"";
            cob_remote_model->setItemData(cob_remote_model->currentIndex()
                                          ,firmware,cFirmwareRole);

            QString remote_firmware=firmware.isEmpty()?"":QString("%1 %2").arg(GENIE2_GET_TEXT(L_RTCF_FIRMWARE_VERSION),firmware);
            lab_remote_firmware->setText(remote_firmware);
        }
    }
}

void QGenieRCWirelessObject::slot_LoginModeChaged(int idx)
{
    if(sRCFlags & (RCF_INCHANGE_UI|RCF_INCHANGELANGUAGE))
    {
        return;
    }
    changeRemoteState(idx==1);
    updateUi();
}

void QGenieRCWirelessObject::slot_TabIdxChanged(int idx)
{
    Q_UNUSED(idx)
    if(sRCFlags &RCF_INCHANGE_UI)
    {
        return;
    }
    const struct _{const QWidget *w;const char * n;} cParas[]={
        {mGaccW,"R6"}
        ,{mPage1,"R5"}
        ,{mSubpluginUpdateW,"R13"}
        ,{mPcpForm,"R20"}
        ,{mTMW ,"R10"}
    };
    QWidget *curW=mTab->currentWidget();
    for(int i=0;i<sizeof(cParas)/sizeof(cParas[0]);i++)
    {
        if(cParas[i].w &&(curW==cParas[i].w))
        {
            GENIE2_STATISTICS(QString(cParas[i].n));
        }
    }
    if(!(sRCFlags & RCF_READYSHARE_MODE))
    {
        mSavedTabW=curW;
    }

//    if(curW &&(curW==mPage1 || curW==mGaccW))
//    {
//        sRCFlags |=RCF_5G_PAGEHAVE_5GFUNC;
//    }
//    else
//    {
//        sRCFlags &= ~RCF_5G_PAGEHAVE_5GFUNC;
//    }
    if(mStackWC)
    {
        mStackWC->setCurrentIndex(0);
    }
    QGenieGaccObject *gacco=QGenieGaccObject::GetSingletonPtr();
    if(gacco && gacco->stack_gacc)
    {
        gacco->stack_gacc->setCurrentIndex(0);
    }

    tryUpdateData();
#ifdef USE_5GFUNC	
    update5GUi();
#endif
}

void QGenieRCWirelessObject::slot_RemmemberClicked(bool b)
{
    QCheckBox *chkbox=qobject_cast<QCheckBox *>(sender());
    Q_ASSERT(chkbox);
    if(chkbox==chk_remmember)
    {

        GENIE2_SET_SETTING(PLUGIN_ROUTECONF_SAVEPWD,b);
    }
    else if(chkbox==chk_smremember)
    {

        GENIE2_SET_SETTING(PLUGIN_SMARTNETWORK_SAVEPWD,b);
    }
    updateUi();
}

void QGenieRCWirelessObject::slot_InputIPCheckBoxChanged(bool b)
{
	GENIE2_SET_SETTING(PLUGIN_ROUTECONF_SAVEIP,b);
	if(b)
	{
		QHostInfo::lookupHost("routerlogin.net",this, SLOT(slot_Lookup(QHostInfo))); 
	}
	frame_ip->setVisible(b);

}
void QGenieRCWirelessObject::slot_Lookup(const QHostInfo hostInfo)
{
	if (hostInfo.error() != QHostInfo::NoError) {  
		qDebug() << "Lookup failed:" << hostInfo.errorString();  
		return;  
	}  
	foreach (QHostAddress address, hostInfo.addresses())  
	{
		if(address.toIPv4Address())
		{
			m_ip_Manual = address.toString();
			break;
		}else
		{
			m_ip_Manual = "";
			continue;
		}
	}
	if(m_ip_Manual!="")
	{
		mEdtIP->setText(m_ip_Manual);
	}
}

void QGenieRCWirelessObject::updateShowingPage()
{

    QSharedPointer<tRCData> data=IS_WIRELESS_5G()?mSavedData_5G:mSavedData;
    if(data.isNull())
    {
        return;
    }

    const struct _{const char *name;QString *s;}
    cPara[]={
        {"lab_ssid", &(data->mSavedSsid)}
        ,{"lab_channel",&(data->mSavedCannel)}
        ,{"lab_key",&(data->mSavedPwd)}
    };
    for(int i=0;i<sizeof(cPara)/sizeof(cPara[0]);i++)
    {
        QLabel *label=mStackWC->findChild<QLabel *>(cPara[i].name);
        Q_ASSERT(label);
        label->setText(autoWrap(*(cPara[i].s)));
    }
#ifdef USE_SUBPLUGINS
    QVariantList vlst;
    vlst<<0<<data->mSavedSsid<<data->mSavedPwd<<data->mAuthentation<<data->mEncryption;
    GENIE2_TELL_OTHERPLUGIN(mSubpluginExportUuid,EPCC_WIRELESSEXPORT_DATA,vlst);
#endif
    bool dataok=false;
    if(IS_WIRELESS_5G())
    {
        dataok=!(sRCFlags&RCF_MUST_REFRESH_WIRELESS_5G);
        mModify->setEnabled(dataok);

    }
    else
    {
        dataok=!(sRCFlags&RCF_MUST_REFRESH_WIRELESS);
        mModify->setEnabled(dataok);
    }
    if(dataok)
    {
        mQRCM->setData(data->mSavedSsid,data->mSavedPwd,QGenieQRCodeManager::QWT_WIRELESS);
    }
    else
    {
        mQRCM->hideWidget(QGenieQRCodeManager::QWT_WIRELESS);
    }


}

void QGenieRCWirelessObject::updateSettingPage()
{

    QSharedPointer<tRCData> data=IS_WIRELESS_5G()?mSavedData_5G:mSavedData;
    if(data.isNull())
    {
        return;
    }
    mCobChannel->clear();
    mCobChannel->addItem("Auto");
    if(IS_WIRELESS_5G())
    {
        for(int i=0;i<sizeof(c5GChannels)/sizeof(c5GChannels[0]);i++)
        {
            mCobChannel->addItem(QString::number(c5GChannels[i]));
        }
    }
    else
    {
        int max=(0==data->mSavedRegion.compare("US",Qt::CaseInsensitive))?11:13;
        for(int i=1;i<=max;i++)
        {
            mCobChannel->addItem(QString::number(i));
        }

    }

    //modify by caiyujun
    mCobChannel->setCurrentIndex(mCobChannel->findText(data->mSavedCannel, Qt::MatchFixedString));
    //modify end
    if(data->mSavedRdt)
    {
        data->mSavedRdt->setChecked(true);
    }
    fr_pwd->setVisible(!mRdtNone->isChecked());

    mEdtKey->setText(data->mSavedPwd);
    mEdtSsid->setText(data->mSavedSsid);
    mEdtKey->setCursorPosition(0);
    mEdtSsid->setCursorPosition(0);
    but_apply->setEnabled(false);
}

bool QGenieRCWirelessObject::tryReloginCP()
{
    QCheckBox *chk=NULL;

    if(!mReloginDlg)
    {
        QUiLoader loader;
        bool btemp;
        QFile f(GENIE2_RES("ui/smlogin.ui"));
        btemp=f.open(QFile::ReadOnly);
        Q_ASSERT(btemp);
        Q_ASSERT(mWirelessW);
        QWidget *w=loader.load(&f, QGenieIface2ShareObject::sSingleInstance->mMainFrame);
        Q_ASSERT(w);
        mReloginDlg=qobject_cast<QDialog *>(w);
        Q_ASSERT(mReloginDlg);
        GENIE2_SET_FRAMELESSSTYLE(mReloginDlg);

        chk=mReloginDlg->findChild<QCheckBox *>("chk_remember");

        connect(chk,SIGNAL(clicked(bool)),this,SLOT(slot_RememberCpCheckBoxChanged(bool)));
        translateSMLoginDialog();

    }
    bool bRememberCPSecure=GENIE2_GET_SETTING(PLUGIN_SMARTNETWORK_SAVECPSECUR).toBool();
    bool bRememberLoginPwd=GENIE2_GET_SETTING(PLUGIN_SMARTNETWORK_SAVEPWD).toBool();
    chk=chk?chk:mReloginDlg->findChild<QCheckBox *>("chk_remember");

    Q_ASSERT(chk);
    //chk->setChecked(bRememberCPSecure&&bRememberLoginPwd);
    //chk->setEnabled(bRememberLoginPwd);
    //chk->setVisible(bRememberLoginPwd);
	
	///kai.yan modify for remote login--------
	chk->setChecked(bRememberCPSecure);
	chk->setEnabled(true);
	chk->setVisible(true);
	////kai.yan end -------------

    QString cpname=getCpName();
    QLineEdit *edt_pwd=mReloginDlg->findChild<QLineEdit *>("edt_pwd");

    QLabel *lab_smmodel=mReloginDlg->findChild<QLabel *>("lab_smmodel");
    Q_ASSERT(lab_smmodel);
    lab_smmodel->setText(cob_remote_model->currentText());
//    if(mCpSecure.contains(cpname))
//    {
//        edt_pwd->setText(mCpSecure[cpname].second);
//    }
//    else
 //   {
        edt_pwd->setText("");
        edt_pwd->setFocus();
   // }
    sRCFlags |=RCF_RELOGIN_DIALOG_ON;
    int execRet=mReloginDlg->exec();
    sRCFlags &=~RCF_RELOGIN_DIALOG_ON;

    if(1==execRet)
    {
        QLineEdit *edt_usr=mReloginDlg->findChild<QLineEdit *>("edt_usr");
        QLineEdit *edt_pwd=mReloginDlg->findChild<QLineEdit *>("edt_pwd");

        QGenieIface2ShareObject::sSingleInstance->mNetgearInfoM->setConfiguration
                (INetgearInterface::IS_SM_CPLOGIN
                 ,QStringList()<<cpname<<edt_usr->text()<<edt_pwd->text());
        mCpSecure.insert(cpname
                         ,QPair<QString,QString>(edt_usr->text(),edt_pwd->text()));

        if(chk->isChecked())
        {
             SerializeCpSecure(mCpSecure,false);
        }
        else
        {
            edt_pwd->clear();
        }

        return true;
    }
    return false;
}

void QGenieRCWirelessObject::connectStateCheckSignalSlots(QWidget *stack_wc)
{
    Q_ASSERT(stack_wc);
    //0.label   1.button    2.checkbox  3.radiobutton 4.editor 5.cobbox
    struct _{const char *name;int type;} cParas[]=
    {
        {"sedt_ssid",4}
        ,{"sedt_key",4}
        ,{"cob_channel",5}
        ,{"rdt_none",3}
        ,{"rdt_aes",3}
        ,{"rdt_mix",3}
    };
    for(int i=0;i<sizeof(cParas)/sizeof(cParas[0]);i++)
    {
        if(cParas[i].type==3)
        {
            QRadioButton *rdt=stack_wc->findChild<QRadioButton *>(cParas[i].name);
            Q_ASSERT(rdt);
            connect(rdt,SIGNAL(clicked()),this,SLOT(slot_CheckUiEnableState()));
        }
        else if(cParas[i].type==4)
        {
            QLineEdit *edt=stack_wc->findChild<QLineEdit *>(cParas[i].name);
            Q_ASSERT(edt);
            connect(edt,SIGNAL(textChanged(QString)),this,SLOT(slot_CheckUiEnableState()));
        }
        else if(cParas[i].type==5)
        {
            QComboBox *box=stack_wc->findChild<QComboBox *>(cParas[i].name);
            Q_ASSERT(box);
            connect(box,SIGNAL(currentIndexChanged(int)),this,SLOT(slot_CheckUiEnableState()));

        }
    }
}

void QGenieRCWirelessObject::slot_CheckUiEnableState()
{
    QSharedPointer<tRCData> data=IS_WIRELESS_5G()?mSavedData:mSavedData_5G;
    if(!data.isNull())
    {
        fr_pwd->setVisible(!mRdtNone->isChecked());
        bool b_changed=(data->mSavedRdt && !data->mSavedRdt->isChecked())
                ||(mEdtSsid->text()!=data->mSavedSsid)
                ||(mEdtKey->text()!=data->mSavedPwd)
                ||(mCobChannel->currentIndex()!=data->mSavedCannel.toInt());
        bool text_valid=(mEdtKey->text().length()>=8 || mRdtNone->isChecked())&&!mEdtSsid->text().isEmpty();
        but_apply->setEnabled(text_valid && b_changed);
    }
//    bool b_changed=(mRdtCurrent && !mRdtCurrent->isChecked())
//                   ||(edt_ssid->text()!=lab_ssid_r->text())
//                   ||(edt_pwd->text()!=lab_pwd_r->text());
//    b_changed=b_changed ||(mRdtCurrent && !mRdtCurrent->isChecked());
//    bool text_valid=((edt_pwd->text().length()>=8 ||rdt_none->isChecked())&& !edt_ssid->text().isEmpty());

//    bool b_applybut_enable=(
//                            (mEnableState && rdt_disable->isChecked())
//                            ||(!mEnableState && rdt_enable->isChecked() && text_valid)
//                            ||(mEnableState && rdt_enable->isChecked() && text_valid && b_changed)
//                            );
//    but_apply->setEnabled(b_applybut_enable);

    //20121120 yankai add
    QString pwd =mEdtKey->text().trimmed();
    if(pwd!="" && pwd != NULL)
    {
        int currentP = mEdtKey->cursorPosition();
            //qDebug()<<pwd<<"cursorPosition:"<<currentP<<"pwd.at(currentP-1):"<<pwd.at(currentP-1);
        if(currentP-1>-1&&(pwd.at(currentP-1)>127||pwd.at(currentP-1)<0))
        {
            pwd.remove(currentP-1,1);
			currentP = currentP-1;
        }
		pwd.toLatin1();
        mEdtKey->setText(pwd);
		mEdtKey->setCursorPosition(currentP);
            //qDebug()<<"--------"<<pwd<<":"<<currentP-1;
    }
}

//void QGenieRCWirelessObject::slot_ChkRememberClicked(bool b)
//{

//}
