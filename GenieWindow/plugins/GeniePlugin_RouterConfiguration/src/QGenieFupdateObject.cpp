#include "QGenieFupdateObject.h"
#include <QtGui>
#include <QtCore>
#include <QUiLoader>
#include "genie2_interface.h"
#include "genie2_netgear.h"
#include "QPiObject.h"
#include "QSetTextHelper.h"
#include "language_RouterConfig.h"
#include "QGenieRCWirelessObject.h"
#include "genie2_routeranim.h"

#include "QGenieTimelineLocker.h"//kai.yan add
#include "QGenieTimeGenerator.h"

#define TIMER_DELAY (1*60*1000)

extern quint64 sRCFlags;

static int compareVersion(const QString &sv1,const QString &sv2)
{
    QRegExp vNumMatch("^\\D+(\\d+(\\.\\d+)*).*");
	//QRegExp vNumMatch("^(\\d+(\\.\\d+)*).*");

    quint32 uiv1,uiv2;
	QString ssv1 = (vNumMatch.indexIn(sv1)>=0)?vNumMatch.cap(1):"";
    QString ssv2 = (vNumMatch.indexIn(sv2)>=0)?vNumMatch.cap(1):"";
    QStringList slv1=ssv1.split(".");
    QStringList slv2=ssv2.split(".");
    int c1=slv1.count();
    int c2=slv2.count();
    int n=qMax(c1,c2);
    for(int i=0;i<n;i++)
    {
        if(c1<=i){return -1;}
        if(c2<=i){return 1;}
        uiv1=slv1[i].toUInt();
        uiv2=slv2[i].toUInt();
        if(uiv1 !=uiv2)
        {
            return uiv1-uiv2;
        }
    }
    return 0;
}

QGenieFupdateObject::QGenieFupdateObject(QObject *parent)
    :    QObject(parent)
    ,mW(NULL)
    ,mStack(NULL)
    ,mPi(NULL)
    ,mPi2(NULL)//hu
    ,mPi3(NULL)//hu

    ,mSession(-1)
    ,but_l(NULL)
    ,but_r(NULL)
    ,but_finish(NULL)
    ,but_restart(NULL)//hu
    ,mLanFinish3(-1)
    ,mLanFinish1(-1)
    ,mLanFinish2(-1)
//    ,mBehindSession(-1)
{
    mRetryTimes=GENIE2_GET_SETTING(SUPDATE_RETRYTIME).toInt();
    connect(&mTimer,SIGNAL(timeout()),this,SLOT(slot_Timeout()));
}

QGenieFupdateObject::~QGenieFupdateObject()
{
    disconnect(&mTimer,SIGNAL(timeout()),NULL,NULL);
    mTimer.stop();
}

QWidget *QGenieFupdateObject::getUi()
{
    if(!mW)
    {
        bool btemp;
        QUiLoader loader;
        QFile f(GENIE2_RES("ui/firmwareupdate.ui"));
        btemp=f.open(QFile::ReadOnly);
        Q_ASSERT(btemp);
        QWidget *w=loader.load(&f,QGenieIface2ShareObject::sSingleInstance->mMainFrame);
        w->hide();
        Q_ASSERT(w);
        mW=w;
        mStack=w->findChild<QStackedWidget *>("stack");
        but_l=w->findChild<QPushButton *>("but_l");
        but_r=w->findChild<QPushButton *>("but_r");
        but_finish=w->findChild<QPushButton *>("but_finish");
        but_restart=w->findChild<QPushButton *>("but_restart");//hu
        mPi=new QPiObject(mStack,"f_pi");
        mPi2=new QPiObject(mStack,"f_pi_2");//hu
        mPi3=new QPiObject(mStack,"f_pi_3");//hu

        QFrame *anchor=w->findChild<QFrame*>("f_anim");
        Q_ASSERT(anchor);
        QGenieRouteWidget_B3 *anim=new QGenieRouteWidget_B3(anchor);
        QHBoxLayout *layout=new QHBoxLayout(anchor);
        layout->setMargin(0);
        layout->addWidget(anim);


        QList<QPushButton *> bts=w->findChildren<QPushButton *>();
        int c=bts.count();
        for(int i=0;i<c;i++)
        {
            connect(bts[i],SIGNAL(clicked()),this,SLOT(slot_ButClicked()));
        }
        translateUi();
        mTimer.start(TIMER_DELAY);
    }
    return mW;
}

