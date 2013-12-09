
#include "PowerlineNode.h"
#include "PowerlineView.h"

#include <QtDebug>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QCheckBox>
#include <QEvent>
#include <QComboBox>
#include <QShowEvent>
#include <QIcon>
#include <QPixmap>
#include <QStackedLayout>
#include <QApplication>
#include <QMenu>
#include <QDesktopServices>
#include <QShortcut>
#include <QMessageBox>


#include "PowerlineMapView.h"
#include "PLDevice.h"
#include "PowerlineDefine.h"
#include "tmpDefine.h"
#include "Powerline_mapuitilities.h"
#include "DialogMgr.h"
#include "waitpanel.h"
#include "Powerline_HubMapView.h"

#include "Powerline_DetailDlg.h"
#include "PowerlineTimeLineBox.h"


#define G_IMGPATH ":/PowerLineImage/info.png"

//QList<Device *>gdev;
PLCTopologyData g_dev;

PowerlineMapView::PowerlineMapView(QWidget *parent)
    :QWidget(parent)
    ,m_topologyview(0)
    //,m_prevPageBtnState(BTNSTATE_NORMAL)
    //,m_nextPageBtnState(BTNSTATE_NORMAL)
    //,m_firstPageBtnState(BTNSTATE_NORMAL)
    //,m_lastPageBtnState(BTNSTATE_NORMAL)
    ,m_layOut(0)
    ,m_devTotalCount(0)
    ,m_pageTotalCount(0)
    ,m_currentPageIndex(1)
    ,m_bUpdateUi(false)
    ,m_nOperRetCode(-1)
    ,m_nWhosOper(-1)
    ,m_nShowState(SS_HIDE)
    ,m_TipShowState(SDW_NO_SHOW)
    ,m_WirelessShowState(SDW_NO_SHOW)
	,m_hubView(NULL)
	,m_isRouterOk(false)
	,m_bAnimateStop(false)
	,m_bHaveIp(false)

{

#ifdef ENABLE_SMARTWIFIWIZARD
    m_dlgMgr.setWiFiWizardParentWidget(this);
#endif

#ifdef TEST_PL_CODE
	m_isHubView = false;
#endif
    m_topologyview = new PowerlineView(this);
    m_topologyview->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

    m_waittingPanel = new WaitPanel(this);
    m_waittingPanel->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

	m_hubView	= new Powerline_HubMapView(this);
	m_hubView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    m_topWaittingLayout = new QStackedLayout();
    m_topWaittingLayout->addWidget(m_topologyview);
    m_topWaittingLayout->addWidget(m_waittingPanel);
	m_topWaittingLayout->addWidget(m_hubView);
	connect(m_hubView, SIGNAL(pageTotal(int)), this, SIGNAL(pageChange(int)));


    m_devNumberLabel = new QLabel(this);
    //    m_netcardPanel = new NetcardsPanel(this);
    //    m_notifyNewDevCB = new QCheckBox(this);


    m_devNumberLabel->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred);
    //    m_notifyNewDevCB->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred);
    //    m_notifyNewDevCB->setFocusPolicy(Qt::NoFocus);

    m_refreshBtn = new QPushButton(this);
    m_refreshBtn->setText("Refresh");
#ifndef TEST_PL_CODE
    m_refreshBtn->setVisible(false);
#else
    m_refreshBtn->setVisible(true);
#endif
    connect(m_refreshBtn, SIGNAL(clicked()), this, SLOT(slot_btn_refresh()));



    QHBoxLayout *horLayout = new QHBoxLayout();
    horLayout->addWidget(m_devNumberLabel);
#ifdef TEST_PL_CODE
    horLayout->addWidget(m_refreshBtn);
#endif


	//////左右箭头
	m_prevPageBtn = new QPushButton(this);
	m_nextPageBtn = new QPushButton(this);
	m_prevPageBtn->setObjectName("prevPageBtn");
	m_nextPageBtn->setObjectName("nextPageBtn");
	m_prevPageBtn->setAttribute(Qt::WA_TranslucentBackground);
	m_nextPageBtn->setAttribute(Qt::WA_TranslucentBackground);
	m_prevPageBtn->setVisible(false);
	m_nextPageBtn->setVisible(false);
	connect(m_prevPageBtn, SIGNAL(clicked()), this, SLOT(btnClicked()));
	connect(m_nextPageBtn, SIGNAL(clicked()), this, SLOT(btnClicked()));
	QHBoxLayout *horLayout2 = new QHBoxLayout();
	horLayout2->addWidget(m_prevPageBtn);
	horLayout2->addLayout(m_topWaittingLayout);
	horLayout2->addWidget(m_nextPageBtn);

	QFile f(/*GENIE2_RES(*/":styles/powerlineview.qss")/*)*/;
	bool btemp=f.open(QIODevice::ReadOnly);
	Q_ASSERT(btemp);
	QString stemp;
	stemp=f.readAll();
	f.close();


	//m_prevPageBtn->setStyleSheet(stemp);
	//m_nextPageBtn->setStyleSheet(stemp);

	m_prevPageBtn->setStyleSheet(stemp);
	m_nextPageBtn->setStyleSheet(stemp);
	//////end
    m_layOut = new QVBoxLayout(this);
#ifdef TEST_PL_CODE
    m_layOut->addLayout(horLayout);
#endif

    m_layOut->addLayout(horLayout2);

    updateUiElements();


    //setLayout(m_layOut);

    //    QHBoxLayout *layout = new QHBoxLayout();
    //    layout->addWidget(m_topologyview);
    //    setLayout(layout);


    /**********END******************/
    connect(m_topologyview, SIGNAL(nodeClicked(int)), this, SLOT(onNodeClicked(int)));
    connect(m_topologyview, SIGNAL(nodeContextMenu(int ,QPoint)), this, SLOT(onNodeContextMenu(int, QPoint)));

    //connect(m_waittingPanel, SIGNAL(translateText(int,QString*)), this, SIGNAL(translateText(int, QString*)));

    connect(&m_dlgMgr, SIGNAL(onNewData(TopologyDataTransfer)), this, SLOT(slot_onNewData(TopologyDataTransfer)));
    connect(&m_dlgMgr, SIGNAL(commitOper(int)), this, SLOT(slot_dataFinish(int)));
    connect(&m_dlgMgr, SIGNAL(operRet(int,bool)), this, SLOT(slot_operRet(int,bool)));
	connect(&m_dlgMgr, SIGNAL(timeLineDlgOver(int)), this, SLOT(slot_timelineDlgOver(int)));

    //connect(m_topologyview, SIGNAL(btnClicked(int)), this, SLOT(slot_Btn_Click(int)));




    m_dlgMgr.setDlgParent(this);



    //slot_btn_refresh();
}

