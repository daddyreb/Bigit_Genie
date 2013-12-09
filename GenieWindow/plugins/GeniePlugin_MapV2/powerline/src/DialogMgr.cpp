#include "DialogMgr.h"
#include "PowerlineHandler.h"
#include "PowerlinePrompt.h"
#include "PowerlineSecurity.h"
#include "Powerline_mapuitilities.h"
#include "transactions.h"
#include "tmpDefine.h"
#include "Powerline_RateShowDlg.h"
#include "PowerlineTimeLineBox.h"

#include <QDesktopServices>



DialogMgr::DialogMgr(QObject *parent) :
    QObject(parent)
  ,m_widget(NULL)
  ,m_power(NULL)
  ,m_diaPrompt(NULL)
  ,m_dlgDevName(NULL)
  ,m_wirelessDlg(NULL)
  ,m_bFirstStart(false)
  ,m_timelineBox(NULL)
  //,m_msgBox(NULL)
  //,m_dlgDevList(NULL)
  //,m_powerSec(NULL)
{
#ifdef ENABLE_SMARTWIFIWIZARD
    m_wizardParentWidget = 0;
    m_wifiWizard = 0;
    m_searchRouterTag = 0;
#endif

    init();
}

#ifdef ENABLE_SMARTWIFIWIZARD
void DialogMgr::setWiFiWizardParentWidget(QWidget *parent)
{
    m_wizardParentWidget = parent;
}

void DialogMgr::verfyWizard()
{
    if(0 == m_wifiWizard){
        m_wifiWizard = new SmartWirelessSettingWizard(/*m_wizardParentWidget*/
                                              #ifndef TEST_PL_CODE
                                                      QGenieIface2ShareObject::sSingleInstance->mMainFrame
                                              #endif
                                                      );
        connect(m_wifiWizard, SIGNAL(requestSearchRouter()), SLOT(OnSearchRouter()));
        connect(m_wifiWizard, SIGNAL(requestApplySetting(QString,const WLANInfo*)), SLOT(OnRequestApplySetting(QString,const WLANInfo*)));
        connect(m_wifiWizard, SIGNAL(cancelWizard()), SLOT(OnWizardCancel()));
    }
}

void DialogMgr::OnSearchRouter()
{
    if(++m_searchRouterTag == 0){
        m_searchRouterTag = 1;
    }

     if(!m_plHandler.getDeviceWLANInfo("www.routerlogin.net", m_searchRouterTag)){
        m_wifiWizard->notifySearchRouterResult(false,0);
        //showMsgBox("Prompt", "NETGEAR router not founded!");
    }
}

void DialogMgr::OnRequestApplySetting(const QString &ip, const WLANInfo *wlanInfo)
{
    m_wifiWizard->end(true);

    bool bRet = m_plHandler.setDeviceWLANInfo(ip, *wlanInfo, false);
    emit operRet(OR_WIRELESS_SET, bRet);
}

void DialogMgr::OnWizardCancel()
{
    m_wifiWizard->end();
}

#endif

DialogMgr::~DialogMgr()
{
    qDebug() << "DialogMgr exit";

    POWERLINE_SAFEDEL_LATER(m_wirelessDlg);
    POWERLINE_SAFEDEL_LATER(m_power);
    POWERLINE_SAFEDEL_LATER(m_dlgDevName);
    POWERLINE_SAFEDEL_LATER(m_diaPrompt);

#ifdef ENABLE_SMARTWIFIWIZARD
    m_wifiWizard->end();
    delete m_wifiWizard;
#endif
}