void QGenieFupdateObject::slot_ButClicked()
{
    QPushButton *but=qobject_cast<QPushButton *>(sender());
    Q_ASSERT(but);
    bool bLeft=(but->objectName()=="but_l");
    bool bRight=(but->objectName()=="but_r");
    bool bRestart=(but->objectName()=="but_restart");
    bool bFinish=(but->objectName()=="but_finish");
    QString pageName=mStack->currentWidget()->objectName();
    if(bRight||bFinish)
    {
        mSession=-1;
        mPi->stopAnimation();
        changePage("p_intro");
        return;
    }
    if(bLeft&&pageName=="p_intro")
    {
//        mBehindSession
        changePage("p_wait");
        tryRefreshData();
    }

    if(bLeft&&pageName=="p_data")
    {
        QDesktopServices::openUrl(QUrl("http://routerlogin.net"));
        setFinishPageStr(L_UPGRADE_FINISH_EXPLAIN,L_UPGRADE_COMPLETE_EXPLAIN3);
		changePage("p_finish");

		/////////////////////////////////kai.yan /////////////////////////////////////////
		/*QStringList dialog_lans;
        dialog_lans<<GENIE2_GET_TEXT(L_UPGRADE_MAIN_NAME)<<GENIE2_GET_TEXT(L_UPDATELAB_INTERNET_BREAK);
        sRCFlags |= RCF_MODAL_DIALOG_ON;
        bool dialogOk=GENIE2_UI_COMMAND(UICMD_DIALOG_MODAL,dialog_lans).toBool();
        sRCFlags &=~RCF_MODAL_DIALOG_ON;
        if(!dialogOk)
        {
            return;
        }
        changePage("p_update"); 
        mPi2->startAnimation();
        tryUpdateFirware();
		QGenieTimelineLocker *locker = QGenieTimelineLocker::GetSingletonPtr();
		Q_ASSERT(locker);
		locker->setNeedSingleFinish(true);
		locker->startTimer(3*60*1000,this,SLOT(slot_UpdateFinish()),2);*/

    }

	//if(bRestart&&pageName=="p_update"){
	//	changePage("p_restart");
	//	mPi3->startAnimation();

	//}
	//else if(pageName=="p_restart")
	//{
	//	changePage("p_finish");
	//	setRestartPageStr(L_RESTART_FINISHED,L_RESTART_FINISHED);
	//	mPi3->stopAnimation();
	//}

}
void QGenieFupdateObject::tryUpdateFirware()//added by hu
{
    mSession=QGenieIface2ShareObject::sSingleInstance->mNetgearInfoM
             ->netgearCommand(INetgearInterface::IC_UPDATE_NEWFIRMWARE,QStringList()
                              ,this,SLOT(slot_DataReturned2(int,QStringList)));
}

void QGenieFupdateObject::tryRefreshData()
{
    //delete by caiyujun
    //bug press cancel button then press next, would not show waiting state
    //after press cancel button, RCFlags would still be freshing state,should still get data
//    if(sRCFlags & RCF_FUPDATE_IN_REFRESHING)
//    {
//        return;
//    }
    //delete end
    sRCFlags |=RCF_FUPDATE_IN_REFRESHING;
    mSession=QGenieIface2ShareObject::sSingleInstance->mNetgearInfoM
             ->netgearCommand(INetgearInterface::IC_CHECK_NEWFIRMWARE2,QStringList()
                              ,this,SLOT(slot_DataReturned(int,QStringList)));
    mPi->startAnimation();
    but_l->setEnabled(false);
    but_r->setEnabled(true);
}
void QGenieFupdateObject::slot_DataReturned2(int session, const QStringList &lst){

    qDebug()<<"lst"<<session<<lst;

    //setUpdatePageStr(L_UPDATELAB_FINISHED,L_UPGRADE_CHECK_NOWVERSION);

    mPi2->stopAnimation();

    mPi3->stopAnimation();

}