PowerlineMapView::~PowerlineMapView(void)
{
    qDebug() << "PowerlineMapView exit()";
}

void PowerlineMapView::showHubView(PowerlineNode *node, PowerlineEdge *edge, const QList<QPair<QString,QString> > &lstUsbDev)
{

	//show hub view
	m_hubView->initializeTopNode(node, edge);
	m_hubView->initializeValue(lstUsbDev);

	m_nextPageBtn->setVisible(false);
	m_prevPageBtn->setVisible(false);
	m_topWaittingLayout->setCurrentWidget(m_hubView);

#ifdef TEST_PL_CODE
	m_isHubView = true;
#endif
}

bool PowerlineMapView::isRouterOk() const
{
	return m_isRouterOk;
}
void PowerlineMapView::hideHubView()
{
	//no show hub view
	m_hubView->delAllData();
	//m_topWaittingLayout->setCurrentWidget(m_topologyview);

	updateUiElements();
#ifdef TEST_PL_CODE
	m_isHubView = false;
#endif
}

void PowerlineMapView::pageBtnClick(int nIdx)
{
	Q_ASSERT(m_hubView);

	m_hubView->pageBtnClick(nIdx);
}
void PowerlineMapView::showSecurity()
{
    if(m_subscribeDevsList.isEmpty())
    {
        return;
    }
	int nRet = m_dlgMgr.showApplyDialog(m_subscribeDevsList);
	if(nRet == 0)
	{
		refresh();
	}
}
void PowerlineMapView::onNodeContextMenu(int idx, const QPoint &pos)
{
    PLDevice *dev = NULL;
    //	emit node2Device(idx, &dev);

    if(m_subscribeDevsList.count() <= idx)
    {
        return;
    }

    dev = m_subscribeDevsList[idx];

    qDebug() << "Network map view context menu idx = " << idx << "pos = " << pos;

    QList<int> items = dev->getDeviceMenuItems();
    //BlockDeviceManager *bdm=BlockDeviceManager::GetSingletonPtr();
	//qDebug() << "Model Name:" << (*dev)[GDP_MODEL_NAME] << "  Items Menu:" << items;
    QString strIp = (*dev)[GDP_IP].toString();
    if(items.size() != 0)
    {
        QMenu menu(this);
        QAction *act = NULL;

        if(items.contains(GDMI_DEVICENAME) && items.contains(GDMI_SECURITY))
        {
            act = menu.addAction(POWERLINE_GET_TEXT(L_POWERLINE_MENU_DEVICE, "Device Name"));
            act->setData(int(GDMI_DEVICENAME));

            if((*dev)[GDP_LED_STATUS].toInt() == 0)
            {
                act = menu.addAction(POWERLINE_GET_TEXT(L_POWERLINE_MENU_LEDON, "Turn LED on"));
            }
            else
            {
                act = menu.addAction(POWERLINE_GET_TEXT(L_POWERLINE_MENU_LEDOFF, "Turn LED off"));
            }
            act->setData(int(GDMI_TURNLED_OFF));

            act = menu.addAction(POWERLINE_GET_TEXT(L_POWERLINE_MENU_SEC, "Security"));
            act->setData(int(GDMI_SECURITY));

            act = menu.addAction(POWERLINE_GET_TEXT(L_POWERLINE_MENU_FACRESET, "FactoryReset"));
            act->setData(int(GDMI_FACTORY_RESET));

			/////hide 
            //act = menu.addAction(POWERLINE_GET_TEXT(L_POWERLINE_MENU_UPGRADE, "Check Update"));
            //act->setData(int(GDMI_UPDATE));
			/////end 

            if((*dev)[GDP_SUPPORT_WIRELESS].toBool() )
            {
                act = menu.addAction(POWERLINE_GET_TEXT(L_POWERLINE_MENU_WIRELESS, "Wireless Configuration"));
                act->setData(int(GDMI_WIRELESS_SET));
            }


            if(!strIp.isEmpty())
            {
                strIp.insert(0, "http://");
                act = menu.addAction(strIp);
                act->setData(int(GDMI_HTTPIP));
            }

        }
		else if(items.contains(GDMI_SECURITY))
		{
			act = menu.addAction(POWERLINE_GET_TEXT(L_POWERLINE_MENU_SEC, "Security"));
			act->setData(int(GDMI_SECURITY));
		}
        act = menu.exec(pos);

        if(act)
        {
            bool bok = false;
            int id = act->data().toInt(&bok);
            if(bok)
            {
                bool bRet = m_dlgMgr.setOperMac((*dev)[GDP_PHYSICALADDR].toString());
                if(!bRet)
                {
                    qDebug() << "PowerlineMapView::onNodeContextMenu mac addr is empty!";
                    return;
                }

                if(id == GDMI_SETUP)
                {
                    QDesktopServices::openUrl(QUrl(QString("http://") + (*dev)[GDP_IP].toString()));
                    emit deviceSetupMenuItemTriggered();
                }
                else if(id == GDMI_VIEWDETAIL)
                {
                    //viewDeviceDetails(dev);
                }

	
				//////ip
				m_bHaveIp = !strIp.isEmpty();

                switch(id)
                {
                    case GDMI_DEVICENAME:
						{
							QString strTitle = POWERLINE_GET_TEXT(L_POWERLINE_DEVNAME_TITLE, "Modify Device Name");
							QString strContent = POWERLINE_GET_TEXT(L_POWERLINE_LED_EXPLAIN, "Connectivity will be temporarily lost if  you continue with this setting Click 'OK' to proceed");

							int nRet = m_dlgMgr.showMsgBox(strTitle, strContent, PL_MB_OK | PL_MB_CANCEL);
							if(nRet != 1)
							{
								refresh();
								return;
							}

							nRet = m_dlgMgr.showDevName((*dev)[GDP_NAME].toString(), (*dev)[GDP_MODEL_NAME].toString());
							if(nRet == 0)
							{
								refresh();
							}
						}                       
						break;
                    case GDMI_TURNLED_OFF:
                    {
                        int nRet = 0;
                        QString strTitle = POWERLINE_GET_TEXT(L_POWERLINE_LED_TITLE, "NETGEAT Powerline Utility");
                        QString strContent = POWERLINE_GET_TEXT(L_POWERLINE_LED_EXPLAIN, "Connectivity will be temporarily lost if \n\n you continue with this setting Click 'OK' to proceedConnectivity will be temporarily lost if \n\n you continue with this setting Click 'OK' to proceed");

                        nRet = m_dlgMgr.showMsgBox(strTitle, strContent, PL_MB_OK | PL_MB_CANCEL);
                        if(nRet == 1)
                        {
                            int nLed = (*dev)[GDP_LED_STATUS].toInt();
                            m_dlgMgr.TurnLedState(!nLed);
                        }
						else
						{
							refresh();
						}
                    }
                        break;
                    case GDMI_SECURITY:
						{
							int nRet = m_dlgMgr.showApplyDialog(m_subscribeDevsList);
							if(nRet == 0)
							{
								refresh();
							}
						}
                        break;
                    case GDMI_FACTORY_RESET:
                    {
                        QString strTitle = POWERLINE_GET_TEXT(L_POWERLINE_FRESET_TITLE, "NETGEAT Powerline Utility");
                        QString strContent = POWERLINE_GET_TEXT(L_POWERLINE_FRESET_EXPLAIN, "Connectivity will be temporarily lost if  you continue with this setting Click 'OK' to proceed");

                        int nRet = m_dlgMgr.showFactorySetTip(strTitle, strContent, QString(G_IMGPATH));
						if(QDialog::Rejected == nRet)
						{
							refresh();
						}
                    }
                        break;
                    case GDMI_WIRELESS_SET:
                    {
                        m_dlgMgr.setOperIp((*dev)[GDP_IP].toString());
                        int ret = m_dlgMgr.showWirelessSet((*dev)[GDP_MODEL_NAME].toString(), dev->m_wlanInfo);
                        if(QDialog::Rejected == ret){
                            refresh();
                        }
                    }
                        break;
                    case GDMI_UPDATE:
                        m_dlgMgr.chkUpdate();
                        break;
                    case GDMI_HTTPIP:
                        QDesktopServices::openUrl(strIp);
                        break;
                    default:
                        break;
                }

				////////control button
				initControlBtn();
				////////end
            }
        }
    }
}