//=================================================
//  Initialization funtion
//=================================================
void DialogMgr::init()
{
    //   connect(&m_power, SIGNAL(signApply(const QString &, const QList<QPair<QString, QString> >&)), this, SLOT(onSecurity(const QString &, const QList<QPair<QString, QString> >&)));
    //   connect(&m_diaPrompt, SIGNAL(btnOK()), this, SLOT(onBackFactory()));
    //   connect(&m_dlgDevName, SIGNAL(signPDNok(QString)), this, SLOT(onDevName(QString)));
    //connect(&m_wirelessDlg, SIGNAL(signalApply(WLANInfo &)), this, SLOT(onWirelessSet(WLANInfo &)));

    connect(&m_plHandler, SIGNAL(OnLCDScanned(TopologyDataTransfer)), this, SLOT(slot_Scanned(TopologyDataTransfer)));
    connect(&m_plHandler, SIGNAL(OnTransFinished(TransactionPointer)), this, SLOT(slot_Finished(TransactionPointer)));

}
//=================================================
//  slot funtion
//=================================================
void DialogMgr::onBackFactory()
{

    POWERLINE_SAFEDEL_LATER(m_diaPrompt);

    bool bRet = false;

    if(isValidValue(m_strMac))
    {
        bRet = m_plHandler.backToFactory(m_strMac);
    }
    emit operRet(OR_BACKTO_FACTORY, bRet);
}
void DialogMgr::onDevName(QString str)
{
    POWERLINE_SAFEDEL_LATER(m_dlgDevName);

    if(str.isEmpty())
    {
        return;
    }

    bool bRet = false;
    if(isValidValue(m_strMac))
    {
        bRet = m_plHandler.modifyDevName(m_strMac, str);
    }

    emit operRet(OR_MODIFY_NAME, bRet);
}
void DialogMgr::onWirelessSet(WLANInfo &wlanInfo)
{
    POWERLINE_SAFEDEL_LATER(m_wirelessDlg);

    bool bRet = false;
    if(isValidValue(m_strIp))
    {
        bRet = m_plHandler.setDeviceWLANInfo(m_strIp, wlanInfo);
    }

    emit operRet(OR_WIRELESS_SET, bRet);
}
void DialogMgr::onSecurity(const QString &key, const QList<QPair<QString, QString> >&lst)
{
    bool bRet = false;

    POWERLINE_SAFEDEL_LATER(m_power);

    QStringList strLstMac;
    QStringList strLstPwd;

    //if(lst.count() == 1)
    //{
    //    bRet = m_plHandler.applySecurity(lst[0].first, key, lst[0].second);
    //}
    //else
    {
        for(int i = 0; i < lst.count(); ++i)
        {
            strLstMac << lst[i].first;
            strLstPwd << lst[i].second;

			
        }
		qDebug() << strLstMac << ":" << strLstPwd;
        bRet = m_plHandler.applySecurityAll(strLstMac, key, strLstPwd);
    }

    emit operRet(OR_MODIFY_KEY, bRet);
}
void DialogMgr::onTurnLed(int nState)
{
    Q_UNUSED(nState)
}