void QGenieFupdateObject::slot_DataReturned(int session, const QStringList &lst)
{
    bool btemp; 
    int c=lst.count();
    bool requestOk=(c>0 && (0==lst[0].toInt(&btemp)) && btemp);
    qDebug()<<"slot_DataReturned----lst"<<lst;

    if(session==mSession )
    {
        sRCFlags &= ~RCF_FUPDATE_IN_REFRESHING;
        but_l->setEnabled(true);
        but_r->setEnabled(true);
        if(requestOk)
        {
            mCurV=(c>1)?lst[1]:"";
            mNewV=(c>2)?lst[2]:"";
            setVersionStr();
            mPi->stopAnimation();
            int itemp=compareVersion(mNewV,mCurV);
            if(itemp>0)
            {
                changePage("p_data");
                sRCFlags |= RCF_HAVE_FIRMWARE_UPDATE;
                sRCFlags |= RCF_FUPDATE_DATA_OK;
//                sRCFlags |= RCF_FIRMWARE_UDPATE_DATAOK;
//                GENIE2_UI_COMMAND(UICMD_TITLE_UPDATE,"firm_on");
            }
            else
            {
                changePage("p_finish");
                setFinishPageStr(L_UPGRADE_MAIN_NAME,L_UPGRADE_CHECK_ISLASTEST);
                sRCFlags &= ~RCF_HAVE_FIRMWARE_UPDATE;
                sRCFlags &= ~RCF_FUPDATE_DATA_OK;
//                sRCFlags &= ~RCF_FIRMWARE_UDPATE_DATAOK;
//                GENIE2_UI_COMMAND(UICMD_TITLE_UPDATE,"firm_off");
            }
            mTimer.stop();
        }
        else
        {
            sRCFlags &= ~RCF_HAVE_FIRMWARE_UPDATE;
            sRCFlags &= ~RCF_FUPDATE_DATA_OK;

            changePage("p_finish");
            setFinishPageStr(L_UPGRADE_MAIN_NAME,L_UPGRADE_FINISH_EXPLAIN,L_UPGRADE_MAIN_FAILED);
        }
    }
    QGenieRCWirelessObject *rcwo=QGenieRCWirelessObject::GetSingletonPtr();
    Q_ASSERT(rcwo);
    rcwo->updateUi();
}

///-------------kai.yan add ------------------
void QGenieFupdateObject::slot_UpdateFinish()
{
	sRCFlags &= ~RCF_HAVE_FIRMWARE_UPDATE;
	sRCFlags &= ~RCF_FUPDATE_DATA_OK;

	changePage("p_finish");
	setFinishPageStr(L_UPDATELAB_FINISHED,L_UPGRADE_CHECK_NOWVERSION);
    QGenieRCWirelessObject *rcwo=QGenieRCWirelessObject::GetSingletonPtr();
    Q_ASSERT(rcwo);
	rcwo->mWirelessW->setEnabled(true);
	sRCFlags &= ~RCF_LOCAL_LOGEDIN;
}
//------------- kai.yan end---------------------

void QGenieFupdateObject::slot_Timeout()
{
    if(--mRetryTimes<=0)
    {
        mTimer.stop();
    }
    else
    {
        mSession=QGenieIface2ShareObject::sSingleInstance->mNetgearInfoM
                   ->netgearCommand(INetgearInterface::IC_CHECK_NEWFIRMWARE2,QStringList()
                                    ,this,SLOT(slot_DataReturned(int,QStringList)));
    }

}
static QString formatS(const QString &s1,const QString &s2,QFontMetrics &fm)
{
    const int cSecondX=200;
    const int cSpaceW=fm.width(" ");
    const int w=fm.width(s1);
    int cCount=(cSecondX-w)/cSpaceW;
    QString ret=s1;
    while(cCount-- >0){ret+=" ";}
    ret+=s2+"\n";
    return ret;
}

