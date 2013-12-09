#include "QGenieRebootObject.h"
#include <QUiLoader>
#include "genie2_interface.h"
#include "genie2_netgear.h"
#include "language_RouterConfig.h"
#include "QGenieRCWirelessObject.h"
#include "QGenieTimelineLocker.h"
extern quint64 sRCFlags;
QGenieRebootObject::QGenieRebootObject(QObject* parent)
    :QObject(parent)
    ,mW(NULL)
	,mSession(-1)
{
}

QGenieRebootObject::~QGenieRebootObject()
{

}

QWidget* QGenieRebootObject::getRebootUi()
{
    if(!mW)
    {
        bool btemp;
        QUiLoader loader;
        QFile f(GENIE2_RES("ui/reboot.ui"));
        btemp = f.open(QFile::ReadOnly);
        Q_ASSERT(btemp);
        QWidget* w = loader.load(&f,QGenieIface2ShareObject::sSingleInstance->mMainFrame);
        Q_ASSERT(w);
        w->hide();
        mW = w;
		QList<QPushButton *> bts=w->findChildren<QPushButton *>();
		int c=bts.count();
		for(int i=0;i<c;i++)
		{
			connect(bts[i],SIGNAL(clicked()),this,SLOT(slot_buttonClicked()));
		}
    }
    return mW;
}
void QGenieRebootObject::slot_buttonClicked()
{
	QPushButton *but = qobject_cast<QPushButton *>(sender());
	QString objName = but->objectName();
	if(objName == "btn_Reboot")
	{
		QStringList dialog_lans;
		dialog_lans<<GENIE2_GET_TEXT(L_OTHERS_REBOOT)<<GENIE2_GET_TEXT(L_OTHERS_MSG_NOTIFY);
		sRCFlags |= RCF_MODAL_DIALOG_ON;
		bool dialogOk=GENIE2_UI_COMMAND(UICMD_DIALOG_MODAL,dialog_lans).toBool();
		sRCFlags &=~RCF_MODAL_DIALOG_ON;
		if(!dialogOk)
		{
			return;
		}

		mSession = QGenieIface2ShareObject::sSingleInstance->mNetgearInfoM
			->netgearCommand(INetgearInterface::IC_REBOOT,QStringList(),this,NULL);
		QGenieTimelineLocker *locker = QGenieTimelineLocker::GetSingletonPtr();
		Q_ASSERT(locker);
		locker->setNeedSingleFinish(true);
		locker->setNeedReconnect();
		locker->startTimer(90*1000,this,SLOT(slot_RebootFinish()),1);
	}
	if(objName == "btn_Documentation")
	{
		QUrl qurl("http://downloadcenter.netgear.com/");
		QDesktopServices::openUrl(qurl);
	}
}

void QGenieRebootObject::slot_RebootFinish()
{
	QGenieRCWirelessObject *rcwo=QGenieRCWirelessObject::GetSingletonPtr();
	Q_ASSERT(rcwo);
	//rcwo->updateUi();
	rcwo->mWirelessW->setEnabled(true);
	sRCFlags &= ~RCF_LOCAL_LOGEDIN;
	sRCFlags &= ~RCF_REMOTE_LOGEDIN;
	sRCFlags &= ~RCF_IN_REMOTE_LOGIN_PROCESS;
}

void QGenieRebootObject::retranslateUi()
{
	if(!mW)
	{
		return;
	}
	//0.label   1.button    2.checkbox  3.radiobutton
	const struct _{const char *name;int lan;int type;}
	cPara[]={
		{"btn_Reboot",L_OTHERS_REBOOT,1},
		{"btn_Documentation",L_OHTERS_DOCUMENTATION,1}
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
}