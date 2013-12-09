#include "QGenieTrafficMeterObject.h"
#include <QUiLoader>
#include <QtCore>
#include <QtGui>
#include <QDate>
#include "genie2_interface.h"
#include "genie2_netgear.h"
#include "language_RouterConfig.h"
#include "QGenieRCWirelessObject.h"
#include "QPiObject.h"
#include "QGenieTimelineLocker.h"
#include "SvtFcmlError.h"
#include "QGenieTimeGenerator.h"

extern quint64 sRCFlags;
const int cErrorLabGeometry[]={10,30,400,20};
const int cPiGeometry[]={470,2,40,40};
#define ERROR_MODE "<center><font color='red'>%1</font>"\
"<a href=\"http://www.netgear.com/landing/en-us/netgear-genie-faq.aspx\"><img src=\":/images/faq.png\" /></a></center>"


QGenieTrafficMeterObject::QGenieTrafficMeterObject(QObject *parent)
    :QObject(parent)
    ,mW(NULL)
    ,mSessionTMOption(-1)
    ,mSessionTMStatistics(-1)
    ,mErrorLabel(NULL)
    ,mErrorLan(-1)
    ,mPi(NULL)
    ,totalchart(NULL)
    ,avgchart(NULL)
{
}

QWidget *QGenieTrafficMeterObject::getUi()
{
    if(!mW)
    {
        QUiLoader loader;
        QFile f(GENIE2_RES("ui/trafficmetter.ui"));
        bool btemp=f.open(QFile::ReadOnly);
        Q_ASSERT(btemp);
        QWidget *w=loader.load(&f,QGenieIface2ShareObject::sSingleInstance->mMainFrame);
        w->hide();
        Q_ASSERT(w);

        //mErrorLabel=new QLabel(w);
        //mErrorLabel->setGeometry(cErrorLabGeometry[0],cErrorLabGeometry[1],cErrorLabGeometry[2],cErrorLabGeometry[3]);
        //mErrorLabel->hide();
		//QString style="QLabel{background-color:yellow}";
		//w->setStyleSheet(style);


        //table_statistics=qFindChild<QTableWidget *>(w,"table_statistics");
        rdt_enable=w->findChild<QRadioButton *>("rdt_enable");
        rdt_disable=w->findChild<QRadioButton *>("rdt_disable");
        but_modify=w->findChild<QPushButton *>("but_modify");
        but_apply=w->findChild<QPushButton *>("but_apply");
        Q_ASSERT(table_statistics &&rdt_enable
                 && rdt_disable&& but_modify&&but_apply);


        for(int i=0; i <= 5; i++)
        {
            for(int j=0; j<=4; j++)
            {
                //table_statistics->setItem(i,j,new QTableWidgetItem());
                //table_statistics->item(i,j)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
                //table_statistics->item(i,j)->setFlags(Qt::NoItemFlags);
            }
        }

        QLineEdit *edt_month_limit=w->findChild<QLineEdit *>("edt_month_limit");
        QLineEdit *edt_reset_hour=w->findChild<QLineEdit *>("edt_reset_hour");
        QLineEdit *edt_reset_minute=w->findChild<QLineEdit *>("edt_reset_minute");
        Q_ASSERT(edt_month_limit &&edt_reset_hour &&edt_reset_minute);

        QRegExpValidator *validate=new QRegExpValidator(QRegExp("^[1-9]\\d{0,5}|0$"),edt_month_limit);
        QIntValidator *hourValidator = new QIntValidator(0,11,edt_reset_hour);
        QIntValidator *minuteValidator = new QIntValidator(0,59, edt_reset_minute);

        edt_month_limit->setValidator(validate);
        edt_reset_hour->setValidator(hourValidator);
        edt_reset_minute->setValidator(minuteValidator);

        QList<QLineEdit*> lineedit_lst=w->findChildren<QLineEdit *>();
        QList<QComboBox *> cob_lst=w->findChildren<QComboBox *>();
        int c=lineedit_lst.count();
        for(int i=0;i<c;i++)
        {
            connect(lineedit_lst[i],SIGNAL(textChanged(QString)),this,SLOT(slot_uiModified()));
        }
        c=cob_lst.count();
        for(int i=0;i<c;i++)
        {
            connect(cob_lst[i],SIGNAL(currentIndexChanged(int)),this,SLOT(slot_uiModified()));
        }
        QList<QPushButton *> bts=w->findChildren<QPushButton *>();
        c=bts.count();
        for(int i=0;i<c;i++)
        {
            connect(bts[i],SIGNAL(clicked()),this,SLOT(slot_buttonClicked()));
        }

        QList<QRadioButton *> rdts=w->findChildren<QRadioButton *>();
        c=rdts.count();
        for(int i=0;i<c;i++)
        {
            connect(rdts[i],SIGNAL(clicked()),this,SLOT(slot_radioButclicked()));
        }

        //mPi=new QProgressIndicator(w);
        //mPi->setGeometry(cPiGeometry[0],cPiGeometry[1],cPiGeometry[2],cPiGeometry[3]);
        //mPi->show();
        deployIndicators(w);
        mErrorLabel = w->findChild<QLabel *>("lab_err");
        Q_ASSERT(mErrorLabel);
        //mErrorLabel->hide();
		
        stackedwidget = w->findChild<QStackedWidget *>("stackedWidget");
        Q_ASSERT(stackedwidget);
        combobox = w->findChild<QComboBox *>("comboBox");
        Q_ASSERT(combobox);
        stackedwidget->setVisible(false);
        combobox->setVisible(false);
        {
            if(!totalchart)
            {
            QList<int> xbartitle;
            xbartitle << L_TRAFFICMETER_ROW_1
                    << L_TRAFFICMETER_ROW_2
                    << L_TRAFFICMETER_ROW_3
                    << L_TRAFFICMETER_ROW_4
                    <<L_TRAFFICMETER_ROW_5;
            totalchart = new BarChart(L_TRAFFICMETER_TOTAL,L_TRAFFICMETER_MBYTES,xbartitle,w);
            stackedwidget->addWidget(totalchart);
            }

            if(!avgchart)
            {
                QList<int> xbartitle;
                xbartitle.clear();
                xbartitle << L_TRAFFICMETER_ROW_3
                        << L_TRAFFICMETER_ROW_4
                        <<L_TRAFFICMETER_ROW_5;
                avgchart = new BarChart(L_TRAFFICMETER_AVG,L_TRAFFICMETER_MBYTES,xbartitle,w);
                stackedwidget->addWidget(avgchart);
            }

            qDebug() << "stackedwidget" << stackedwidget->count();
            stackedwidget->setCurrentWidget(totalchart);
            connect(combobox,SIGNAL(currentIndexChanged(int)),stackedwidget,SLOT(setCurrentIndex(int)));
        }

        mW=w;
        translateUi();
    }
    return mW;
}