void QGenieFupdateObject::setVersionStr()
{
    QLabel *lab_version=mStack->findChild<QLabel *>("lab_version");
    Q_ASSERT(lab_version);
    QFontMetrics fm(lab_version->font());

    QGenieRCWirelessObject *rcwo=QGenieRCWirelessObject::GetSingletonPtr();
    Q_ASSERT(rcwo);

    QString stemp=formatS(GENIE2_GET_TEXT(L_RTCF_ROUTETYPE)+":",rcwo->mModel,fm)
                  +formatS(GENIE2_GET_TEXT(L_UPGRADE_CHECK_NOWVERSION),mCurV,fm)
                  +formatS(GENIE2_GET_TEXT(L_UPGRADE_CHECK_NEWVERSION),mNewV,fm);

    lab_version->setText(stemp);
}

void QGenieFupdateObject::setFinishPageStr(int lan1, int lan2, int lan3)
{
    mLanFinish1=lan1;
    mLanFinish2=lan2;
    mLanFinish3=lan3;
    struct _{const char *obj;int lan;} cParas[]={
        {"lab_finish1",mLanFinish1}
        ,{"lab_finish2",mLanFinish2}
        ,{"lab_finish3",mLanFinish3}
    };
    for(int i=0;i<sizeof(cParas)/sizeof(cParas[0]);i++)
    {
        QLabel *lab=mStack->findChild<QLabel *>(cParas[i].obj);
		Q_ASSERT(lab);
		if(cParas[i].lan == L_UPGRADE_CHECK_NOWVERSION)////kai.yan modify for update firmware
		{
			lab->setText((cParas[i].lan<0)?"":GENIE2_GET_TEXT(cParas[i].lan)+mNewV);
		}else
		{
			lab->setText((cParas[i].lan<0)?"":GENIE2_GET_TEXT(cParas[i].lan));
		}
    }

}
void QGenieFupdateObject::setUpdatePageStr(int lan1, int lan2)
{
    mLanFinish1=lan1;
    mLanFinish2=lan2;
    struct _{const char *obj;int lan;} cParas[]={
        {"lab_update1",mLanFinish1}
        ,{"lab_update2",mLanFinish2}

    };
    for(int i=0;i<sizeof(cParas)/sizeof(cParas[0]);i++)
    {
        QLabel *lab=mStack->findChild<QLabel *>(cParas[i].obj);
        Q_ASSERT(lab);
		if(L_UPGRADE_CHECK_NOWVERSION == cParas[i].lan)
		{
			lab->setText((cParas[i].lan<0)?"":GENIE2_GET_TEXT(cParas[i].lan)+mNewV);
		}else
		{
			lab->setText((cParas[i].lan<0)?"":GENIE2_GET_TEXT(cParas[i].lan));
		}
    }

}
void QGenieFupdateObject::setRestartPageStr(int lan1, int lan2)
{
    mLanFinish1=lan1;
    mLanFinish2=lan2;
    struct _{const char *obj;int lan;} cParas[]={
        {"lab_finish1",mLanFinish1}
        ,{"lab_finish2",mLanFinish2}
    };
    for(int i=0;i<sizeof(cParas)/sizeof(cParas[0]);i++)
    {
        QLabel *lab=mStack->findChild<QLabel *>(cParas[i].obj);
        Q_ASSERT(lab);
        lab->setText((cParas[i].lan<0)?"":GENIE2_GET_TEXT(cParas[i].lan));
    }

}
void QGenieFupdateObject::translateUi()
{
    if(!mW)
    {
        return;
    }
    struct _{const char *obj;int lan;} cParas[]={
        {"lab_intro1",L_UPGRADE_MAIN_NAME}
        ,{"lab_intro2",L_UPGRADE_INTRO_EXPLAIN}
        ,{"lab_wait1",L_UPGRADE_MAIN_NAME}
        ,{"lab_wait2",L_UPGRADE_INTRO_EXPLAIN}
        ,{"lab_wait3",L_UPGRADE_INTRO_WAIT}
        ,{"lab_data1",L_UPGRADE_CHECK_TITLE}
        ,{"lab_data2",L_UPGRADE_CHECK_EXPLAIN}
       // ,{"lab_data3",L_UPGRADE_UPDATE_DOWN_FILE}
        ,{"lab_finish1",mLanFinish1}
        ,{"lab_finish2",mLanFinish2}
        ,{"lab_finish3",mLanFinish3}
        ,{"lab_update1",L_UPDATELAB_UPDATEING}//hu
        ,{"lab_update2",L_UPDATELAB_FIRMWARE_NEEDFEWTIME}//hu
        ,{"lab_restart1",L_RESTART_ROUTER}//hu

        ,{"but_l",L_UPGRADE_WIZARD_NEXTBUT}
        ,{"but_r",L_RTCF_CANCEL}
        ,{"but_finish",L_UPGRADE_WIZARD_NEXTBUT_FINISH}
        ,{"but_restart",L_UPDATELAB_RESTART}//hu


    };
    for(int i=0;i<sizeof(cParas)/sizeof(cParas[0]);i++)
    {
        QSetTextHelper(mW->findChild<QWidget *>(cParas[i].obj))
                .setText(GENIE2_GET_TEXT(cParas[i].lan));
    }
    setVersionStr();
}