void PowerlineMapView::loadPLCDevicePassword(const QString &devMac, QString *pwd)
{
    emit queryPLCDevicePasswordSignal(devMac, pwd);
}

void PowerlineMapView::storePLCDevicePassword(const QString &devMac, const QString &pwd)
{
    emit storePLCDevicePasswordSignal(devMac, pwd);
}

void PowerlineMapView::onNodeClicked(int idx)
{
    Q_UNUSED(idx);
    //    PLDevice *dev = NULL;
    //    //emit node2Device(idx,&dev);
    //	if(m_subscribeDevsList.count() <= idx)
    //	{
    //		return;
    //	}
    //
    //	dev = m_subscribeDevsList[idx];
    //    qDebug() << "Network map view node clicked";
    //
    //static Powerline_DetailDlg dlg;
    //dlg.show();

}
void PowerlineMapView::slot_operRet(int oRet, bool status)
{
    //Powerline_MessageBox msgBox;
    QString strInfo = POWERLINE_GET_TEXT(L_POWERLINE_MSG_FAILED, "Failed");;
    QString strTiltle = POWERLINE_GET_TEXT(L_POWERLINE_MSG_TITLE_F, "Warning");

    m_nWhosOper = oRet;
    switch(oRet)
    {
        case DialogMgr::OR_TURN_LED:
            break;
        case DialogMgr::OR_MODIFY_NAME:
            break;
        case DialogMgr::OR_MODIFY_KEY:
            break;
        case DialogMgr::OR_BACKTO_FACTORY:

            break;
        default:
            break;
    }
    if(!status)
    {
        //failed
        m_dlgMgr.showMsgBox(strTiltle, strInfo);
        return;
    }
	else
	{
		////////2012-8-21 add//////////
			//strInfo = strInfo.fromLocal8Bit("本次操作成功!");
		//strTiltle = POWERLINE_GET_TEXT(L_POWERLINE_MSG_TITLE_S, "Information");
		//strInfo = POWERLINE_GET_TEXT(L_POWERLINE_MSG_SUCCESS, "Success");
		//m_dlgMgr.showMsgBox(strTiltle, strInfo);
	}
    m_bUpdateUi = true;
    m_refreshBtn->setDisabled(m_bUpdateUi);
    updateUiElements(true);
}
void PowerlineMapView::slot_dataFinish(int nType)
{
    //Powerline_MessageBox msgBox;
    QString strInfo;
    QString strTitle = POWERLINE_GET_TEXT(L_POWERLINE_FRESET_TITLE, "NETGEAT Powerline Utility");

    //m_beingWaitting = false;
    //updateUiElements();

	

	if(m_nWhosOper == DialogMgr::OR_MODIFY_KEY)
	{
		QMap<QString, QString> mapInfo = m_dlgMgr.getRetSecInfo();

		QMap<QString, QString>::const_iterator iter;
		QString strMac;
		QString strKey;
		for(iter = mapInfo.constBegin(); iter != mapInfo.constEnd(); ++iter)
		{
			strMac	= iter.key();
			strKey	= iter.value();
			if(!isLocalDev(strMac))
			{
				storePLCDevicePassword(strMac, strKey);
			}
		}
	}

	m_nOperRetCode = nType;




    switch(nType)
    {
        case DialogMgr::OT_SUCCESS:
		/*	{
				strTitle = POWERLINE_GET_TEXT(L_POWERLINE_MSG_TITLE_S, "Information");
				switch(m_nWhosOper)
				{
				case DialogMgr::OR_TURN_LED:
					strInfo = POWERLINE_GET_TEXT(L_POWERLINE_MSG_LED_S, "Led Title");
					break;
				case DialogMgr::OR_MODIFY_NAME:
					strInfo = POWERLINE_GET_TEXT(L_POWERLINE_MSG_DEV_S, "Success Dev");
					break;
				case DialogMgr::OR_MODIFY_KEY:
					strInfo = POWERLINE_GET_TEXT(L_POWERLINE_MSG_SEC_S, "Modify Key Success");
					break;
				case DialogMgr::OR_BACKTO_FACTORY:
					strInfo = POWERLINE_GET_TEXT(L_POWERLINE_MSG_FRESET_S, "Factory ok");
					break;
				default:
					strInfo = POWERLINE_GET_TEXT(L_POWERLINE_MSG_SUCCESS, "Success");
					break;
				}

				m_dlgMgr.showMsgBox(strTitle, strInfo);

			}
			break;*/
        case DialogMgr::OT_PENDING:
        case DialogMgr::OT_FAILED:
		/*	{
				strTitle = POWERLINE_GET_TEXT(L_POWERLINE_MSG_TITLE_F, "Warning");
				switch(m_nWhosOper)
				{
				case DialogMgr::OR_TURN_LED:
					strInfo = POWERLINE_GET_TEXT(L_POWERLINE_MSG_LED_F, "LED failed");
					break;
				case DialogMgr::OR_MODIFY_NAME:
					strInfo = POWERLINE_GET_TEXT(L_POWERLINE_MSG_DEV_F, "modify name failed");
					break;
				case DialogMgr::OR_MODIFY_KEY:
					strInfo = POWERLINE_GET_TEXT(L_POWERLINE_MSG_FAILED, "key modify failed");
					break;
				case DialogMgr::OR_BACKTO_FACTORY:
					strInfo = POWERLINE_GET_TEXT(L_POWERLINE_MSG_FRESET_F, "Factory failed");
					break;
				default:
					strInfo = POWERLINE_GET_TEXT(L_POWERLINE_MSG_FAILED, "Failed");
					break;
				}
				m_dlgMgr.showMsgBox(strTitle, strInfo);

				
			}*/

            //m_dlgMgr.refreshScanDev();
			{

				m_TipShowState = SDW_NO_SHOW;
				dealAllData();

				//int nRet = 0;
				//QString strMsg = POWERLINE_GET_TEXT(L_POWERLINE_MSG_TIMELINE, "Please Wait...");
				//if(m_nWhosOper == DialogMgr::OR_BACKTO_FACTORY)
				//{
				//	nRet = m_dlgMgr.showTimeLineBox(strTitle, strMsg, 120, this);
				//	//QTimer::singleShot(120 * 1000, this, SLOT(slot_timer_refresh()));
				//}
				//else
				//{
				//	nRet = m_dlgMgr.showTimeLineBox(strTitle, strMsg, 45, this);
				//	//QTimer::singleShot(45 * 1000, this, SLOT(slot_timer_refresh()));
				//}

				showTimelineDlg();
				
				

			}
            break;
        case DialogMgr::OT_UPDATE_FAILED:
            updateUiElements();
            m_bUpdateUi = false;
            m_refreshBtn->setDisabled(m_bUpdateUi);
            strInfo = POWERLINE_GET_TEXT(L_POWERLINE_MSG_UPGRADE_F, "Upgrade failed!");/*strInfo.fromLocal8Bit("固件检查失败，请重试!");*/
            m_dlgMgr.showMsgBox(strTitle, strInfo);
            break;
        case DialogMgr::OT_UPDATE_SUCCESS:
            updateUiElements();
            m_bUpdateUi = false;
            m_refreshBtn->setDisabled(m_bUpdateUi);
            break;
        case DialogMgr::OT_UPDATE_NOUP:
        {
            updateUiElements();
            m_bUpdateUi = false;
            m_refreshBtn->setDisabled(m_bUpdateUi);



            strInfo = POWERLINE_GET_TEXT(L_POWERLINE_MSG_UPGRADE_S, "ok"); /*strInfo.fromLocal8Bit("当前已经是最新版本!");*/
            m_dlgMgr.showMsgBox(strTitle, strInfo);
        }
            break;
        default:

            /*strInfo = strInfo.fromLocal8Bit("本次操作失敗，請重試!");
   msgBox.information(NULL, "Failed", "The operation failed, please try again!");*/
            break;
    }


	/*updateUiElements(true);*/

}
void PowerlineMapView::showTimelineDlg()
{
	//m_topologyview->setEnabled(false);
	//this->setEnabled(false);

	int nRet = 0;

	if(m_nWhosOper == DialogMgr::OR_BACKTO_FACTORY)
	{
		if(m_bHaveIp)
		{
			m_dlgMgr.showTimeLineBox(L_POWERLINE_MSG_TITLE_S, L_POWERLINE_MSG_TIMELINE, 120, this);
		}
		else
		{
			m_dlgMgr.showTimeLineBox(L_POWERLINE_MSG_TITLE_S, L_POWERLINE_MSG_TIMELINE, 5, this);
		}
		
	}
	else
	{
		if(m_bHaveIp)
		{
			m_dlgMgr.showTimeLineBox(L_POWERLINE_MSG_TITLE_S, L_POWERLINE_MSG_TIMELINE, 45, this);
		}
		else
		{
			m_dlgMgr.showTimeLineBox(L_POWERLINE_MSG_TITLE_S, L_POWERLINE_MSG_TIMELINE, 5, this);
		}
		
	}

	m_nWhosOper		= -1;
}
void PowerlineMapView::slot_timelineDlgOver(int nRet)
{
	Q_UNUSED(nRet)
	//this->setEnabled(true);

	slot_timer_refresh();
}
void PowerlineMapView::slot_onNewData(TopologyDataTransfer dev)
{

    //m_refreshBtn->setDisabled(m_bUpdateUi);
    ////clear start
    m_dev.clear();

    m_dev			= dev.get()->GetTopologyData();
   // m_devLinkRate	= dev.get()->GetNodeLinkRate2LocalDevice();
	m_isRouterOk	= dev.get()->IsRouterLocated();
	m_strLocalMac	= dev.get()->LocalDeviceMacAddr();

    int idx = 0;
    for(idx = 0 ; idx < m_dev.size() ; ++idx)
    {
        if(idx >= m_subscribeDevsList.size())
        {
            m_subscribeDevsList.append(new PLDevice());
        }


		deviceFromRawData(*(m_subscribeDevsList[idx]), *(m_dev[idx]));
        //deviceFromRawData(*(m_subscribeDevsList[idx]), *(m_dev[idx]), m_devLinkRate[idx]);
    }
    int tmpCount = m_subscribeDevsList.size();
    for(int k = idx ; k < tmpCount ; ++k)
    {
        delete m_subscribeDevsList.takeAt(idx);
    }
    Q_ASSERT(m_subscribeDevsList.size() == m_dev.size() && "Err 239");

    // dealLinkPLCNode(m_dev, m_subscribeDevsList);



    dataDealByPage();
    updateUiElements();

    ////state reset
    m_WirelessShowState = SDW_NO_SHOW;
    /*m_TipShowState = SDW_NO_SHOW;*/
    ////end


    emit refreshOver();

 //   dealAllData();

#ifdef TEST_PL_CODE
	QTimer::singleShot(10000, this, SLOT(notifyInternetState()));
#endif

}