void QGenieTrafficMeterObject::translateUi()
{
    if(!mW)
    {
        return;
    }
    const struct _{const char *name;int lan;int type;}
    cPara[]={
        //        {"lab_enable_l", L_RTCF_LOGIN_TITLE, 0}
        //        ,{"lab_ssid_l",L_GACC_GUESTWIRELESS_SSID,0}
        //        ,{"lab_pwd_l",L_RTCF_PASSWORD,0}
        //        ,{"lab_set_ssid",L_GACC_GUESTWIRELESS_SSID,0}
        //        ,{"lab_set_period",L_GACC_WORKTIME,0}
        //        ,{"lab_set_secure",L_RTCF_SETTING_SECURITY,0}
		
		////////2012-3-6 chensi//////////////
		{"lab_controlby", L_TRAFFICMETER_CONTROLBY, 0}
		,{"lab_monthby", L_TRAFFICMETER_MONTHBY, 0}
		,{"lab_mbytes", L_TRAFFICMETER_MBYTES, 0}
		,{"lab_counter", L_TRAFFICMETER_COUNTER, 0}
		,{"lab_restartcounter", L_TRAFFICMETER_RESTARTCOUNTER, 0}
		,{"lab_counter_at", L_TRAFFICMETER_COUNTER_AT, 0}
		,{"lab_counter_day", L_TRAFFICMETER_COUNTER_DAY, 0}

        ,{"rdt_enable",L_TRAFFICMETER_ENABLE,3}
        ,{"rdt_disable",L_TRAFFICMETER_DISABLE,3}
        ,{"but_refresh",L_RTCF_INFO_REFRESH,1}

        ,{"but_modify",L_RTCF_INFO_MODIFY,1}
        ,{"but_logout",L_RTCF_INFO_LOGINOUT,1}
        ,{"but_apply",L_RTCF_SETTING_APPLY,1}
        ,{"but_cancel",L_RTCF_CANCEL,1}
    };
    for(int i=0;i<sizeof(cPara)/sizeof(cPara[0]);i++)
    {
        if(0==cPara[i].type)
        {
            QLabel *lab=mW->findChild<QLabel *>(cPara[i].name);
            Q_ASSERT(lab);
            lab->setText(GENIE2_GET_TEXT(cPara[i].lan));
        }
        if(1==cPara[i].type)
        {
            QPushButton *but=mW->findChild<QPushButton *>(cPara[i].name);
            Q_ASSERT(but);
            but->setText(GENIE2_GET_TEXT(cPara[i].lan));
        }
        if(3==cPara[i].type)
        {
            QRadioButton *rdt=mW->findChild<QRadioButton *>(cPara[i].name);
            Q_ASSERT(rdt);
            rdt->setText(GENIE2_GET_TEXT(cPara[i].lan));
        }
    }

    if(mErrorLabel)
    {
        mErrorLabel->setText((mErrorLan<0)?"":QString(ERROR_MODE).arg(GENIE2_GET_TEXT(mErrorLan)));
        //mErrorLabel->setVisible(mErrorLan>=0);
    }

    QComboBox *cob_limit_type=mW->findChild<QComboBox *>("cob_limit_type");
    QComboBox *cob_morning=mW->findChild<QComboBox *>("cob_morning");
    QComboBox *cob_reset_day=mW->findChild<QComboBox *>("cob_reset_day");


    Q_ASSERT(cob_limit_type && cob_morning&&cob_reset_day);
    cob_limit_type->clear();
    cob_limit_type->insertItems(0,QStringList()
                                <<GENIE2_GET_TEXT(L_TRAFFICMETER_NO_LIMIT)
                                <<GENIE2_GET_TEXT(L_TRAFFICMETER_DOWNLOAD_ONLY)
                                <<GENIE2_GET_TEXT(L_TRAFFICMETER_BOTH_DIRECTIONS)
                                );
    cob_morning->clear();
    cob_morning->insertItems(0,QStringList()
                             <<GENIE2_GET_TEXT(L_TRAFFICMETER_AM)
                             <<GENIE2_GET_TEXT(L_TRAFFICMETER_PM)
                             );
    cob_reset_day->clear();
    QStringList templist;
    for(int i=L_TRAFFICMETER_01ST;i<=L_TRAFFICMETER_29TH;i++)
    {
        templist<<GENIE2_GET_TEXT(i);
    }


    cob_reset_day->insertItems(0,templist);

    combobox->clear();
    combobox->insertItems(0,QStringList() << GENIE2_GET_TEXT(L_TRAFFICMETER_TOTAL)
                          << GENIE2_GET_TEXT(L_TRAFFICMETER_AVG));
}