QMap<QString, QString> DialogMgr::getRetSecInfo() const
{
	return m_mapSecKeyInfo;
}
void DialogMgr::slot_timelineover(int nRet)
{
	if(m_timelineBox)
	{
		POWERLINE_SAFEDEL(m_timelineBox);
	}
	emit timeLineDlgOver(nRet);
}
void DialogMgr::slot_Finished(TransactionPointer stTmp)
{
    int nId = stTmp.get()->Transaction();
    int nState = stTmp.get()->State();

    switch(nId)
    {
	case PLCTransaction::TC_SET_DEVICE_SECURITY_KEY:
	case PLCTransaction::TC_BATCH_SET_DEVICE_SECURITY_KEY:
		{
//			if(nState == PLCTransaction::TS_SUCCESS)
//			{

//			}

            PLC_TBatchSetDevicesSecurityKey *pSecKey = dynamic_cast<PLC_TBatchSetDevicesSecurityKey *>(stTmp.get());
            Q_ASSERT(pSecKey);
            m_mapSecKeyInfo = pSecKey->GetPasswordOkDeviceDetails();
		}
    case PLCTransaction::TC_DEVICE_RESET_DEFAULT:
    case PLCTransaction::TC_SET_DEVICE_LED:
    case PLCTransaction::TC_SET_DEVICE_NAME:
    case PLCTransaction::TC_SET_DEVICE_QOS:
    case PLCTransaction::TC_SET_DEVICE_WLAN_INFO:
        if(nState == PLCTransaction::TS_SUCCESS)
        {
            emit commitOper(OT_SUCCESS);
        }
        else if(nState == PLCTransaction::TS_PENDING)
        {
            emit commitOper(OT_PENDING);
        }
        else
        {
            emit commitOper(OT_FAILED);
        }
        break;

    case PLCTransaction::TC_CHECK_FIRMWARE_UPGRADE:
        if(nState == PLCTransaction::TS_SUCCESS)
        {
            PLC_TCheckFirmwareUpgrade *pChkUpdate = dynamic_cast<PLC_TCheckFirmwareUpgrade *>(stTmp.get());
            QString strUrl;

            bool bRet = pChkUpdate->GetUpgradeUrl(strUrl);
            if(bRet)
            {
                emit commitOper(OT_UPDATE_SUCCESS);
                openUrl(strUrl);
            }
            else
            {
                emit commitOper(OT_UPDATE_NOUP);
            }
        }
        else
        {
            emit commitOper(OT_UPDATE_FAILED);
        }
        break;
#ifdef ENABLE_SMARTWIFIWIZARD
    case PLCTransaction::TC_GET_DEVICE_WLAN_INFO:
    {
        PLC_TGetDeviceWLANInfo *getwlaninfo = dynamic_cast<PLC_TGetDeviceWLANInfo *>(stTmp.get());
        if(getwlaninfo){
            unsigned long tagNumber = getwlaninfo->GetTagNumber();

            if((tagNumber == m_searchRouterTag) && m_wifiWizard && m_wifiWizard->running()){
                if(nState == PLCTransaction::TS_SUCCESS){
                    WLANInfo wlanInfo;
                    getwlaninfo->GetWLANInfo(wlanInfo);

                    m_wifiWizard->notifySearchRouterResult(true, &wlanInfo);
                }else{
                    m_wifiWizard->notifySearchRouterResult(false, 0);
                    //showMsgBox("Prompt", "NETGEAR router not founded!");
                }
            }
        }

    }

        break;
#endif

    default:
        break;
    }
}
void DialogMgr::slot_Scanned(TopologyDataTransfer stTmp)
{
    emit onNewData(stTmp);

    m_bFirstStart = true;
}
//=================================================
//  public interface funtion
//=================================================
int DialogMgr::showFactorySetTip(const QString &title, const QString &context, const QString &imgPath)
{
    POWERLINE_SAFEDEL_LATER(m_diaPrompt);
    m_diaPrompt = new PowerlinePrompt(
            #ifndef TEST_PL_CODE
                QGenieIface2ShareObject::sSingleInstance->mMainFrame
            #endif
                );

    connect(m_diaPrompt, SIGNAL(btnOK()), this, SLOT(onBackFactory()));


    QWidget *w;

    m_diaPrompt->setTitle(title);
    m_diaPrompt->setContext(context);
    m_diaPrompt->setPic(imgPath);
    w = static_cast<QWidget *>(m_diaPrompt);
    setFramelessStyle(w);

    positionCenter(w);

    return m_diaPrompt->exec();

    //w->show();
}
int DialogMgr::showWirelessSet(const QString &model, WLANInfo &data)
{
	Q_UNUSED(model)
#ifndef ENABLE_SMARTWIFIWIZARD

    POWERLINE_SAFEDEL_LATER(m_wirelessDlg);
    m_wirelessDlg = new PowerlineWirelessSet(
            #ifndef TEST_PL_CODE
                QGenieIface2ShareObject::sSingleInstance->mMainFrame
            #endif
                );

    connect(m_wirelessDlg, SIGNAL(signalApply(WLANInfo &)), this, SLOT(onWirelessSet(WLANInfo &)));


    QWidget *w;

    w = static_cast<QWidget *>(m_wirelessDlg);
    setFramelessStyle(w);
    m_wirelessDlg->initializeValue(data);

    positionCenter(w);

    return m_wirelessDlg->exec();
    //w->show();
#else
    verfyWizard();

    positionCenter(m_wifiWizard);

    if(++m_searchRouterTag == 0){
        m_searchRouterTag = 1;
    }

    return m_wifiWizard->begin(model, m_strIp, data);
#endif

}