void PowerlineMapView::dealAllData()
{
#ifdef TEST_PL_CODE
    if(isTipShowed()/* || (isShow())*/)
    {
        return;
    }
#else
    if(isTipShowed() || (!isShow()))
    {
        return;
    }
#endif


    if(m_bUpdateUi /*&& m_bAnimateStop*/)
    {
        //Powerline_MessageBox msgBox;
        QString strInfo;
        QString strTitle;


        m_bUpdateUi = false;


        if(m_nOperRetCode == DialogMgr::OT_SUCCESS)
        {
            //strInfo = strInfo.fromLocal8Bit("本次操作成功!");
			strTitle = POWERLINE_GET_TEXT(L_POWERLINE_MSG_TITLE_S, "Information");
			switch(m_nWhosOper)
			{
			case DialogMgr::OR_TURN_LED:
				strInfo = POWERLINE_GET_TEXT(L_POWERLINE_MSG_LED_S, "Led Title");
				break;
			case DialogMgr::OR_MODIFY_NAME:
				strInfo = POWERLINE_GET_TEXT(L_POWERLINE_MSG_DEV_S, "Success Dev");
				break;
			case DialogMgr::OR_MODIFY_KEY:
				strInfo = POWERLINE_GET_TEXT(L_POWERLINE_MSG_SEC_S, "Modify Key Success");
				break;
			case DialogMgr::OR_BACKTO_FACTORY:
				strInfo = POWERLINE_GET_TEXT(L_POWERLINE_MSG_FRESET_S, "Factory ok");
				break;
			default:
				strInfo = POWERLINE_GET_TEXT(L_POWERLINE_MSG_SUCCESS, "Success");
				break;
			}

            m_dlgMgr.showMsgBox(strTitle, strInfo);


        }
        else if(m_nOperRetCode == DialogMgr::OT_FAILED)
        {
            strTitle = POWERLINE_GET_TEXT(L_POWERLINE_MSG_TITLE_F, "Warning");
            switch(m_nWhosOper)
            {
                case DialogMgr::OR_TURN_LED:
                    strInfo = POWERLINE_GET_TEXT(L_POWERLINE_MSG_LED_F, "LED failed");
                    break;
                case DialogMgr::OR_MODIFY_NAME:
                    strInfo = POWERLINE_GET_TEXT(L_POWERLINE_MSG_DEV_F, "modify name failed");
                    break;
                case DialogMgr::OR_MODIFY_KEY:
                    strInfo = POWERLINE_GET_TEXT(L_POWERLINE_SEC_DLG2_ERR, "The Device Password Entered is Invalid.\nPlease check it and try again.");
                    break;
                case DialogMgr::OR_BACKTO_FACTORY:
                    strInfo = POWERLINE_GET_TEXT(L_POWERLINE_MSG_FRESET_F, "Factory failed");
                    break;
                default:
					strInfo = POWERLINE_GET_TEXT(L_POWERLINE_MSG_FAILED, "Failed");
					
                    break;
            }
            /*strInfo = strInfo.fromLocal8Bit("本次操作失敗，請重試!");*/

            m_dlgMgr.showMsgBox(strTitle, strInfo);

        }
        else
        {

        }

        m_nOperRetCode	= -1;


        m_TipShowState = SDW_HAVE_SHOW;
    }
}