//void QGenieTrafficMeterObject::whenLogin()
//{
////    mbRefreshAfterLogin=true;
//}

//void QGenieTrafficMeterObject::tabClicked()
//{
//    if(sRCFlags & RCF_MUST_REFRESH_TM)
//    {
////        mbRefreshAfterLogin=false;
//        slot_RefreshData();
//    }
//}

void QGenieTrafficMeterObject::refreshData(bool remote)
{
    sRCFlags &= ~RCF_MUST_REFRESH_TM;
    QGenieRCWirelessObject *rcwo=QGenieRCWirelessObject::GetSingletonPtr();
    Q_ASSERT(rcwo);

    mPi->startAnimation();
    setError(-1);
    QStackedWidget *stack_tm=mW->findChild<QStackedWidget*>("stack_tm");
    Q_ASSERT(stack_tm);
    stack_tm->setCurrentIndex(0);
    if(remote)
    {
        sRCFlags |= RCF_IN_REMOTE_REFRESHING;
        mSessionTMOption=QGenieIface2ShareObject::sSingleInstance->mNetgearInfoM
                         ->netgearCommand(INetgearInterface::IC_SM_GET_TM_OPTION
                                          ,QStringList()<<rcwo->getCpName(),this,SLOT(slot_DataRreturned(int,QStringList)));
    }
    else
    {
        sRCFlags |= RCF_IN_LOCAL_REFRESHING;
        mSessionTMOption=QGenieIface2ShareObject::sSingleInstance->mNetgearInfoM
                         ->netgearCommand(INetgearInterface::IC_GET_TM_OPION
                                          ,QStringList(),this,SLOT(slot_DataRreturned(int,QStringList)));
    }
}

void QGenieTrafficMeterObject::slot_RefreshData()
{
    refreshData(sRCFlags & RCF_REMOTE_LOGEDIN);
}

void QGenieTrafficMeterObject::slot_buttonClicked()
{
    QPushButton *but=qobject_cast<QPushButton *>(sender());
    Q_ASSERT(but);
    QString obj_name=but->objectName();
    QStackedWidget *stack_tm=mW->findChild<QStackedWidget*>("stack_tm");
    Q_ASSERT(stack_tm);
    QGenieRCWirelessObject *rcwo=QGenieRCWirelessObject::GetSingletonPtr();
    Q_ASSERT(rcwo);
    if(obj_name=="but_modify")
    {
        stack_tm->setCurrentIndex(1);
    }
    else if(obj_name=="but_cancel")
    {
        stack_tm->setCurrentIndex(0);
    }
    else if(obj_name=="but_refresh")
    {
        slot_RefreshData();
    }
    else if(obj_name=="but_apply")
    {
        setTMOptions();
    }
    else if(obj_name=="but_logout")
    {
        rcwo->logOut(IS_REMOTE_UI());
    }
    rcwo->updateUi();
}