int DialogMgr::showApplyDialog(QList<PLDevice *> &devList,QDialog *dlg /* = NULL */)
{
    POWERLINE_SAFEDEL_LATER(m_power);
    m_power = new PowerlineSecurity(
            #ifndef TEST_PL_CODE
                QGenieIface2ShareObject::sSingleInstance->mMainFrame
            #endif
                );

    connect(m_power, SIGNAL(signApply(const QString &, const QList<QPair<QString, QString> >&)), this, SLOT(onSecurity(const QString &, const QList<QPair<QString, QString> >&)));

    Q_UNUSED(dlg);
    QWidget *w;

    w = static_cast<QWidget *>(m_power);
    setFramelessStyle(w);
    m_power->initializeValue(devList);

    positionCenter(w);
    return m_power->exec();

    //w->show();
}
//void DialogMgr::showApplyDlgSec(QList<QStringList> strList)
//{
//	POWERLINE_SAFEDELETE(m_powerSec);
//	m_powerSec = new PowerlineSecurity_PWD;
//
//
//	QWidget *w;
//
//	w = static_cast<QWidget *>(&powerSec);
//	setFramelessStyle(w);
//	powerSec.initializedTable(strList);
//
//	positionCenter(w);
//
//	powerSec.exec();
//	//w->show();
//}
int DialogMgr::showDevName(const QString &name, const QString &type)
{
    POWERLINE_SAFEDEL_LATER(m_dlgDevName);
    m_dlgDevName = new PowerlineDeviceName(
            #ifndef TEST_PL_CODE
                QGenieIface2ShareObject::sSingleInstance->mMainFrame
            #endif
                );

    connect(m_dlgDevName, SIGNAL(signPDNok(QString)), this, SLOT(onDevName(QString)));


    QWidget *w;

    m_dlgDevName->InitializedData(name, type);
    w = static_cast<QWidget *>(m_dlgDevName);
    setFramelessStyle(w);

    positionCenter(w);

    return m_dlgDevName->exec();
    //w->show();
}
bool DialogMgr::setOperMac(const QString &addr)
{
    /*Q_ASSERT_X(!addr.isEmpty(), "DialogMgr", "setOperMac Mac addr empty!");*/
    if(addr.isEmpty())
    {
        return false;
    }

    m_strMac.clear();
    m_strMac = addr;

    return true;
}
void DialogMgr::setOperIp(const QString &addr)
{
    m_strIp.clear();
    m_strIp = addr;
}
void DialogMgr::ModifyDevName(const QString &name)
{
    bool bRet = false;

    if(isValidValue(m_strMac))
    {
        bRet = m_plHandler.modifyDevName(m_strMac, name);
    }

    emit operRet(OR_MODIFY_NAME, bRet);
}
void DialogMgr::TurnLedState(unsigned int nState)
{
    bool bRet = false;

    if(isValidValue(m_strMac))
    {
        //nState = (nState != 0) ? 0 : 1;
        bRet = m_plHandler.turnLedOff(m_strMac, nState);
    }

    emit operRet(OR_TURN_LED, bRet);

}
void DialogMgr::chkUpdate()
{
    bool bRet = false;
    if(isValidValue(m_strMac))
    {
        //nState = (nState != 0) ? 0 : 1;
        bRet = m_plHandler.chkUpdate(m_strMac);
    }

    emit operRet(OR_CHKUPDATE, bRet);
}

int DialogMgr::showMsgBox(const QString &title, const QString &content, int nType /* = PL_MB_OK */)
{
    Powerline_MessageBox msgBox
        #ifndef TEST_PL_CODE
            (QGenieIface2ShareObject::sSingleInstance->mMainFrame)
        #endif
            ;
    QWidget *w;

    w = static_cast<QWidget *>(&msgBox);
    setFramelessStyle(w);

    msgBox.showDlg(title, content, nType);

    positionCenter(&msgBox);

    return msgBox.exec();
}
void DialogMgr::showTimeLineBox(int titleID, int contentID, unsigned int timeLine, QWidget *parent /* = NULL */)
{
	//Powerline_MessageBox msgBox(parent);
//#ifndef TEST_PL_CODE
//		(QGenieIface2ShareObject::sSingleInstance->mMainFrame)
//#endif
		//;
	if(!m_timelineBox)
	{
		m_timelineBox = new PowerlineTimeLineBox(parent);
		
		connect(m_timelineBox, SIGNAL(dlgOver(int)), this, SLOT(slot_timelineover(int)));
	}
	QWidget *w;

	w = static_cast<QWidget *>(m_timelineBox);
	setFramelessStyle(w, false);

	m_timelineBox->showTimeLineDlg(titleID, contentID, timeLine);

	positionCenter(m_timelineBox, parent);

	m_timelineBox->show();
}
int DialogMgr::showMsgBox(const QString &title, QString &content, const QString &strChkBox, int nType /* = PL_MB_OK */)
{
    Powerline_MessageBox msgBox
        #ifndef TEST_PL_CODE
            (QGenieIface2ShareObject::sSingleInstance->mMainFrame)
        #endif
            ;
    QWidget *w;
    m_chkboxState = -1;

    w = static_cast<QWidget *>(&msgBox);
    setFramelessStyle(w);

    msgBox.openChkBox(strChkBox);
    msgBox.showDlg(title, content, nType);


    positionCenter(&msgBox);

    msgBox.exec();
    m_chkboxState = msgBox.retChkboxState();

    return m_chkboxState;
}
int DialogMgr::getChkboxState() const
{
    return m_chkboxState;
}
bool DialogMgr::refreshScanDev()
{
    return m_plHandler.refreshScanDev();
}