void PowerlineMapView::dataDealByPage()
{
    int nTmp = 0;

    m_devTotalCount		= m_subscribeDevsList.count() - 1;
    nTmp = m_devTotalCount / (PER_PAGE_DATACOUNT - 1);
    m_pageTotalCount	= (m_devTotalCount % (PER_PAGE_DATACOUNT - 1) == 0 ) ? nTmp : (nTmp + 1);



    updateUiElements();
    subscribeCurrentPageData();



}
void PowerlineMapView::dealLinkPLCNode(QList<PLCNodeData *> &data, QList<PLDevice *> &dev)
{
    //QQueue<PLCNodeData *> tmpQueue;
    /* PLCNodeData *tmpDev = NULL;*/
    QString strSonIdx, strParentIdx;
    QString strRates;


    int nLen = data.count();
    if(nLen == 0)
    {
        return;
    }

    ////initialized dev m_parent and m_son
    for(int i = 0; i < nLen; ++i)
    {
        dev[i]->m_parent.clear();
        dev[i]->m_son.clear();

        dev[i]->m_parent << "None";
        dev[i]->m_son << "None";
    }
    strParentIdx = QString("%1").arg(0);
    ///////end

    for(int i = 1; i < nLen; ++i)
    {
        strSonIdx = QString("%1").arg(i);
        dev[i]->m_parent << strParentIdx;
        dev[0]->m_son << strSonIdx;
    }

#ifdef TEST_CODE
    if(data.count() == 1)
    {
        dev[0]->m_parent.clear();
        dev[0]->m_son.clear();

        dev[0]->m_parent << "None";
        dev[0]->m_son << "None";
        return;
    }

    ////initialized dev m_parent and m_son
    for(int i = 0; i < data.count(); ++i)
    {
        dev[i]->m_parent.clear();
        dev[i]->m_son.clear();

        dev[i]->m_parent << "None";
        dev[i]->m_son << "None";
    }


    for(int i = 0; i < data.count(); ++i)
    {
        for(int j = 0; j < data[i]->m_linkPlcNodes.count(); ++j)
        {
            tmpDev = data[i]->m_linkPlcNodes[j];

            if(data[i]->m_devMAC == tmpDev->m_devMAC)
            {
                continue;
            }
            for(int k = 0; k < dev.count(); ++k)
            {

                if((*(dev[k]))[GDP_PHYSICALADDR].toString() == tmpDev->m_devMAC)
                {
                    strIdx = QString("%1").arg(i);
                    dev[k]->m_parent << strIdx;
                    strIdx = QString("%1").arg(k);
                    dev[i]->m_son << strIdx;

                    //strRates = QString("%1").arg(dev[i]->m_linkPlcNodeLinkRates[k - 1].first);
                    //dev[k]->m_linkRate_send = strRates;
                    //strRates = QString("%1").arg(dev[i]->m_linkPlcNodeLinkRates[k - 1].second);
                    //dev[k]->m_linkRate_recv = strRates;

                    break;
                }
            }
        }
    }
#endif

}
QWidget *PowerlineMapView::createPageBtnsAndLabelPanel()
{

    /*************refresh button****************/
    //connect(m_refreshBtn, SIGNAL(clicked()), this, SLOT(slot_btn_refresh()));
    //m_refreshBtn->setDisabled(true);
    ///******************END**********************/
    //m_prevPageBtn = new QPushButton(this);
    //m_nextPageBtn = new QPushButton(this);

    //m_firstPageBtn = new QPushButton(this);
    //m_lastPageBtn  = new QPushButton(this);

    ///********2012-3-19 add*************/
    //m_prevPageBtn->setObjectName("prevPageBtn");
    //m_nextPageBtn->setObjectName("nextPageBtn");
    //m_firstPageBtn->setObjectName("firstPageBtn");
    //m_lastPageBtn->setObjectName("lastPageBtn");
    ///**************END****************/

    //m_pagesLabel  = new QLabel(this);

    //m_pagesLabel->setAlignment(Qt::AlignCenter);

    //m_prevPageBtn->setFocusPolicy(Qt::NoFocus);
    //m_nextPageBtn->setFocusPolicy(Qt::NoFocus);

    //m_firstPageBtn->setFocusPolicy(Qt::NoFocus);
    //m_lastPageBtn->setFocusPolicy(Qt::NoFocus);

    //m_prevPageBtn->setFlat(true);
    //m_nextPageBtn->setFlat(true);
    //m_firstPageBtn->setFlat(true);
    //m_lastPageBtn->setFlat(true);

    //connect(m_prevPageBtn,SIGNAL(clicked()),this,SLOT(btnClicked()));
    //connect(m_nextPageBtn,SIGNAL(clicked()),this,SLOT(btnClicked()));
    //connect(m_firstPageBtn,SIGNAL(clicked()),this,SLOT(btnClicked()));
    //connect(m_lastPageBtn,SIGNAL(clicked()),this,SLOT(btnClicked()));


    //m_prevPageBtn->installEventFilter(this);
    //m_nextPageBtn->installEventFilter(this);

    //m_firstPageBtn->installEventFilter(this);
    //m_lastPageBtn->installEventFilter(this);

    //QWidget *panel = new QWidget(this);
    //QHBoxLayout *layOut = new QHBoxLayout(panel);
    //layOut->setSpacing(2);
    //layOut->setContentsMargins(0,0,0,0);

    //layOut->addWidget(m_firstPageBtn);
    //layOut->addWidget(m_prevPageBtn);
    //layOut->addWidget(m_pagesLabel);
    //layOut->addWidget(m_nextPageBtn);
    //layOut->addWidget(m_lastPageBtn);

    //retranslateImages();

    /* return panel;*/
    return NULL;
}
void PowerlineMapView::updateUiElements(bool m_beingWaitting)
{
    //m_pageNavPanel->setVisible(m_pageTotalCount >= 2 && m_currentPageIndex >= 1);
	m_prevPageBtn->setVisible(m_pageTotalCount >= 2 && m_currentPageIndex >= 1);
	m_nextPageBtn->setVisible(m_pageTotalCount >= 2 && m_currentPageIndex >= 1);
    m_prevPageBtn->setEnabled(m_currentPageIndex > 1);
    m_nextPageBtn->setEnabled(m_currentPageIndex < m_pageTotalCount);

    //m_firstPageBtn->setEnabled(m_currentPageIndex > 1);
    //m_lastPageBtn->setEnabled(m_currentPageIndex < m_pageTotalCount);

    //m_netcardPanel->updateUiElements(m_beingWaitting);


    //m_topologyview->btnSetShow(m_pageTotalCount >= 2 && m_currentPageIndex >= 1, PowerlineView::BI_PRE_BTN, m_currentPageIndex > 1);
    //m_topologyview->btnSetShow(m_pageTotalCount >= 2 && m_currentPageIndex >= 1, PowerlineView::BI_NEXT_BTN, m_currentPageIndex < m_pageTotalCount);

    m_refreshBtn->setDisabled(m_beingWaitting);
    if(m_beingWaitting)
    {
        m_topWaittingLayout->setCurrentWidget(m_waittingPanel);
        m_waittingPanel->start();

		m_bAnimateStop = false;
    }
    else
    {
        m_topWaittingLayout->setCurrentWidget(m_topologyview);
        m_waittingPanel->stop();

		m_bAnimateStop = true;
    }
}
void PowerlineMapView::subscribeCurrentPageData()
{
    int nStart = (PER_PAGE_DATACOUNT - 1) * m_currentPageIndex - (PER_PAGE_DATACOUNT - 1);
    nStart += 1;
    int nLen   = m_subscribeDevsList.count() > 0 ? (PER_PAGE_DATACOUNT - 1) : 0;
    if(m_subscribeDevsList.count() == 0)
    {
        m_topologyview->onNewData(NULL, m_subscribeDevsList, nStart, nLen);
        return;
    }

    if((m_devTotalCount % (PER_PAGE_DATACOUNT - 1) != 0) && m_pageTotalCount == m_currentPageIndex)
    {
        m_topologyview->onNewData(m_subscribeDevsList[0], m_subscribeDevsList, nStart, m_devTotalCount % (PER_PAGE_DATACOUNT - 1));
    }
    else
    {
        m_topologyview->onNewData(m_subscribeDevsList[0], m_subscribeDevsList, nStart, nLen);
    }
}
void PowerlineMapView::retranslateImages()
{
 /*   QString prevBtnImage = (qApp->layoutDirection() == Qt::LeftToRight) ? ":/map/others/Prev.png" : ":/map/others/Next.png";
    QString nextBtnImage = (qApp->layoutDirection() == Qt::LeftToRight) ? ":/map/others/Next.png" : ":/map/others/Prev.png";

    QString prevBtnDisableImage = (qApp->layoutDirection() == Qt::LeftToRight) ? ":/map/others/PrevDisabled.png" : ":/map/others/NextDisabled.png";
    QString nextBtnDisableImage = (qApp->layoutDirection() == Qt::LeftToRight) ? ":/map/others/NextDisabled.png" : ":/map/others/PrevDisabled.png";

    QString prevBtnSelectedImage = (qApp->layoutDirection() == Qt::LeftToRight) ? ":/map/others/PrevSelected.png" : ":/map/others/NextSelected.png";
    QString nextBtnSelectedImage = (qApp->layoutDirection() == Qt::LeftToRight) ? ":/map/others/NextSelected.png" : ":/map/others/PrevSelected.png";

    QString firstBtnImage = (qApp->layoutDirection() == Qt::LeftToRight) ? ":/map/others/First.png" : ":/map/others/Last.png";
    QString lastBtnImage = (qApp->layoutDirection() == Qt::LeftToRight) ? ":/map/others/Last.png" : ":/map/others/First.png";

    QString firstBtnDisableImage = (qApp->layoutDirection() == Qt::LeftToRight) ? ":/map/others/FirstDisabled.png" : ":/map/others/LastDisabled.png";
    QString lastBtnDisableImage = (qApp->layoutDirection() == Qt::LeftToRight) ? ":/map/others/LastDisabled.png" : ":/map/others/FirstDisabled.png";

    QString firstBtnSelectedImage = (qApp->layoutDirection() == Qt::LeftToRight) ? ":/map/others/FirstSelected.png" : ":/map/others/LastSelected.png";
    QString lastBtnSelectedImage = (qApp->layoutDirection() == Qt::LeftToRight) ? ":/map/others/LastSelected.png" : ":/map/others/FirstSelected.png";

    QPixmap prevPixmap(prevBtnImage);
    m_prevPageBtn->setFixedSize(prevPixmap.size());
    QPixmap nextPixmap(nextBtnImage);
    m_nextPageBtn->setFixedSize(nextPixmap.size());
    QPixmap firstPixmap(firstBtnImage);
    m_firstPageBtn->setFixedSize(firstPixmap.size());
    QPixmap lastPixmap(lastBtnImage);
    m_lastPageBtn->setFixedSize(lastPixmap.size());

    if (m_prevPageBtnState & BTNSTATE_DISABLED)
    {
        m_prevPageBtn->setIcon(QIcon(prevBtnDisableImage));

    }
    else if(m_prevPageBtnState & BTNSTATE_HOVER)
    {
        m_prevPageBtn->setIcon(QIcon(prevBtnSelectedImage));
    }
    else
    {
        m_prevPageBtn->setIcon(QIcon(prevBtnImage));
    }

    if (m_nextPageBtnState & BTNSTATE_DISABLED)
    {
        m_nextPageBtn->setIcon(QIcon(nextBtnDisableImage));
    }
    else if(m_nextPageBtnState & BTNSTATE_HOVER)
    {
        m_nextPageBtn->setIcon(QIcon(nextBtnSelectedImage));
    }
    else
    {
        m_nextPageBtn->setIcon(QIcon(nextBtnImage));
    }

    if (m_firstPageBtnState & BTNSTATE_DISABLED)
    {
        m_firstPageBtn->setIcon(QIcon(firstBtnDisableImage));
    }
    else if(m_firstPageBtnState & BTNSTATE_HOVER)
    {
        m_firstPageBtn->setIcon(QIcon(firstBtnSelectedImage));
    }
    else
    {
        m_firstPageBtn->setIcon(QIcon(firstBtnImage));
    }

    if (m_lastPageBtnState & BTNSTATE_DISABLED)
    {
        m_lastPageBtn->setIcon(QIcon(lastBtnDisableImage));
    }
    else if(m_lastPageBtnState & BTNSTATE_HOVER)
    {
        m_lastPageBtn->setIcon(QIcon(lastBtnSelectedImage));
    }
    else
    {
        m_lastPageBtn->setIcon(QIcon(lastBtnImage));
    }*/
}
//void PowerlineMapView::deviceFromRawData(PLDevice &dev, const PLCNodeData &data, QPair<qint16,qint16> &linkRate)
//{
//    QString macAddr = data.m_devMAC.toUpper();
//
//    dev.clear();
//    //dev.m_linkPlcNodeLinkRates.clear();
//    //dev.m_linkPlcNodes.clear();
//
//    //dev[GDP_ATTACH_DEV]           = data.m_attachNetDevice;
//	static int nID = 0;
//	nID++;
//	if(nID > 20000)
//	{
//		nID = 0;
//	}
//
//	
//    dev[GDP_VERSION]              = data.m_devFWVersion;
//    dev[GDP_IP]                   = data.m_devIP;
//    dev[GDP_LED_STATUS]           = data.m_devLEDOn;
//    dev[GDP_MODEL_NAME]           = data.m_devModel;
//    dev[GDP_NAME]                 = data.m_devName;
//    dev[GDP_NETWORK_TYPE]         = data.m_devNetworkType;
//    dev[GDP_PHYSICALADDR]         = macAddr;
//    dev[GDP_TYPE]                 = GDT_POWERLINE;
//    dev[GDP_SUPPORT_USB]			 = data.m_withUSB;
//    dev[GDP_SUPPORT_WIRELESS]	 = data.m_withWireless;
//    dev.m_linkRate_recv			 = QString("%1").arg(linkRate.second);
//    dev.m_linkRate_send			 = QString("%1").arg(linkRate.first);
//    dev.m_attachDeviceList		 = data.m_attachDeviceList;
//    dev.m_attachUsbDevice		 = data.m_attachUsbDevice;
//    dev.m_wlanInfo				 = data.m_wlanInfo;
//	dev.m_nID					 = nID;
//}
void PowerlineMapView::deviceFromRawData(PLDevice &dev, const PLCNodeData &data)
{
	QString macAddr = data.m_devMAC.toUpper();

	dev.clear();
	//dev.m_linkPlcNodeLinkRates.clear();
	//dev.m_linkPlcNodes.clear();

	//dev[GDP_ATTACH_DEV]           = data.m_attachNetDevice;
	static int nID = 0;
	nID++;
	if(nID > 20000)
	{
		nID = 0;
	}


	dev[GDP_VERSION]              = data.m_devFWVersion;
	dev[GDP_IP]                   = data.m_devIP;
	dev[GDP_LED_STATUS]           = data.m_devLEDOn;
	dev[GDP_MODEL_NAME]           = (data.m_devModel == "--") ? POWERLINE_UNKNOWN_DEV : data.m_devModel;
	dev[GDP_NAME]                 = data.m_devName;
	dev[GDP_NETWORK_TYPE]         = data.m_devNetworkType;
	dev[GDP_PHYSICALADDR]         = macAddr;
	dev[GDP_TYPE]                 = GDT_POWERLINE;
	dev[GDP_SUPPORT_USB]			 = data.m_withUSB;
	dev[GDP_SUPPORT_WIRELESS]	 = data.m_withWireless;
	//dev.m_linkRate_recv			 = QString("%1").arg(linkRate.second);
	//dev.m_linkRate_send			 = QString("%1").arg(linkRate.first);
	dev.m_attachDeviceList		 = data.m_attachDeviceList;
	dev.m_attachUsbDevice		 = data.m_attachUsbDevice;
	dev.m_wlanInfo				 = data.m_wlanInfo;
	dev.m_nID					 = nID;
	dev.m_linkRate				 = data.m_devLinkRates;


//	static int idx = 0;
//	if(idx == 2)
//	{
//		dev[GDP_MODEL_NAME] = "--";
//	}
//	idx++;
//if(dev[GDP_MODEL_NAME] == "--")
//{
//	dev[GDP_MODEL_NAME] = "UNKNOWN";
//	}
	qDebug() << "d: " << data.m_devLinkRates;
}