void QGenieTrafficMeterObject::slot_radioButclicked()
{
    QRadioButton *rdt=qobject_cast<QRadioButton*>(sender());
    Q_ASSERT(rdt);
    QString obj_name=rdt->objectName();
    QGenieTimelineLocker *locker=QGenieTimelineLocker::GetSingletonPtr();
    Q_ASSERT(locker);
    QGenieRCWirelessObject *rcwo=QGenieRCWirelessObject::GetSingletonPtr();
    Q_ASSERT(rcwo);
    if(obj_name=="rdt_enable" && !mSavedEnableState)
    {
        QStringList lans;
        lans<<GENIE2_GET_TEXT(L_RTCF_TRAFFICMETTER)<<GENIE2_GET_TEXT(L_TRAFFICMETER_MSGBOX_ENABLE);
        sRCFlags |= RCF_MODAL_DIALOG_ON;
        bool dialogOk=GENIE2_UI_COMMAND(UICMD_DIALOG_MODAL,lans).toBool();
        sRCFlags &=~RCF_MODAL_DIALOG_ON;
        if(dialogOk)
        {
            GENIE2_STATISTICS("R11");
            if(IS_REMOTE_UI())
            {
                QGenieIface2ShareObject::sSingleInstance->mNetgearInfoM
                        ->netgearCommand(INetgearInterface::IC_SM_SET_TM_ENABLE
                                         ,QStringList()<<rcwo->getCpName()<<"1"
                                         ,NULL,NULL);

            }
            else
            {
                QGenieIface2ShareObject::sSingleInstance->mNetgearInfoM
                        ->netgearCommand(INetgearInterface::IC_SET_TM_ENABLE,QStringList()<<"1",NULL,NULL);
            }

            int t=IS_REMOTE_UI()?SMARTNETWORK_WAIT_TIME:20;

            locker->startTimer(t*1000,this,SLOT(slot_RefreshData())/*,mPi*/);
        }
        else
        {
            rdt_disable->setChecked(true);
        }

    }
    else if(obj_name=="rdt_disable" && mSavedEnableState)
    {
        QStringList lans;
        lans<<GENIE2_GET_TEXT(L_RTCF_TRAFFICMETTER)<<GENIE2_GET_TEXT(L_TRAFFICMETER_MSGBOX_DISABLE);
        sRCFlags |= RCF_MODAL_DIALOG_ON;
        bool dialogOk=GENIE2_UI_COMMAND(UICMD_DIALOG_MODAL,lans).toBool();
        sRCFlags &=~RCF_MODAL_DIALOG_ON;
        if(dialogOk)
        {
            GENIE2_STATISTICS("R12");
            if(IS_REMOTE_UI())
            {
                QGenieIface2ShareObject::sSingleInstance->mNetgearInfoM
                        ->netgearCommand(INetgearInterface::IC_SM_SET_TM_ENABLE
                                         ,QStringList()<<rcwo->getCpName()<<"0"
                                         ,NULL,NULL);
            }
            else
            {
                QGenieIface2ShareObject::sSingleInstance->mNetgearInfoM
                        ->netgearCommand(INetgearInterface::IC_SET_TM_ENABLE,QStringList()<<"0",NULL,NULL);

            }
            int t=IS_REMOTE_UI()?SMARTNETWORK_WAIT_TIME:20;
            locker->startTimer(t*1000,NULL,NULL/*,mPi*/);
            QStackedWidget *stack_tm=mW->findChild<QStackedWidget *>("stack_tm");
            Q_ASSERT(stack_tm);
//            table_statistics->setVisible(false);
            combobox->setVisible(false);
            stackedwidget->setVisible(false);
            mSavedEnableState=false;
            //add by caiyujun for bug 1534
            //bug 1534:Modify button should became grey after disable Traffic Meter
            but_modify=mW->findChild<QPushButton *>("but_modify");
            but_modify->setEnabled(false);
            //add end
        }
        else
        {
            rdt_enable->setChecked(true);
        }
    }
    rcwo->updateUi();
}