void DialogMgr::showDlgRate(PowerlineNode *topNode, PowerlineEdge *topEdge, const QList<PLDevice *> &lstPL)
{
	Powerline_RateShowDlg dlgRate
#ifndef TEST_PL_CODE
	(QGenieIface2ShareObject::sSingleInstance->mMainFrame)
#endif
	;

	QWidget *w = NULL;
	w = static_cast<QWidget *>(&dlgRate);
	setFramelessStyle(w);

	dlgRate.initializeTopNode(topNode, topEdge);
	dlgRate.initializeValue(lstPL);

	positionCenter(&dlgRate);

	dlgRate.exec();
}
void DialogMgr::showDlgDevLst(PowerlineNode *topNode, PowerlineEdge *topEdge, const QList<QPair<QString,QString> > &lstDev)
{
    Powerline_DetailDlg dlgDevList
        #ifndef TEST_PL_CODE
            (QGenieIface2ShareObject::sSingleInstance->mMainFrame)
        #endif
            ;

    QWidget *w = NULL;
    w = static_cast<QWidget *>(&dlgDevList);
    setFramelessStyle(w);

    dlgDevList.initializeTopNode(topNode, topEdge);
    dlgDevList.initializeValue(lstDev);

    positionCenter(&dlgDevList);

    dlgDevList.exec();
}
void DialogMgr::showDlgDevLst(PowerlineNode *topNode, PowerlineEdge *topEdge, const QList<AttachDeviceEntry> &lstDev)
{
    Powerline_DetailDlg dlgDevList
        #ifndef TEST_PL_CODE
            (QGenieIface2ShareObject::sSingleInstance->mMainFrame)
        #endif
            ;
    QWidget *w;

    w = static_cast<QWidget *>(&dlgDevList);
    setFramelessStyle(w);

    dlgDevList.initializeTopNode(topNode, topEdge);
    dlgDevList.initializeValue(lstDev);

    positionCenter(&dlgDevList);

    dlgDevList.exec();
}


void DialogMgr::setDlgParent(QWidget *w)
{
    Q_ASSERT(w && "DialogMgr::setDlgparent");

    m_widget = w;
}
//=================================================
//  other funtion
//=================================================
bool DialogMgr::isValidValue(const QString &str)
{
    return (!str.isEmpty());
}

void DialogMgr::openUrl(const QString &strUrl)
{
    QDesktopServices::openUrl(strUrl);

}
void DialogMgr::positionCenter(QWidget *w, QWidget *parentW)
{
    //QDialog *dlg=qobject_cast<QDialog *>(w);
    //Q_ASSERT(m_widget && "DialogMgr::positionCenter");

    //int xPPos = 0, yPPos = 0;
    //xPPos = m_widget->pos().x();
    //yPPos = m_widget->pos().y();

    //int nPCenterX = 0, nPCenterY = 0;
    //nPCenterX = m_widget->width() / 2;
    //nPCenterY = m_widget->height() / 2;

    //int nSCenterX = 0, nSCenterY = 0;
    //nSCenterX = w->width() / 2;
    //nSCenterY = w->height() / 2;

    //int xPos = 0, yPos = 0;
    //xPos = nPCenterX - nSCenterX;
    //yPos = nPCenterY - nSCenterY;

    ////xPos += xPPos;
    ////yPos += yPPos;

    //w->move(xPos, yPos);

	w->show();

    QWidget *tmpW = NULL;
    if(parentW != NULL)
    {
        Q_ASSERT(parentW && w);
        tmpW = parentW;

		w->move((parentW->width() - w->width()) / 2, (parentW->height() - w->height()) / 2);
    }
    else
    {
        Q_ASSERT(m_widget && w);
        tmpW = m_widget;


		QPoint ref_pt = tmpW->mapToGlobal(tmpW->pos());

		QPoint target_pt = w->mapToGlobal(w->pos());

		target_pt.rx() = ref_pt.x() + (tmpW->width() - w->width()) / 2;
		target_pt.ry() = ref_pt.y() + (tmpW->height() - w->height()) / 2;

		w->move(target_pt);
    }

    //	w->setParent(tmpW);

}