bool PowerlineMapView::eventFilter(QObject *obj, QEvent *event)
{
  /*  if ((obj == m_prevPageBtn) || (obj == m_nextPageBtn) ||
        (obj == m_firstPageBtn) || (obj == m_lastPageBtn))
    {
        QPushButton *btn = qobject_cast<QPushButton *>(obj);

        if (event->type() == QEvent::EnabledChange)
        {
            int *pbtnState = NULL;

            if(btn == m_prevPageBtn)
            {
                pbtnState = &m_prevPageBtnState;
            }
            else if(btn == m_nextPageBtn)
            {
                pbtnState = &m_nextPageBtnState;
            }
            else if(btn == m_firstPageBtn)
            {
                pbtnState = &m_firstPageBtnState;
            }
            else
            {
                pbtnState = &m_lastPageBtnState;
            }

            if(btn->isEnabled())
            {
                *pbtnState &= ~BTNSTATE_DISABLED;
            }
            else
            {
                *pbtnState |= BTNSTATE_DISABLED;
            }
        }
        else if(event->type() == QEvent::Enter)
        {
            if(btn == m_prevPageBtn)
            {
                m_prevPageBtnState |= BTNSTATE_HOVER;
            }
            else if(btn == m_nextPageBtn)
            {
                m_nextPageBtnState |= BTNSTATE_HOVER;
            }
            else if(btn == m_firstPageBtn)
            {
                m_firstPageBtnState |= BTNSTATE_HOVER;
            }
            else
            {
                m_lastPageBtnState |= BTNSTATE_HOVER;
            }
        }
        else if(event->type() == QEvent::Leave)
        {
            if(btn == m_prevPageBtn)
            {
                m_prevPageBtnState &= ~BTNSTATE_HOVER ;
            }
            else if(btn == m_nextPageBtn)
            {
                m_nextPageBtnState &= ~BTNSTATE_HOVER;
            }
            else if(btn == m_firstPageBtn)
            {
                m_firstPageBtnState &= ~BTNSTATE_HOVER;
            }
            else
            {
                m_lastPageBtnState &= ~BTNSTATE_HOVER;
            }
        }
        else
        {
            return false;
        }

        retranslateImages();

        return true;
    }*/

    return QWidget::eventFilter(obj, event);

}
void PowerlineMapView::slot_Btn_Click(int nBtn)
{

    if(nBtn == 0)
    {
        ////preBtn
        if(m_currentPageIndex > 1)
        {
            --m_currentPageIndex;
            subscribeCurrentPageData();
        }
    }
    else if(nBtn == 1)
    {
        ////nextBtn
        if(m_currentPageIndex < m_pageTotalCount)
        {
            ++m_currentPageIndex;
            subscribeCurrentPageData();
        }
    }


    updateUiElements();

}
void PowerlineMapView::btnClicked()
{
    QPushButton *btn = qobject_cast<QPushButton*>(sender());
    if(btn == m_prevPageBtn)
    {
        if(m_currentPageIndex > 1)
        {
            --m_currentPageIndex;
            subscribeCurrentPageData();
        }
    }
    else if(btn == m_nextPageBtn)
    {
        if(m_currentPageIndex < m_pageTotalCount)
        {
            ++m_currentPageIndex;
            subscribeCurrentPageData();
        }
    }
    //else if(btn == m_firstPageBtn)
    //{
    //    if(m_pageTotalCount > 0)
    //    {
    //        m_currentPageIndex = 1;
    //        subscribeCurrentPageData();
    //    }
    //}
    //else if(btn == m_lastPageBtn)
    //{
    //    if(m_pageTotalCount > 0)
    //    {
    //        m_currentPageIndex = m_pageTotalCount;
    //        subscribeCurrentPageData();
    //    }
    //}

    updateUiElements();

}