void QGenieTrafficMeterObject::slot_DataRreturned(int session,const QStringList &lst)
{
    QGenieRCWirelessObject *rcwo=QGenieRCWirelessObject::GetSingletonPtr();
    Q_ASSERT(rcwo);

	bool btemp=false;
	int length=lst.count();
	int rvalue=(length>0)?lst[0].toInt(&btemp):-1;
	bool requestOk=((0==rvalue) && btemp);
	
    if(sRCFlags & RCF_REMOTE_LOGEDIN)
    {
        Q_ASSERT(sRCFlags &RCF_REMOTE);

        bool wantReLoginCP=((FE_WRONG_USER_OR_PWD==rvalue) && btemp);
        bool wantReAuthen=((FE_NOT_AUTHENTICATIED==rvalue)&&btemp);

        if(requestOk)
        {
            QTellLPCHelper::GetSingleton().tryTellLPC();
            rcwo->tryUpdateFirmware();
        }

        if(wantReAuthen)
        {
            rcwo->setLoginError(L_RTCF_MSG_PLEASERELOGIN,true);
            rcwo->logOut(true);
            return;
        }
        if(wantReLoginCP  )
        {
            if(rcwo->tryReloginCP())
            {
                if(session==mSessionTMOption)
                {
                    mSessionTMOption=QGenieIface2ShareObject::sSingleInstance->mNetgearInfoM
                            ->netgearCommand(INetgearInterface::IC_SM_GET_TM_OPTION
                                             ,QStringList()<<rcwo->getCpName(),this,SLOT(slot_DataRreturned(int,QStringList)));
                }
                else if(session == mSessionTMStatistics)
                {
                    mSessionTMStatistics=QGenieIface2ShareObject::sSingleInstance->mNetgearInfoM
                            ->netgearCommand(INetgearInterface::IC_SM_GET_TM_STATISTICS
                                             ,QStringList()<<rcwo->getCpName()
                                             ,this,SLOT(slot_DataRreturned(int,QStringList)));

                }
            }
            else
            {
				rcwo->mWirelessW->setEnabled(true);
                but_modify->setEnabled(false);
                rdt_enable->setEnabled(false);
                rdt_disable->setEnabled(false);
                stackedwidget->setCurrentIndex(0);
                sRCFlags|=RCF_MUST_REFRESH_TM;
                sRCFlags &= ~RCF_IN_REMOTE_REFRESHING;
                mPi->stopAnimation();
            }
			if(requestOk)
			{
				rcwo->updateUi();
			}
            return;
        }
    }
    if(session==mSessionTMOption)
    {
        sessionOptionReturned(lst);
    }
    else if(session==mSessionTMStatistics)
    {
        sessionStatisticsReturned(lst);
    }
	
	if(requestOk)
	{
		rcwo->updateUi();///kai.yan modify
	}

}

void QGenieTrafficMeterObject::slot_uiModified()
{
    QLineEdit *edt_month_limit=mW->findChild<QLineEdit *>("edt_month_limit");
    QLineEdit *edt_reset_hour=mW->findChild<QLineEdit *>("edt_reset_hour");
    QLineEdit *edt_reset_minute=mW->findChild<QLineEdit *>("edt_reset_minute");
    Q_ASSERT(edt_month_limit &&edt_reset_hour &&edt_reset_minute);
    QComboBox *cob_limit_type=mW->findChild<QComboBox *>("cob_limit_type");
    QComboBox *cob_reset_day=mW->findChild<QComboBox *>("cob_reset_day");
    QComboBox *cob_morning=mW->findChild<QComboBox *>("cob_morning");
    Q_ASSERT(cob_limit_type &&cob_reset_day &&cob_morning);

    //add by caiyujun for bug 1540
    //bug 1540:It's better to edit monthly limit when the traffic meter is "No Limit"
    if (0 == cob_limit_type->currentIndex())
    {
        edt_month_limit->setEnabled(false);
    }
    else
    {
        edt_month_limit->setEnabled(true);
    }
    //add end

    bool text_valid=!edt_month_limit->text().isEmpty()
                    && !edt_reset_hour->text().isEmpty()
                    && !edt_reset_minute->text().isEmpty();
    bool changed=(edt_month_limit->text()!=mSavedMonthLimit)
                 ||(edt_reset_hour->text()!=mSavedResetHour)
                 ||(edt_reset_minute->text()!=mSavedResetMinute)
                 ||(cob_limit_type->currentIndex()!=mSavedLimitType)
                 ||(cob_morning->currentIndex()!=mSavedMorning)
                 ||(cob_reset_day->currentIndex()!=mSavedResetDay);
    but_apply->setEnabled(text_valid&&changed);

}