void QGenieFupdateObject::wantRefreshData()
{
    if(mW)
    {
        sRCFlags &= ~RCF_HAVE_FIRMWARE_UPDATE;
        changePage("p_intro");
        mTimer.stop();
        mTimer.start(TIMER_DELAY);
        QGenieRCWirelessObject::GetSingletonPtr()->updateUi();
    }

}





void QGenieFupdateObject::whenLogin()
{
    mSession=-1;
    changePage("p_intro");
}

void QGenieFupdateObject::changePage(const QString &name)
{
    if(mStack)
    {
        QWidget *w=mStack->findChild<QWidget*>(name);
        Q_ASSERT(w);
        if(w)
        {
            mStack->setCurrentWidget(w);
        }
        const quint8 cVisibleF=1;
        const quint8 cEnableF=(1<<1);

        const quint8 cParas[][4]={
            // but_finish       but_l                   but_r              but_restart
            {   0               ,cVisibleF|cEnableF     ,cVisibleF           ,0} //p_intro
            ,{  0               ,cVisibleF              ,cVisibleF|cEnableF  ,0} //p_wait
            ,{  0               ,cVisibleF|cEnableF     ,cVisibleF|cEnableF  ,0} //p_data
            ,{cVisibleF|cEnableF,0                      ,0                   ,0} //p_finish
            ,{  0               ,0                      ,0				     ,0} //p_update
            ,{cVisibleF|cEnableF,0                      ,0                   ,0} //p_restart
       };
        int idx=mStack->currentIndex();
        Q_ASSERT(idx>=0 && idx<(sizeof(cParas)/sizeof(cParas[0])));
        but_finish->setVisible(cParas[idx][0]&cVisibleF);
        but_finish->setEnabled(cParas[idx][0]&cEnableF);
        but_l->setVisible(cParas[idx][1]&cVisibleF);
        but_l->setEnabled(cParas[idx][1]&cEnableF);
        but_r->setVisible(cParas[idx][2]&cVisibleF);
        but_r->setEnabled(cParas[idx][2]&cEnableF);
        but_restart->setVisible(cParas[idx][3]&cVisibleF);
        but_restart->setEnabled(cParas[idx][3]&cEnableF);
    }
}