bool PowerlineMapView::slot_btn_refresh()
{
    m_refreshBtn->setDisabled(true);
    
	
	initControlBtn(true);
    updateUiElements(true);


#ifdef TEST_PL_CODE
	if(m_isHubView)
	{
		hideHubView();
	}
#endif
    return m_dlgMgr.refreshScanDev();
}
bool PowerlineMapView::slot_timer_refresh()
{
	return m_dlgMgr.refreshScanDev();
}
bool PowerlineMapView::refresh()
{
    return slot_btn_refresh();
}

bool PowerlineMapView::haveData()
{
    if(m_subscribeDevsList.isEmpty())
    {
        return false;
    }

    return true;
}

void PowerlineMapView::notifyRaise()
{
    qDebug () << "PowerlineMapView::notifyRaise()";

    m_nShowState = SS_SHOW;

    dealAllData();
    m_topologyview->preWirelessConfiguration();
}

void PowerlineMapView::notifyLower()
{
    qDebug () << "PowerlineMapView::notifyLower()";

    m_nShowState = SS_HIDE;
}

void PowerlineMapView::notifyInternetState(bool connected)
{
	//m_topologyview->nodeOnlineStateChange(connected);
	m_topologyview->onInternetStateChanged(connected);

    qDebug () << " PowerlineMapView::notifyInternetState internet state = " << connected;
}
void PowerlineMapView::notifyInternetState()
{
	//m_topologyview->nodeOnlineStateChange(false);
	m_topologyview->onInternetStateChanged(false);
}


void PowerlineMapView::initControlBtn(bool b)
{
	Q_UNUSED(b)
	m_pageTotalCount	= 0;
	m_currentPageIndex	= 1;
	
	m_prevPageBtn->setVisible(false);
	m_nextPageBtn->setVisible(false);
	m_prevPageBtn->setEnabled(false);
	m_nextPageBtn->setEnabled(false);
}



//////////////////////////
////// funtion
///////////////////////
//void PowerlineMapView::insertKey(QList<QPair<QString, QString> > &lst)
//{
//	for(int i = 0; i < lst.count(); ++i)
//	{
//		m_gKey.insert(lst[i].first, lst[i].second);
//	}
//}
//QString PowerlineMapView::getKey(QString &mac)
//{
//	return m_gKey.value(mac);
//}
//void PowerlineMapView::saveIni()
//{
//	POWERLINE_SET_SETTING(CONFIG_SEC_KEY, m_gKey);
//}
//void PowerlineMapView::readIni()
//{
//	m_gKey = POWERLINE_GET_SETTING(CONFIG_SEC_KEY).toMap();
//}