void QGenieTrafficMeterObject::sessionOptionReturned(const QStringList &lst)
{
    QGenieRCWirelessObject *rcwo=QGenieRCWirelessObject::GetSingletonPtr();
    Q_ASSERT(rcwo);

    bool btemp;
    int c=lst.count();
    bool soapOk=(c>0 && (0==lst[0].toInt(&btemp)) && btemp);
	
    if(!soapOk)
    {
        int itemp;

        if(c>0
           && ((itemp=lst[0].toInt(&btemp))/100==4 || itemp/100==5)
           && btemp)
        {
            setError(L_TRAFFICMETER_BOX_2_MESSAGE);
        }
        else
        {
            setError(L_TRAFFICMETER_BOX_3_MESSAGE);
        }
        //table_statistics->setVisible(false);
        combobox->setVisible(false);
        stackedwidget->setVisible(false);

        mPi->stopAnimation();
        if(sRCFlags & RCF_REMOTE)
        {
            sRCFlags &=~RCF_IN_REMOTE_REFRESHING;
        }
        else
        {
            sRCFlags &=~RCF_IN_LOCAL_REFRESHING;
        }
		rcwo->mWirelessW->setEnabled(true);
        but_modify->setEnabled(false);
        rdt_enable->setEnabled(false);
        rdt_disable->setEnabled(false);
        sRCFlags |= RCF_MUST_REFRESH_TM;
        return;
    }
    bool enabled=(c>1 && (1==lst[1].toInt(&btemp))&&btemp);
    mSavedEnableState=enabled;

    rdt_enable->setEnabled(true);
    rdt_disable->setEnabled(true);

    if(!enabled)
    {
        rdt_disable->setChecked(true);
        mPi->stopAnimation();
        //table_statistics->setVisible(false);
        combobox->setVisible(false);
        stackedwidget->setVisible(false);

//        rcwo->mWirelessW->setEnabled(true);
        if(sRCFlags & RCF_REMOTE)
        {
            sRCFlags &= ~RCF_IN_REMOTE_REFRESHING;
        }
        else
        {
            sRCFlags &= ~RCF_IN_LOCAL_REFRESHING;
        }

        but_modify->setEnabled(false);
    }
    else
    {
        QString stemp;
        const QString &control_option=(c>2)?lst[2]:stemp;
        const QString &monthly_limit=(c>3)?lst[3]:stemp;
        const QString &restart_hour=(c >4)?lst[4]:stemp;
        const QString &restart_minute=(c>5)?lst[5]:stemp;
        const QString &restart_day=(c>6)?lst[6]:stemp;

        QComboBox *cob_limit_type=mW->findChild<QComboBox *>("cob_limit_type");
        QComboBox *cob_morning=mW->findChild<QComboBox *>("cob_morning");
        QComboBox *cob_reset_day=mW->findChild<QComboBox *>("cob_reset_day");
        QLineEdit *edt_reset_hour=mW->findChild<QLineEdit*>("edt_reset_hour");
        QLineEdit *edt_reset_minute=mW->findChild<QLineEdit*>("edt_reset_minute");
        QLineEdit *edt_month_limit=mW->findChild<QLineEdit*>("edt_month_limit");

        Q_ASSERT(cob_limit_type && cob_morning&&cob_reset_day
                 &&edt_reset_hour &&edt_reset_minute
                 &&edt_month_limit);
        const char *cPara[]={"No Limit","Download only","Both directions"};
        for(int i=0;i<sizeof(cPara)/sizeof(cPara[0]);i++)
        {
            if(0==control_option.compare(cPara[i],Qt::CaseInsensitive))
            {
                mSavedLimitType=i;
                cob_limit_type->setCurrentIndex(i);
                break;
            }
        }


        int hour = restart_hour.toInt();
        mSavedResetHour=QString("%1").arg(hour%12);
        edt_reset_hour->setText(mSavedResetHour);

        mSavedResetMinute=restart_minute;
        edt_reset_minute->setText(mSavedResetMinute);
        mSavedMorning=hour/12>0?1:0;
        cob_morning->setCurrentIndex(mSavedMorning);

		//LQ
		mSavedResetDay = restart_day.toInt() - 1;
        cob_reset_day->setCurrentIndex(mSavedResetDay<28? mSavedResetDay:28);

        mSavedMonthLimit=monthly_limit;
        edt_month_limit->setText(mSavedMonthLimit);

        rdt_enable->setChecked(true);
        if(sRCFlags & RCF_REMOTE_LOGEDIN)
        {
            mSessionTMStatistics=QGenieIface2ShareObject::sSingleInstance->mNetgearInfoM
                                 ->netgearCommand(INetgearInterface::IC_SM_GET_TM_STATISTICS
                                                  ,QStringList()<<rcwo->getCpName()
                                                  ,this,SLOT(slot_DataRreturned(int,QStringList)));

        }
        else
        {
            mSessionTMStatistics=QGenieIface2ShareObject::sSingleInstance->mNetgearInfoM
                                 ->netgearCommand(INetgearInterface::IC_GET_TM_STATISTICS
                                                  ,QStringList(),this,SLOT(slot_DataRreturned(int,QStringList)));
        }
    }
}

float getFirstValue(const QString &s)
{
    QStringList lst=s.split("/");
    float ret=lst.value(0).replace(",","").toFloat();
    return ret;
}

float getSecondValue(const QString &s)
{
    QStringList lst=s.split("/");
    float ret=lst.value(1).replace(",","").toFloat();
    return ret;
}

void QGenieTrafficMeterObject::sessionStatisticsReturned(const QStringList &lst)
{
    QGenieRCWirelessObject *rcwo=QGenieRCWirelessObject::GetSingletonPtr();
    Q_ASSERT(rcwo);


    bool btemp;
    int c=lst.count();
    bool soapOk=(c>0 && (0==lst[0].toInt(&btemp)) && btemp);
	
    if(!soapOk)
    {
        but_modify->setEnabled(false);
        if(sRCFlags &RCF_REMOTE)
        {
            sRCFlags &=~RCF_IN_REMOTE_REFRESHING;
        }
        else
        {
            sRCFlags &=~RCF_IN_LOCAL_REFRESHING;
        }
        mPi->stopAnimation();
        if(c>0 &&(401==lst[0].toInt(&btemp)) && btemp)
        {
            setError(L_TRAFFICMETER_BOX_2_MESSAGE);
        }
        else
        {
            setError(L_TRAFFICMETER_BOX_3_MESSAGE);
        }
        //table_statistics->setVisible(true);
		rcwo->mWirelessW->setEnabled(true);
        combobox->setVisible(true);
        stackedwidget->setVisible(true);
        sRCFlags |= RCF_MUST_REFRESH_TM;
    }
    else
    {
        //table_statistics->setVisible(true);
        combobox->setVisible(true);
        stackedwidget->setVisible(true);
        QString stemp;
        const QString &today_connection=(c>1)?lst[1]:stemp;
        const QString &today_up_avg=(c>2)?lst[2]:stemp;
        const QString &today_down_avg=(c>3)?lst[3]:stemp;
        const QString &yesterday_connection=(c>4)?lst[4]:stemp;
        const QString &yesterday_up_avg=(c>5)?lst[5]:stemp;
        const QString &yesterday_down_avg=(c>6)?lst[6]:stemp;
        const QString &week_connection=(c>7)?lst[7]:stemp;
        const QString &week_up_avg=(c>8)?lst[8]:stemp;
        const QString &week_down_avg=(c>9)?lst[9]:stemp;
        const QString &month_connection=(c>10)?lst[10]:stemp;
        const QString &month_up_avg=(c>11)?lst[11]:stemp;
        const QString &month_down_avg=(c>12)?lst[12]:stemp;
        const QString &lastmonth_connection=(c>13)?lst[13]:stemp;
        const QString &lastmonth_up_avg=(c>14)?lst[14]:stemp;
        const QString &lastmonth_down_avg=(c>15)?lst[15]:stemp;
        float itoday_up=getFirstValue(today_up_avg);
        float itoday_down=getFirstValue(today_down_avg);
        float iyesterday_up=getFirstValue(yesterday_up_avg);
        float iyesterday_down=getFirstValue(yesterday_down_avg);
        float iweek_up=getFirstValue(week_up_avg);
        float iweek_down=getFirstValue(week_down_avg);
        float iweek_up_avg=getSecondValue(week_up_avg);
        float iweek_down_avg=getSecondValue(week_down_avg);

        float imonth_up=getFirstValue(month_up_avg);
        float imonth_down=getFirstValue(month_down_avg);
        float imonth_up_avg=getSecondValue(month_up_avg);
        float imonth_down_avg=getSecondValue(month_down_avg);

        float ilastmonth_up=getFirstValue(lastmonth_up_avg);
        float ilastmonth_down=getFirstValue(lastmonth_down_avg);
        float ilastmonth_up_avg=getSecondValue(lastmonth_up_avg);
        float ilastmonth_down_avg=getSecondValue(lastmonth_down_avg);

        QList<float> totalchartdata;
        totalchartdata << itoday_up << itoday_down
                << iyesterday_up << iyesterday_down
                << iweek_up << iweek_down
                << imonth_up << imonth_down
                << ilastmonth_up << ilastmonth_down;
                totalchart->updateData(totalchartdata);


        QList<float> avgchartdata;
        avgchartdata << iweek_up_avg << iweek_down_avg
                << imonth_up_avg << imonth_down_avg
                << ilastmonth_up_avg << ilastmonth_down_avg;
        avgchart->updateData(avgchartdata);
        //table_statistics->item(1,1)->setText(today_connection);
        //table_statistics->item(1,2)->setText(QString("%1").arg(itoday_up));
        //table_statistics->item(1,3)->setText(QString("%1").arg(itoday_down));
        //table_statistics->item(1,4)->setText(QString("%1").arg(itoday_up+itoday_down));

        //table_statistics->item(2,1)->setText(yesterday_connection);
        //table_statistics->item(2,2)->setText(QString("%1").arg(iyesterday_up));
        //table_statistics->item(2,3)->setText(QString("%1").arg(iyesterday_down));
        //table_statistics->item(2,4)->setText(QString("%1").arg(iyesterday_up+iyesterday_down));

        //table_statistics->item(3,1)->setText(week_connection);
        //table_statistics->item(3,2)->setText(week_up_avg);
        //table_statistics->item(3,3)->setText(week_down_avg);
        //table_statistics->item(3,4)->setText
        //        (QString("%1/%2").arg(iweek_up+iweek_down).arg(iweek_up_avg+iweek_down_avg));

        //table_statistics->item(4,1)->setText(month_connection);
        //table_statistics->item(4,2)->setText(month_up_avg);
        //table_statistics->item(4,3)->setText(month_down_avg);
        //table_statistics->item(4,4)->setText
        //        (QString("%1/%2").arg(imonth_up+imonth_down).arg(imonth_up_avg+imonth_down_avg));

        //table_statistics->item(5,1)->setText(lastmonth_connection);
        //table_statistics->item(5,2)->setText(lastmonth_up_avg);
        //table_statistics->item(5,3)->setText(lastmonth_down_avg);
        //table_statistics->item(5,4)->setText
        //        (QString("%1/%2").arg(ilastmonth_up+ilastmonth_down).arg(ilastmonth_up_avg+ilastmonth_down_avg));


        but_modify->setEnabled(true);
        mPi->stopAnimation();
    }

    if(sRCFlags &RCF_REMOTE)
    {
        sRCFlags &=~RCF_IN_REMOTE_REFRESHING;
    }
    else
    {
        sRCFlags &=~RCF_IN_LOCAL_REFRESHING;
    }
}

void QGenieTrafficMeterObject::setTMOptions()
{
    QComboBox *cob_limit_type=mW->findChild<QComboBox *>("cob_limit_type");
    QComboBox *cob_morning=mW->findChild<QComboBox *>("cob_morning");
    QComboBox *cob_reset_day=mW->findChild<QComboBox *>("cob_reset_day");
    QLineEdit *edt_reset_hour=mW->findChild<QLineEdit*>("edt_reset_hour");
    QLineEdit *edt_reset_minute=mW->findChild<QLineEdit*>("edt_reset_minute");
    QLineEdit *edt_month_limit=mW->findChild<QLineEdit*>("edt_month_limit");
    QGenieRCWirelessObject *rcwo=QGenieRCWirelessObject::GetSingletonPtr();

    Q_ASSERT(cob_limit_type && cob_morning&&cob_reset_day
             &&edt_reset_hour &&edt_reset_minute
             &&edt_month_limit && rcwo);
    const char *cNull="NULL";
    const char *cLimitType[]={"No Limit","Download only","Both directions"};
//    const char *cResetDay[]={"1","2","3","4","5","6","7","8","9","10","11","12",}

    QStringList soapparas;
    int idx=cob_limit_type->currentIndex();

    if((idx>=0)&&(idx<(sizeof(cLimitType)/sizeof(cLimitType[0]))) )
    {
        soapparas<<cLimitType[idx];
    }
    else
    {
        soapparas<<cNull;
    }

    soapparas<<edt_month_limit->text();
    int itemp=edt_reset_hour->text().toInt();
    itemp+=(cob_morning->currentIndex()>0)?12:0;
    soapparas<<QString::number(itemp);
    itemp=edt_reset_minute->text().toInt()%60;
    soapparas<<QString::number(itemp);
    idx=cob_reset_day->currentIndex();
	if (idx == (cob_reset_day->count() - 1))
	{
		QDate cur = QDate::currentDate();
		soapparas<<QString::number(cur.daysInMonth());
	}
	else
	{
		soapparas<<QString::number(idx+1);
	}
    

    if(IS_REMOTE_UI())
    {
        soapparas.insert(0,rcwo->getCpName());
        QGenieIface2ShareObject::sSingleInstance->mNetgearInfoM
                ->netgearCommand(INetgearInterface::IC_SM_SET_TM_OPTION,soapparas,NULL,NULL);

    }
    else
    {
        QGenieIface2ShareObject::sSingleInstance->mNetgearInfoM
                ->netgearCommand(INetgearInterface::IC_SET_TM_OPTION,soapparas,NULL,NULL);
    }

    int t=IS_REMOTE_UI()?SMARTNETWORK_WAIT_TIME:40;
    QGenieTimelineLocker *locker=QGenieTimelineLocker::GetSingletonPtr();
    Q_ASSERT(locker);
    locker->startTimer(t*1000,this,SLOT(slot_RefreshData())/*,mPi*/);

}

void QGenieTrafficMeterObject::setError(int lanidx)
{
    mErrorLan=lanidx;
    if(mErrorLabel)
    {
        mErrorLabel->setText((mErrorLan<0)?"":QString(ERROR_MODE).arg(GENIE2_GET_TEXT(mErrorLan)));
        //mErrorLabel->setVisible(mErrorLan>=0);
    }
}



////////////////////////////////////////////////////////////
//        NAME: deployIndicators()
// DESCRIPTION: .
//   ARGUMENTS: QWidget *w
//     RETURNS: void
//      AUTHOR: Chensi
//        DATA: 2012-3-7
////////////////////////////////////////////////////////////
void QGenieTrafficMeterObject::deployIndicators(QWidget *w)
{
	Q_ASSERT(w);
    const struct _{const char *name;QPiObject**p;}
    cParas[]={
        {"fr_pi",&mPi}
    };
    for(int i=0;i<sizeof(cParas)/sizeof(cParas[0]);i++)
    {
        Q_ASSERT(*(cParas[i].p)==NULL);
        *(cParas[i].p)=new QPiObject(w,cParas[i].name);
    }
}
