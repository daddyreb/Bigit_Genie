#include "networkmapview.h"
#include "devicedetailviewmanager.h"
#include "topologyview.h"
#include "mapuitilities.h"
#include "language_Map.h"
#include "netcardspanel.h"
#include "waittingpanel.h"
#include "device.h"
#include "topologynode.h"
#include "blockdevicemanager.h"
#include "genie2_interface.h"

#ifdef ENABLE_POWERLINE
#include "powerlinepage.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <ShlObj.h>
#include "window.h"
#include <CommDlg.h>
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
#include "puttyfuncs.h"
//add by caiyujun
#include <QList>
#include <QUrl>
#include "newsendfilenotifywidget.h"
#include "mapdevicetype.h"
#include "udtproxy.h"
#include "howlpublish.h"
#include "browser.h"
#include "dnsservice.h"
#include "filenotifymanager.h"
#include "DLNAProxy.h"

#include <CommDlg.h>
//
#include <fstream>

//add end
int MakePath(wchar_t *dest, const wchar_t *dir, const wchar_t *file)
{
	BOOL	separetor = TRUE;
	int		len;

	if ((len = wcslen(dir)) == 0)
		return	wsprintf(dest, L"%s", file);

	if (dir[len -1] == '\\')	
	{
		if (len >= 2 && IsDBCSLeadByte(dir[len -2]) == FALSE)
			separetor = FALSE;
		else {
			u_char *p = 0;
			for (p=(u_char *)dir; *p && p[1]; IsDBCSLeadByte(*p) ? p+=2 : p++)
				;
			if (*p == '\\')
				separetor = FALSE;
		}
	}
	return	wsprintf(dest, L"%s%s%s", dir, separetor ? L"\\" : L"", file);
}

int CALLBACK BrowseDirDlgProc(HWND hWnd, UINT uMsg, LPARAM lParam, LPARAM path)
{
	switch (uMsg)
	{
	case BFFM_INITIALIZED:
		SendMessage(hWnd, BFFM_SETSELECTION, (WPARAM)TRUE, path);
		break;

	case BFFM_SELCHANGED:
		break;
	}
	return 0;
}


#define MAP_DEVICECOUNT_PERPAGE     6

void setButtonIco(QPushButton *but,const QString &ico)
{
    Q_ASSERT(but);
#ifdef Q_OS_WIN32
    but->setIcon(QIcon(ico));
#else
    but->setStyleSheet(QString("border-image: url(%1)").arg(ico));
#endif
}

#ifdef Q_OS_MAC
//add by caiyujun
#define LONG_TIME 100000000

typedef union { unsigned char b[2]; unsigned short NotAnInteger; } Opaque16;

static int operation;
static uint32_t opinterface = kDNSServiceInterfaceIndexAny;
static DNSServiceRef client    = NULL;
static DNSServiceRef disclient = NULL;
static DNSServiceRef client_pa = NULL;	// DNSServiceRef for RegisterProxyAddressRecord
static DNSServiceRef sc1;		// DNSServiceRefs for kDNSServiceFlagsShareConnection

static volatile int stopNow = 0;
static volatile int timeOut = LONG_TIME;

QMap<QString,QString> NetworkMapView::m_onLineServiceToHost;
//QMap<QString,QString> NetworkMapView::m_onLineHostToIp;
std::vector<std::string> NetworkMapView::m_onLineServiceIp;
NetworkMapView * NetworkMapView::pthis=NULL;
//add end
#endif

NetworkMapView::NetworkMapView(QWidget *parent) :
    QWidget(parent),
    m_topologyView(0),
    m_ddvm(0),
    m_prevPageBtnState(BTNSTATE_NORMAL),
    m_nextPageBtnState(BTNSTATE_NORMAL),
    m_firstPageBtnState(BTNSTATE_NORMAL),
    m_lastPageBtnState(BTNSTATE_NORMAL),
    m_layOut(0),
    m_devTotalCount(0),
    m_pageTotalCount(0),
    m_currentPageIndex(1),
    m_beingWaitting(true),
    //add by caiyujun
    m_currentNodeIndex(-1),
    m_isInNode(false),
    m_strLastOpenFolder(""),
	m_bExtendMap(false),
	mCurrentExtenderIp(0)
    //add end
  #ifdef ENABLE_POWERLINE
  ,m_powerlinePageValid(false)
  ,m_plcPagingNow(false)
  #endif
{
    //add by caiyujun

	m_howlpublish = 0;
	m_browser = 0;
	m_service = 0;

   // add end
    m_topologyView = new TopologyView(this);

    connect(m_topologyView,SIGNAL(nodeClicked(int)),
            this,SLOT(onNodeClicked(int)));
    //add by caiyujun
    connect(m_topologyView,SIGNAL(mouseInNode(bool,int)),
            this,SLOT(onMouseInNode(bool,int)));
    connect(m_topologyView,SIGNAL(bonjourTypeChanged(Device)),
            this,SLOT(onBonjourTypeChanged(Device)));
    //add end

    connect(m_topologyView,SIGNAL(nodeContextMenu(int,QPoint)),
            this,SLOT(onNodeContextMenu(int,QPoint)));
    connect(m_topologyView,SIGNAL(translateText(int,QString*)),
            this,SIGNAL(translateText(int,QString*)));

    m_topologyView->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

    //add by caiyujun
    setAcceptDrops(true);
    //add end

    m_waittingPanel = new WaittingPanel(this);
    connect(m_waittingPanel,SIGNAL(translateText(int,QString*)),
            this,SIGNAL(translateText(int,QString*)));
    m_waittingPanel->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
#ifdef ENABLE_POWERLINE
    m_mapPage = new QWidget(this);
    m_mapPageLayout = new QStackedLayout(m_mapPage);
    m_mapPageLayout->setContentsMargins(0,0,0,0);
    m_mapPageLayout->addWidget(m_topologyView);
    m_mapPageLayout->addWidget(m_waittingPanel);


    m_powerlinePage = new PowerLinePage(this);
    connect(m_powerlinePage, SIGNAL(refreshOver()), SLOT(onPowerlineRefreshOver()));
    connect(m_powerlinePage, SIGNAL(pageChange(int)), SLOT(onPLCPageChanged(int)));
    connect(m_powerlinePage, SIGNAL(queryPLCDevicePassword(QString,QString*)),
            this, SIGNAL(queryPLCDevicePassword(QString,QString*)));
    connect(m_powerlinePage, SIGNAL(storePLCDevicePassword(QString,QString)),
            this, SIGNAL(storePLCDevicePassword(QString,QString)));
#endif

    //m_extenderPage = new ExtenderPage(this);

    m_netmapStackLayout = new QStackedLayout();
    m_netmapStackLayout->setContentsMargins(0,0,0,0);
#ifdef ENABLE_POWERLINE
    m_netmapStackLayout->addWidget(m_mapPage);
    m_netmapStackLayout->addWidget(m_powerlinePage);
    //m_netmapStackLayout->addWidget(m_extenderPage);
#else
    m_netmapStackLayout->addWidget(m_topologyView);
    m_netmapStackLayout->addWidget(m_waittingPanel);
#endif



    m_devNumberLabel = new QLabel(this);
    m_netcardPanel = new NetcardsPanel(this);
    m_notifyNewDevCB = new QCheckBox(this);


    m_devNumberLabel->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred);
    m_notifyNewDevCB->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred);
    m_notifyNewDevCB->setFocusPolicy(Qt::NoFocus);

    connect(m_notifyNewDevCB,SIGNAL(stateChanged(int)),this,SLOT(stateChanged(int)));
    connect(m_netcardPanel,SIGNAL(currentNetcardChanged(int)),
            this,SLOT(currrentNetcardChanged(int)));
    connect(m_netcardPanel,SIGNAL(refreshClicked()),this,SLOT(onRefreshClicked()));

#ifdef ENABLE_POWERLINE
    connect(m_netcardPanel, SIGNAL(requestGoPLCPage(bool)), SLOT(onGo2PLCPage(bool)));
    connect(m_netcardPanel, SIGNAL(backClicked()), SLOT(onBackClicked()));
#endif

    QHBoxLayout *horLayout = new QHBoxLayout();
    horLayout->addWidget(m_devNumberLabel);
    horLayout->addWidget(m_netcardPanel);

    m_pageNavPanel = createPageBtnsAndLabelPanel();


    QHBoxLayout *horLayout2 = new QHBoxLayout();
    horLayout2->addWidget(m_notifyNewDevCB);
    QSpacerItem *hspacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    horLayout2->addSpacerItem(hspacer);
    horLayout2->addWidget(m_pageNavPanel);
    horLayout2->setStretchFactor(m_notifyNewDevCB,2);


    m_layOut = new QVBoxLayout(this);
    m_layOut->addLayout(horLayout);
    m_layOut->addLayout(m_netmapStackLayout);
    m_layOut->addLayout(horLayout2);


    QShortcut *shortcut = new QShortcut(this);
    shortcut->setContext(Qt::ApplicationShortcut);
    shortcut->setKey(Qt::Key_Escape);
    connect(shortcut,SIGNAL(activated()),this,SLOT(escKeyActived()));

    applyUiElementsStyle();

    retranslateUi();
    updateUiElements();

    //add by caiyujun
    //test();
	pUdt = UDTProxy::GetInStance();
	//connect(pUdt,SIGNAL(serviceOffline(QString,int)),this,SLOT(onServiceOffline(QString,int)));

	m_fileNotifyManager = new FileNotifyManager(this);

	connect(m_fileNotifyManager,SIGNAL(translateText(int,QString*)),this,SIGNAL(translateText(int,QString*)));
	connect(m_fileNotifyManager, SIGNAL(queryDefaultRecvFolder(QString*)),
		this,SIGNAL(queryDefaultRecvFolder(QString*)));
	connect(m_fileNotifyManager, SIGNAL(requestStoreDefaultRecvFolder(QString)),
		this,SIGNAL(requestStoreDefaultRecvFolder(QString)));
	connect(pUdt,SIGNAL(recvFile(int,int,QString,QString,QString,QString)),
		m_fileNotifyManager,SLOT(onRecvFile(int,int,QString,QString,QString,QString)));
	connect(pUdt,SIGNAL(recvMessage(QString,QString,QString)),
		m_fileNotifyManager,SLOT(onRecvMessage(QString,QString,QString)));
	connect(pUdt,SIGNAL(fileFinished(int,int)),
		m_fileNotifyManager,SLOT(onFileFinished(int,int)));
	connect(pUdt,SIGNAL(transfer(int,QString,double,double)),
		m_fileNotifyManager,SLOT(onTransfer(int,QString,double,double)));
	connect(m_fileNotifyManager,SIGNAL(saveResult(int,QString)),
		pUdt,SLOT(onSaveResult(int,QString)));
	connect(m_fileNotifyManager,SIGNAL(stopTransfer(int)),
		pUdt,SLOT(onStopTransfer(int)));
	connect(pUdt,SIGNAL(sendFile(int,QString,int,int)),
		m_fileNotifyManager,SLOT(onSendFile(int,QString,int,int)));
	connect(pUdt,SIGNAL(connResult(int,int,int)),
		m_fileNotifyManager,SLOT(onConnResult(int,int,int)));

	pUdt->Init(7777, 7778);

    initDeviceNameToType();
	m_service = new dnsservice(this);
	m_service->start();
	m_howlpublish = new howlpublish(this);
    connect(m_service,SIGNAL(srvStarted(bool)),m_howlpublish,SLOT(onSrvStarted(bool)));
    connect(m_howlpublish,SIGNAL(rebootService()),m_service,SLOT(onRebootService()));
    m_howlpublish->start();
    m_browser = Browser::GetInStance();
    connect(m_service,SIGNAL(srvStarted(bool)),m_browser,SLOT(onSrvStarted(bool)));
    connect(m_browser,SIGNAL(removeService(QString)),this,SLOT(onRemoveService(QString)));
    connect(m_browser,SIGNAL(addService(QString,QString)),this,SLOT(onAddService(QString,QString)));
    connect(m_browser,SIGNAL(rebootService()),m_service,SLOT(onRebootService()));
    connect(m_browser,SIGNAL(rebootService()),m_howlpublish,SLOT(onRebootService()));

    connect(m_howlpublish,SIGNAL(rebootService()),m_browser,SLOT(onRebootService()));
    m_browser->start();
	dlnaProxy = DLNAProxy::GetInstance();
	connect(dlnaProxy,SIGNAL(dlnaChange(QString,QString,int)),this,SLOT(onDlnaChange(QString,QString,int)));
	dlnaProxy->enableFunction(DLNACore::Function_ControlPoint, true);
	dlnaProxy->enableFunction(DLNACore::Function_MediaServer, true);

	
    //add end

#ifdef ENABLE_POWERLINE
    m_powerlinePage->refresh();
#endif
}

NetworkMapView::~NetworkMapView()
{
    //delete m_howlpublish;
}
#ifdef USE_NODE_ANIMATION
#ifdef REBOUNCE_FOR_RAISE
void NetworkMapView::reBounce()
{
    m_topologyView->mAnimTimer.start(100);
    foreach(TopologyNode *node,m_topologyView->m_nodesList)
    {
        node->reBounce();
    }
    if(m_topologyView->m_InternetNode)
    {
        m_topologyView->m_InternetNode->reBounce();
    }

//    if (NULL != client)
//    {
//        DNSServiceRefDeallocate(client);
//    }
}

//add by caiyujun
void NetworkMapView::test()
{

//    char ac[80];
//    int i = gethostname(ac, sizeof(ac));
//    qDebug() << "start get local ip";
//    struct hostent *phe = gethostbyname(ac);
//    if (phe == 0)
//    {
//        qDebug() << "get local ip failed";
//    }
//    else
//    {
//        for (int i = 0; phe->h_addr_list[i] != 0; ++i)
//        {
//            struct in_addr addr;
//            memcpy(&addr, phe->h_addr_list[i], sizeof(struct in_addr));
//            QString strLocalIp = inet_ntoa(addr);
//            qDebug() << "localip:" << strLocalIp;
//        }
//    }

//    QStringList env_list(QProcess::systemEnvironment());
//    foreach(QString item,env_list)
//    {
//        qDebug() << "environment: " << item;
//    }

//    int idx = env_list.indexOf(QRegExp("^WINDIR=.*", Qt::CaseInsensitive));
//    if (idx > -1)
//    {
//        QStringList windir = env_list[idx].split('=');
//        qDebug() << "Var : " << windir[0];
//        qDebug() << "Path: " << windir[1];
//        QString path = windir[1];
//        path += "\\";
//        QString strDnssdPath1 = path += "system32\\dnssd.dll";
//        QString strDnssdPath2 = path += "System32\\dnssd.dll";
//        QString strDnssdPath3 = path += "SysWOW64\\dnssd.dll";
//        QFile file1(strDnssdPath1);
//        QFile file2(strDnssdPath2);
//        QFile file3(strDnssdPath3);
//        if (file1.exists() || file2.exists() || file3.exists())
//        {
//            qDebug() << "dnssd existed";

//            const char *serviceName = ac;

//            Opaque16 registerPort = { { 0x23, 0x45 } };
//            DNSServiceErrorType err;
//            const char TXT[] = "\x09" "Test Data";
//            err = DNSServiceRegister(&client, 0, opinterface, serviceName, "_ros-master._tcp", "", NULL, registerPort.NotAnInteger, 0, NULL, reg_reply, NULL);
//            if (err)
//            {
//                err = DNSServiceRegister(&client, 0, opinterface, serviceName, "_ros-master._tcp", "", NULL, registerPort.NotAnInteger, 0, NULL, reg_reply, NULL);
//            }

//            if (!err)
//            {
//                err = DNSServiceUpdateRecord(client, NULL, 0, sizeof(TXT)-1, TXT, 0);
//                if (err)
//                {
//                    err = DNSServiceUpdateRecord(client, NULL, 0, sizeof(TXT)-1, TXT, 0);
//                }

//                qDebug() << "register success";

//                int socket = DNSServiceRefSockFD(client);
//                QSocketNotifier* notifier = new QSocketNotifier(socket, QSocketNotifier::Read, this);

//                QObject::connect(notifier, SIGNAL(activated(int)), this, SLOT(socketData()));

//                char *typ, *dom;
//                int opi;
//                dom = "";  // Missing domain argument is the same as empty string i.e. use system default(s)
//                typ = "_ros-master._tcp";
//                err = DNSServiceCreateConnection(&disclient);
//                qDebug() << "connection errcode :" << err;
//                sc1 = disclient;
//                err = DNSServiceBrowse(&sc1, kDNSServiceFlagsShareConnection, opinterface, typ, dom, zonedata_browse, NULL);

//                int socket1 = DNSServiceRefSockFD(disclient);
//                QSocketNotifier* notifier1 = new QSocketNotifier(socket1, QSocketNotifier::Read, this);

//                QObject::connect(notifier1, SIGNAL(activated(int)), this, SLOT(socketData1()));
//            }
//            else
//            {
//                qDebug() << "register failed";
//            }
//        }
//        else
//        {
//            qDebug() << "not existed";
//        }
//    }
}


void NetworkMapView::setExit()
{
	if(dlnaProxy)
	{
		dlnaProxy->setProperty("CUSTOM", "");
		dlnaProxy->enableFunction(DLNACore::Function_MediaServer, false);
		dlnaProxy->enableFunction(DLNACore::Function_ControlPoint, false);	
	}
   if(m_howlpublish)
	 m_howlpublish->cancelPublish();
   if(m_howlpublish)
     m_service->stopService();

}

void NetworkMapView::initDeviceNameToType()
{
    m_devNameToType.insert("Windows", MDT_WINDOWSPC);
    m_devNameToType.insert("MacBook", MDT_MACBOOK);
    m_devNameToType.insert("IMac", MDT_IMAC);
    m_devNameToType.insert("MacMini", MDT_MACMINI);
    m_devNameToType.insert("MacPro", MDT_MACPRO);
    m_devNameToType.insert("iPad", MDT_IPAD);
    m_devNameToType.insert("iPodtouch", MDT_IPODTOUCH);
    m_devNameToType.insert("iphone", MDT_IPHONE);
    m_devNameToType.insert("AndroidTablet", MDT_ANDROIDTABLET);
    m_devNameToType.insert("AndroidPhone", MDT_ANDROIDPHONE);
    m_devNameToType.insert("AndroidDevice", MDT_ANDROIDDEVICE);
    m_devNameToType.insert("AmazonKindle", MDT_AMAZONKINDLE);
    m_devNameToType.insert("iPadMini", MDT_IPADMINI);
    m_devNameToType.insert("iphone5", MDT_IPHONE5);

    m_unableSendFolderType.push_back(MDT_IPAD);
    m_unableSendFolderType.push_back(MDT_IPODTOUCH);
    m_unableSendFolderType.push_back(MDT_IPHONE);
    m_unableSendFolderType.push_back(MDT_IPADMINI);
    m_unableSendFolderType.push_back(MDT_IPHONE5);
}

bool NetworkMapView::unsupportedSendFolderType(int iType)
{
	if (m_unableSendFolderType.contains(iType))
	{
		return true;
	}

	return false;
}

//void NetworkMapView::HandleEvents(void)
//{
//    int dns_sd_fd  = client    ? DNSServiceRefSockFD(client   ) : -1;
//    int dns_sd_fd2 = client_pa ? DNSServiceRefSockFD(client_pa) : -1;
//    int nfds = dns_sd_fd + 1;
//    fd_set readfds;
//    struct timeval tv;
//    int result;

//    if (dns_sd_fd2 > dns_sd_fd) nfds = dns_sd_fd2 + 1;

//    while (!stopNow)
//    {
//        // 1. Set up the fd_set as usual here.
//        // This example client has no file descriptors of its own,
//        // but a real application would call FD_SET to add them to the set here
//        FD_ZERO(&readfds);

//        // 2. Add the fd for our client(s) to the fd_set
//        if (client   ) FD_SET(dns_sd_fd , &readfds);
//        if (client_pa) FD_SET(dns_sd_fd2, &readfds);

//        // 3. Set up the timeout.
//        tv.tv_sec  = timeOut;
//        tv.tv_usec = 0;

//        result = select(nfds, &readfds, (fd_set*)NULL, (fd_set*)NULL, &tv);
//        if (result > 0)
//        {
//            DNSServiceErrorType err = kDNSServiceErr_NoError;
//            if(client && FD_ISSET(dns_sd_fd, &readfds))
//            {
//                err = DNSServiceProcessResult(client   );
//            }
//            else if(client_pa && FD_ISSET(dns_sd_fd2, &readfds))
//            {
//                err = DNSServiceProcessResult(client_pa);
//            }
//            if (err)
//            {
//                stopNow = 1;
//            }
//        }
//        else if (result == 0)
//        {
//            //
//        }
//        else
//        {
//            if (errno != EINTR)
//            {
//                stopNow = 1;
//            }
//        }
//    }
//}

//void NetworkMapView::reg_reply(DNSServiceRef sdref, const DNSServiceFlags flags, DNSServiceErrorType errorCode,
//                               const char *name, const char *regtype, const char *domain, void *context)
//{
//    (void)sdref;    // Unused
//    (void)flags;    // Unused
//    (void)context;  // Unused

//    qDebug() << "register success";


//    if (errorCode == kDNSServiceErr_NoError)
//    {
//        if (flags & kDNSServiceFlagsAdd) printf("Name now registered and active\n");
//        else printf("Name registration removed\n");
//        if (operation == 'A' || operation == 'U' || operation == 'N') timeOut = 5;
//    }
//    else if (errorCode == kDNSServiceErr_NameConflict)
//    {
//        printf("Name in use, please choose another\n");
//        exit(-1);
//    }
//    else
//        printf("Error %d\n", errorCode);

//    if (!(flags & kDNSServiceFlagsMoreComing)) fflush(stdout);
//}

//void NetworkMapView::zonedata_browse(DNSServiceRef sdref, const DNSServiceFlags flags, uint32_t ifIndex, DNSServiceErrorType errorCode,
//                                     const char *replyName, const char *replyType, const char *replyDomain, void *context)
//{
//    DNSServiceRef *newref;
//    (void)sdref;        // Unused
//    (void)context;      // Unused

//    qDebug() << "add or remove service****";
//    qDebug() << "replyname:" << replyName;
//    qDebug() << "replydomain:" << replyDomain;
//    if (!(flags & kDNSServiceFlagsAdd))
//    {
//        qDebug() << "replyname :" << replyName << " index:" << ifIndex;

//        QString strReplyName = replyName;
//        strReplyName.replace("\\032","\x20");
//        strReplyName.remove("\\");
//        QStringList strList = strReplyName.split(".");
//        QString strServiceName = strList.at(0);

//        QMap<QString,QString>::iterator it = m_onLineServiceToHost.find(strServiceName);
//        if (it != m_onLineServiceToHost.end())
//        {
//            QString host = it.value();
//            m_onLineServiceToHost.remove(strServiceName);
//            QMap<QString,QString>::iterator it1 = m_onLineHostToIp.find(host);
//            if (it1 != m_onLineHostToIp.end())
//            {
//                QString ip = it1.value();
//                m_onLineHostToIp.remove(host);
//                Device dev;
//                emit pthis->ip2Device(it1.value(),&dev);
//                dev[DP_TURBOFLAG] = false;
//                pthis->m_topologyView->onTurboStateChanged(&dev,ip,false);

//                return;
//            }
//        }
//    }
//    else
//    {
//        printf("Add*****************\n");
//    }
//    if (errorCode) { printf("Error code %d\n", errorCode); return; }

//    if (NULL != disclient)
//    {
//        newref = (DNSServiceRef *)malloc(sizeof(*newref));
//        *newref = disclient;
//        DNSServiceResolve(newref, kDNSServiceFlagsShareConnection, ifIndex, replyName, replyType, replyDomain, zonedata_resolve, newref);
//        free(newref);
//    }
//}

//void NetworkMapView::zonedata_resolve(DNSServiceRef sdref, const DNSServiceFlags flags, uint32_t ifIndex, DNSServiceErrorType errorCode,
//                                      const char *fullname, const char *hosttarget, uint16_t opaqueport, uint16_t txtLen, const unsigned char *txt, void *context)
//{
//    qDebug() << "resolve data";
//    union { uint16_t s; u_char b[2]; } port = { opaqueport };
//    uint16_t PortAsNumber = ((uint16_t)port.b[0]) << 8 | port.b[1];

//    const char *p = fullname;
//    char n[kDNSServiceMaxDomainName];
//    char t[kDNSServiceMaxDomainName];

//    const unsigned char *max = txt + txtLen;

//    (void)sdref;        // Unused
//    (void)ifIndex;      // Unused
//    (void)context;      // Unused

//    QString strFullName = fullname;
//    qDebug() << "fullname" << strFullName << "index:" << ifIndex;
//    strFullName.replace("\\032","\x20");
//    strFullName.remove("\\");
//    QStringList strList = strFullName.split(".");
//    QString strServiceName = strList.at(0);

//    qDebug() << "hostname:" << hosttarget;
//    printf("hosttarget:%s\n",hosttarget);

//    if (!m_onLineServiceToHost.contains(strServiceName))
//    {
//        m_onLineServiceToHost.insert(strServiceName,hosttarget);
//    }

//    if (NULL != disclient)
//    {
//        DNSServiceRef *newref;
//        newref = (DNSServiceRef *)malloc(sizeof(*newref));
//        *newref = disclient;
//        DNSServiceGetAddrInfo(newref, kDNSServiceFlagsShareConnection, opinterface, kDNSServiceProtocol_IPv4, hosttarget, addrinfo_reply, NULL);
//        free(newref);
//    }

//    if (NULL != sdref)
//    {
//        //DNSServiceRefDeallocate(sdref);
//    }
//    if (NULL != context)
//    {
//        //free(context);
//    }
//    if (!(flags & kDNSServiceFlagsMoreComing)) fflush(stdout);
//}


//void NetworkMapView::addrinfo_reply(DNSServiceRef sdref, DNSServiceFlags flags, uint32_t interfaceIndex, DNSServiceErrorType errorCode, const char *hostname, const struct sockaddr *address, uint32_t ttl, void *context)
//{
//    const char *op = (flags & kDNSServiceFlagsAdd) ? "Add" : "Rmv";
//    char addr[256] = "";
//    (void) sdref;
//    (void) context;

//    qDebug() << "addrinfo_reply";


//    if (address && address->sa_family == AF_INET)
//    {
//        const unsigned char *b = (const unsigned char *) &((struct sockaddr_in *)address)->sin_addr;
//        snprintf(addr, sizeof(addr), "%d.%d.%d.%d", b[0], b[1], b[2], b[3]);

//        qDebug() << "ipaddress:" << addr;
//        qDebug() << "hostname 111:" << hostname;
//        if (!m_onLineHostToIp.contains(hostname))
//        {
//            QString ip = addr;
//            m_onLineHostToIp.insert(hostname,ip);
//            qDebug() << "online host:" << hostname << " onlineIp:" << ip;
//            Device dev;
//            emit pthis->ip2Device(ip,&dev);
//            dev[DP_TURBOFLAG] = true;
//            m_onLineHostToIp.insert(hostname,ip);
//            pthis->m_topologyView->onTurboStateChanged(&dev,ip,true);
//        }
//    }

//    if (errorCode)
//    {
//        if (errorCode == kDNSServiceErr_NoSuchRecord) printf("   No Such Record");
//        else                                          printf("   Error code %d", errorCode);
//    }

//    if (!(flags & kDNSServiceFlagsMoreComing)) fflush(stdout);
//}

//qint64 NetworkMapView::countSize(QString filePath)
//{
//    m_fileList.clear();
//    m_emptyFolderList.clear();
//    qint64 fileSize = 0;
//    QList<QString> fileList;
//    QFileInfo fileInfo(filePath);
//    QString strTopDir;
//    if(fileInfo.isDir())
//    {
//        m_bFolder = true;
//        if (filePath.contains('/'))
//        {
//            QStringList strList = filePath.split("/");
//            int iSize = strList.size();
//            strTopDir = strList.at(iSize-1);
//        }
//        else if(filePath.contains('\\'))
//        {
//            QStringList strList = filePath.split("\\");
//            int iSize = strList.size();
//            strTopDir = strList.at(iSize-1);
//        }
//        fileList.push_back(filePath);
//    }
//    else
//    {
//        m_bFolder = false;
//        m_fileList.push_front(filePath);
//        fileSize = fileInfo.size();
//        QString createTime = fileInfo.created().toString();
//        QString modifiedTime = fileInfo.lastModified().toString();
//        QString checkString = createTime + modifiedTime;
//        checkString += filePath;
//    }
//
//    while(!fileList.isEmpty())
//    {
//        QString filePath = fileList.value(0);
//        QDir dir(filePath);
//        dir.setFilter(QDir::Dirs|QDir::Files|QDir::Hidden|QDir::System);
//        dir.setSorting(QDir::DirsFirst);
//        QFileInfoList list = dir.entryInfoList();
//        QFileInfoList::iterator iter;
//        if (2 == list.size())
//        {
//            qDebug() << "empty dir : " << filePath;
//            m_emptyFolderList.push_back(filePath);
//        }
//        for(iter=list.begin(); iter!=list.end(); iter++)
//        {
//            //iterator folder
//            if(iter->fileName() == "." || iter->fileName() == "..")
//            {
//                continue;
//            }
//            if(iter->isDir())
//            {
//                fileList.push_back(iter->filePath());
//            } else
//            {
//                //if is file,get file size and push file list;
//                fileSize += iter->size();
//                m_fileList.push_front(iter->filePath());
//            }
//        }
//        fileList.pop_front();
//    }
//
//    qDebug() << "file or director size : " << fileSize;
//    return fileSize;
//}

//add end

void NetworkMapView::fadeoutAll()
{
    foreach(TopologyNode *node,m_topologyView->m_nodesList)
    {
        node->setVisible(false);
    }
    if(m_topologyView->m_InternetNode)
    {
        m_topologyView->m_InternetNode->setVisible(false);
    }
}
#endif
#endif

void NetworkMapView::checkNewDeviceNotifyCB()
{
    int option = 0;

    emit queryNewDeviceNotifyOption(&option);

    m_notifyNewDevCB->setChecked(option != 0);
}

int NetworkMapView::getDefaultInterface()
{
    QString infIdx;
    emit queryDefaultInterface(&infIdx);

    bool bok = false;
    int idx = infIdx.toInt(&bok);

    return ((infIdx.isEmpty() || !bok) ? -1 : idx);
}

void NetworkMapView::storeDefaultInterface()
{
    if(m_netcardPanel->m_currentAdapterId != -1)
    {
        emit requestStoreDefaultInterface(QString::number(m_netcardPanel->m_currentAdapterId));
    }
}

QString NetworkMapView::translateText(int idx, const QString &defaultText)
{
    QString text;
    emit translateText(idx,&text);
    return text.isEmpty() ? defaultText : text;
}

void NetworkMapView::changeEvent(QEvent *event)
{
    if(event->type() == QEvent::LanguageChange)
    {
        retranslateUi();
        emit languageChanged();
    }
    else if(event->type() == QEvent::LayoutDirectionChange)
    {
        retranslateImages();
    }

    QWidget::changeEvent(event);
}

void NetworkMapView::showEvent(QShowEvent *event)
{
    checkNewDeviceNotifyCB();

#ifdef ENABLE_POWERLINE
    m_powerlinePage->verifyStart();
#endif

    QWidget::showEvent(event);
}

//add by caiyujun
void NetworkMapView::dragEnterEvent(QDragEnterEvent *event)
{
    m_urls.clear();
    event->acceptProposedAction();
}

void NetworkMapView::dropEvent(QDropEvent *event)
{
    if (event->mimeData()->hasUrls())
    {
        m_urls = event->mimeData()->urls();
    }
}
//add end

void NetworkMapView::applyUiElementsStyle()
{
}

void NetworkMapView::retranslateImages()
{
    QString prevBtnImage = GENIE2_RES((qApp->layoutDirection() == Qt::LeftToRight) ? "map/others/Prev.png" : "map/others/Next.png");
    QString nextBtnImage = GENIE2_RES((qApp->layoutDirection() == Qt::LeftToRight) ? "map/others/Next.png" : "map/others/Prev.png");

    QString prevBtnDisableImage = GENIE2_RES((qApp->layoutDirection() == Qt::LeftToRight) ? "map/others/PrevDisabled.png" : "map/others/NextDisabled.png");
    QString nextBtnDisableImage = GENIE2_RES((qApp->layoutDirection() == Qt::LeftToRight) ? "map/others/NextDisabled.png" : "map/others/PrevDisabled.png");

    QString prevBtnSelectedImage = GENIE2_RES((qApp->layoutDirection() == Qt::LeftToRight) ? "map/others/PrevSelected.png" : "map/others/NextSelected.png");
    QString nextBtnSelectedImage = GENIE2_RES((qApp->layoutDirection() == Qt::LeftToRight) ? "map/others/NextSelected.png" : "map/others/PrevSelected.png");

    QString firstBtnImage = GENIE2_RES((qApp->layoutDirection() == Qt::LeftToRight) ? "map/others/First.png" : "map/others/Last.png");
    QString lastBtnImage = GENIE2_RES((qApp->layoutDirection() == Qt::LeftToRight) ? "map/others/Last.png" : "map/others/First.png");

    QString firstBtnDisableImage = GENIE2_RES((qApp->layoutDirection() == Qt::LeftToRight) ? "map/others/FirstDisabled.png" : "map/others/LastDisabled.png");
    QString lastBtnDisableImage = GENIE2_RES((qApp->layoutDirection() == Qt::LeftToRight) ? "map/others/LastDisabled.png" : "map/others/FirstDisabled.png");

    QString firstBtnSelectedImage = GENIE2_RES((qApp->layoutDirection() == Qt::LeftToRight) ? "map/others/FirstSelected.png" : "map/others/LastSelected.png");
    QString lastBtnSelectedImage = GENIE2_RES((qApp->layoutDirection() == Qt::LeftToRight) ? "map/others/LastSelected.png" : "map/others/FirstSelected.png");

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
        setButtonIco(m_prevPageBtn,prevBtnDisableImage);
    }
    else if(m_prevPageBtnState & BTNSTATE_HOVER)
    {
        setButtonIco(m_prevPageBtn,prevBtnSelectedImage);
        // m_prevPageBtn->setIcon(QIcon(prevBtnSelectedImage));
    }
    else
    {
        setButtonIco(m_prevPageBtn,prevBtnImage);
        //   m_prevPageBtn->setIcon(QIcon(prevBtnImage));
    }

    if (m_nextPageBtnState & BTNSTATE_DISABLED)
    {
        setButtonIco(m_nextPageBtn,nextBtnDisableImage);
        // m_nextPageBtn->setIcon(QIcon());
    }
    else if(m_nextPageBtnState & BTNSTATE_HOVER)
    {
        setButtonIco(m_nextPageBtn,nextBtnSelectedImage);
        // m_nextPageBtn->setIcon(QIcon());
    }
    else
    {
        setButtonIco(m_nextPageBtn,nextBtnImage);
        // m_nextPageBtn->setIcon(QIcon());
    }

    if (m_firstPageBtnState & BTNSTATE_DISABLED)
    {
        setButtonIco(m_firstPageBtn,firstBtnDisableImage);
        //  m_firstPageBtn->setIcon(QIcon(firstBtnDisableImage));
    }
    else if(m_firstPageBtnState & BTNSTATE_HOVER)
    {
        setButtonIco(m_firstPageBtn,firstBtnSelectedImage);
        //  m_firstPageBtn->setIcon(QIcon(firstBtnSelectedImage));
    }
    else
    {
        setButtonIco(m_firstPageBtn,firstBtnImage);
        // m_firstPageBtn->setIcon(QIcon(firstBtnImage));
    }

    if (m_lastPageBtnState & BTNSTATE_DISABLED)
    {
        setButtonIco(m_lastPageBtn,lastBtnDisableImage);
        //  m_lastPageBtn->setIcon(QIcon());
    }
    else if(m_lastPageBtnState & BTNSTATE_HOVER)
    {
        setButtonIco(m_lastPageBtn,lastBtnSelectedImage);
        //   m_lastPageBtn->setIcon(QIcon());
    }
    else
    {
        setButtonIco(m_lastPageBtn,lastBtnImage);
        // m_lastPageBtn->setIcon(QIcon());
    }
}

void NetworkMapView::retranslateUi()
{
#ifdef ENABLE_POWERLINE
    if(/*m_pageTotalCount >= 2*/(m_devTotalCount - 1) > 5 && (!(m_powerlinePageValid &&  (m_netmapStackLayout->currentWidget() == m_powerlinePage)/*m_currentPageIndex == 1*/)))
#else
    if(/*m_pageTotalCount >= 2*/(m_devTotalCount - 1) > 5)
#endif
    {
        QString tmp;
        tmp.setNum(m_devTotalCount - 1);
        m_devNumberLabel->setText(translateText(L_MAP_DEVICENUMBER,QString("Number of devices: ")) + tmp);
    }
    else
    {
        m_devNumberLabel->setText("");
    }

    m_notifyNewDevCB->setText(translateText(L_MAP_NOTIFYNEWDEVICE,QString("Notify me of new devices that connect to the network")));

#ifdef ENABLE_POWERLINE
    if(!m_plcPagingNow){
        if(m_pageTotalCount >= 2 && m_currentPageIndex >= 1)
        {
            QString strTmp("%1/%2");
            m_pagesLabel->setText(strTmp.arg(QString::number(m_currentPageIndex),QString::number(m_pageTotalCount)));
        }
    }else{
        if(m_powerlinePage->pageCount() >= 2)
        {
            QString strTmp("%1/%2");
            m_pagesLabel->setText(strTmp.arg(QString::number(m_powerlinePage->curPage()),QString::number(m_powerlinePage->pageCount())));
        }

        m_netcardPanel->setBackBtnText(translateText(L_MAP_CLOSE, "Close"));
    }
#else
    if(m_pageTotalCount >= 2 && m_currentPageIndex >= 1)
    {
        QString strTmp("%1/%2");
        m_pagesLabel->setText(strTmp.arg(QString::number(m_currentPageIndex),QString::number(m_pageTotalCount)));
    }
#endif
}


void NetworkMapView::onNodeClicked(int idx)
{
    Device dev;
    emit node2Device(idx,&dev);

    qDebug() << "Network map view node clicked";

#ifdef POWERLINE_REVISION
    if((dev[DP_PHYSICALADDR].toString() == QString("FF:FF:FF:FF:FF:FF")) &&
            (dev[DP_IP].toString() == QString("0.0.0.0")))
    {
        PowerlineHandler *plh = PowerlineHandler::GetSingletonPtr();
        Q_ASSERT(plh);
        plh->launchUtitity();
        return;
    }
#endif
    viewDeviceDetails(dev);
}

//add by caiyujun
void NetworkMapView::onMouseInNode(bool isIn,int idx)
{
    if (isIn)
    {
        m_isInNode = true;
        m_currentNodeIndex = idx;
 
        Device devRecv;
        node2Device(idx,&devRecv);
        bool bOnLine = devRecv[DP_TURBOFLAG].toBool();

		if (devRecv[DP_ISLOCALHOST].toBool() || !bOnLine || !devRecv[DP_STATUS].toBool() || 1 == devRecv[DP_ATTACHDEVICE_BLOCKED].toInt())
		{
		    m_urls.clear();
		    return;
		}
		else
		{
			QString strFile;
			std::vector<std::wstring> vecNames;
			Device devSend;
			node2Device(0,&devSend);
			QString strDevName = devSend[DP_NAME].toString();

			QString recvDevName = devRecv[DP_NAME].toString();
			if ("" == recvDevName)
			{
				recvDevName = devRecv[DP_IP].toString();
			}
			QString strIp = devRecv[DP_IP].toString();
			QString strRecvType;
			if (m_ipToType.end() != m_ipToType.find(strIp))
			{
				strRecvType = m_ipToType.value(strIp);
			}
			else
			{
				strRecvType = "Windows";
			}

			foreach(QUrl url, m_urls)
			{
				QString strFile = url.toLocalFile();
				QFileInfo file(strFile);
				if (file.isDir())
				{
					int iType = m_topologyView->getDeviceType(devRecv[DP_IP].toString());
					if (!unsupportedSendFolderType(iType) && !unsupportedSendFolderType(devRecv[DP_TYPE].toInt()))
					{
						std::wstring wstrFile = strFile.toStdWString();
						m_fileNotifyManager->sendFolderFiles(strIp.toStdString().c_str(),7000,wstrFile.c_str(),strDevName.toStdString().c_str(),"Windows", recvDevName.toStdString().c_str(), strRecvType.toStdString().c_str());
					}
					else
					{
						continue ;
					}
				}
				else
				{
					std::wstring wstrFile = strFile.toStdWString();			
					vecNames.push_back(wstrFile.c_str());	
				}
			}
			m_urls.clear();
			if (0 != vecNames.size())
			{
				if (1 == vecNames.size())
				{
					m_fileNotifyManager->sendFile(strIp.toStdString().c_str(), 7000, vecNames.at(0).c_str(), strDevName.toStdString().c_str(), "Windows", recvDevName.toStdString().c_str(), strRecvType.toStdString().c_str());
				}
				else if (0 != vecNames.size())
				{
					m_fileNotifyManager->sendMultiFiles(strIp.toStdString().c_str(),7000,vecNames,strDevName.toStdString().c_str(), "Windows", recvDevName.toStdString().c_str(), strRecvType.toStdString().c_str());
				}
			}
		}
	}
    else
    {
        m_isInNode = false;
        m_currentNodeIndex = -1;
    }
}


#ifdef Q_OS_MAC
void NetworkMapView::socketData()
{
    if (NULL != client)
    {
        DNSServiceProcessResult(client);
    }

}

void NetworkMapView::socketData1()
{
    if (NULL != disclient)
    {
        DNSServiceProcessResult(disclient);
    }
}
#endif

void NetworkMapView::onNodeContextMenu(int idx,const QPoint &pos)
{
	Device dev;
	emit node2Device(idx,&dev);

#ifdef POWERLINE_REVISION
	if((dev[DP_PHYSICALADDR].toString() == QString("FF:FF:FF:FF:FF:FF")) &&
		(dev[DP_IP].toString() == QString("0.0.0.0")))
	{
		return;
	}
#endif

	qDebug() << "Network map view context menu idx = " << idx << "pos = " << pos;

	QList<int> items = dev.getDeviceMenuItems();
	BlockDeviceManager *bdm=BlockDeviceManager::GetSingletonPtr();

	if(items.size() != 0)
	{
		QMenu menu(this);
		QAction *act = NULL;

		if(items.contains(DMI_VIEWDETAIL))
		{
			act = menu.addAction(QIcon(GENIE2_RES("map/others/ViewDetails.png")),translateText(L_MAP_MENUITEM_DETAILS,QString("Details...")));
			act->setData(int(DMI_VIEWDETAIL));

			QString mac = dev[DP_PHYSICALADDR].toString();
			bool bResult = false;
			quint32 iNodeFlags = m_topologyView->getNodeFlag(mac,bResult);
			if (bResult)
			{
				if (!(iNodeFlags & TopologyNode::NF_ISLOCALHOST)
					&& (iNodeFlags & TopologyNode::NF_ISONLINE)
					&& (iNodeFlags & TopologyNode::NF_ISTURBOONLINE)
					&& !(iNodeFlags & TopologyNode::NF_BLOCKED))
				{
					act = menu.addAction(translateText(L_MAP_SEND_FILE));
					act->setData(int(DMI_SEND_FILE));

					int iType = m_topologyView->getDeviceType(dev[DP_IP].toString());
					if (!unsupportedSendFolderType(iType) && !unsupportedSendFolderType(dev[DP_TYPE].toInt()))
					{
						act = menu.addAction(translateText(L_MAP_SEND_FILE_FOLDER));
						act->setData(int(DMI_SEND_FOLDER));
					}
				}
			}
			else
			{
				if (false == dev[DP_ISLOCALHOST].toBool())
				{
					if (true == dev[DP_TURBOFLAG].toBool())
					{
						if (true == dev[DP_STATUS].toBool())
							//if (false == dev[DP_ISLOCALHOST].toBool() && onLine && dev[DP_STATUS].toBool())
						{
							act = menu.addAction(translateText(L_MAP_SEND_FILE));
							act->setData(int(DMI_SEND_FILE));

							int iType = m_topologyView->getDeviceType(dev[DP_IP].toString());
							if (!unsupportedSendFolderType(iType) && !unsupportedSendFolderType(dev[DP_TYPE].toInt()))
							{
								act = menu.addAction(translateText(L_MAP_SEND_FILE_FOLDER));
								act->setData(int(DMI_SEND_FOLDER));
							}
						}
					}
				}
			}

			/*if (MDT_REPEATER == dev[DP_TYPE].toInt())
			{
			QString strExtenderIp = dev[DP_IP].toString();
			unsigned long ulExtenderIp = inet_addr(strExtenderIp.toStdString().c_str());
			if (ulExtenderIp != mCurrentExtenderIp)
			{
			act = menu.addAction("Extender Map");
			act->setData(int(DMI_EXTENDER_MAP));
			}
			}

			if (MDT_ROUTER == dev[DP_TYPE].toInt() && m_bExtendMap)
			{
			act = menu.addAction("Network Map");
			act->setData(int(DMI_NETWORK_MAP));
			}*/
			//add end
		}

		if(items.contains(DMI_SETUP))
		{
			act = menu.addAction(QIcon(GENIE2_RES("map/others/Setup.png")),translateText(L_MAP_MENUITEM_SETUP,QString("Setting...")));
			act->setData(int(DMI_SETUP));
		}

		if(bdm && (
			bdm->state()==BLOCKDEVICE_FSM::FS_RUNNING
			|| bdm->state()==BLOCKDEVICE_FSM::FS_RUNNING_ADMIN)
			)
		{
			if(items.contains(DMI_BLOCK_ENABLE))
			{
				act = menu.addAction(QIcon(GENIE2_RES("map/others/BD_Enable.png")),translateText(L_MAP_BLOCKDEVICE_NOBLOCK,"Enable..."));
				act->setData(int(DMI_BLOCK_ENABLE));
			}

			if(items.contains(DMI_BLOCK_DISABLE))
			{
				act = menu.addAction(QIcon(GENIE2_RES("map/others/BD_Disable.png")),translateText(L_MAP_BLOCKDEVICE_BLOCK,"Disable..."));
				act->setData(DMI_BLOCK_DISABLE);
			}
		}
#ifdef USE_RDP
		if(items.contains(DMI_RDT) )
		{
			act =menu.addAction(QIcon(GENIE2_RES("map/others/Rdp_Launch.png")),"Remote Control");
			act->setData(int(DMI_RDT));
		}
#endif
#ifdef USE_PUTTY
		//for putty
		QList<QVariant> list;
		if(items.contains(DMI_PUTTY) /*&& QDir::current().exists("putty.exe")*/)
		{
			QMenu *submenu=menu.addMenu(QIcon(GENIE2_RES("map/others/Rdp_Launch.png")),"putty");
			QStringList templist=dev[DP_PUTTY_HINT].toStringList();
			foreach(QString hint,templist)
			{
				act=submenu->addAction(QString("connect session ")+hint);
				list.clear();
				list<<int(DMI_PUTTY)<<int(POT_SESSION)<<hint;
				act->setData(list);
			}
			QString ip=dev[DP_IP].toString();
			act=submenu->addAction(QString("connect ")+ip);
			list.clear();
			list<<int(DMI_PUTTY)<<int(POT_IP)<<ip;
			act->setData(list);
		}

		//for winscp
		if(items.contains(DMI_WINSCP))
		{
			list.clear();
			QMenu *submenu=menu.addMenu(QIcon(GENIE2_RES("map/others/Rdp_Launch.png")),"WinSCP");
			QStringList templist=dev[DP_WINSCP_HINT].toStringList();
			foreach(QString hint,templist)
			{
				act=submenu->addAction(QString("connect session ")+hint);
				list.clear();
				list<<int(DMI_WINSCP)<<(POT_SESSION)<<hint;
				act->setData(list);
			}
			QString ip=dev[DP_IP].toString();
			act=submenu->addAction(QString("connect ")+ip);
			list.clear();
			list<<int(DMI_WINSCP)<<int(POT_IP)<<ip;
			act->setData(list);
		}
#endif

		act = menu.exec(pos);

		if(act)
		{
			bool bok = false;
			int id = act->data().toInt(&bok);
			if(bok)
			{
				if(id == DMI_SETUP)
				{
					QDesktopServices::openUrl(QUrl(QString("http://") + dev[DP_IP].toString()));
					emit deviceSetupMenuItemTriggered();
				}
				else if(id == DMI_VIEWDETAIL)
				{
					viewDeviceDetails(dev);
				}
				//add by caiyujun
				else if(id == DMI_SEND_FILE)
				{
					QString strIp=dev[DP_IP].toString();

					if ("" == m_strLastOpenFolder)
					{
						emit queryDefaultSendFolder(&m_strLastOpenFolder);
					}

					OPENFILENAME ofn;
					TCHAR szOpenFileNames[80*MAX_PATH] = L"";		
					TCHAR szDir[MAX_PATH] = L"";
	
					ZeroMemory( &ofn, sizeof(ofn) );
					ofn.hwndOwner = HWND(winId());
					ofn.Flags = OFN_EXPLORER | OFN_ALLOWMULTISELECT | OFN_HIDEREADONLY | OFN_FILEMUSTEXIST;
					ofn.lStructSize = sizeof(ofn);
					ofn.lpstrFile =szOpenFileNames;
					ofn.nMaxFile = sizeof(szOpenFileNames);
					ofn.lpstrFile[0] = '\0';
					ofn.lpstrFilter = L"All Files(*.*)\0*.*\0";
					ofn.lpstrFileTitle = NULL;
					ofn.nMaxFileTitle = 0;

					if ("" != m_strLastOpenFolder)
					{
						std::wstring wstrDir = m_strLastOpenFolder.toStdWString();
						wmemcpy(szDir,wstrDir.c_str(),wcslen(wstrDir.c_str()));
						ofn.lpstrInitialDir = szDir;
					}
					else
					{
						ofn.lpstrInitialDir = NULL;
					}
					
					TCHAR   defaultDir[1024] = L"";
					TCHAR	path[1024] = L"";
					int		nLen = 0;
					std::vector<std::wstring> vecNames;
					if(GetOpenFileName(&ofn))
					{  
						wcsncpy_s(defaultDir, ofn.lpstrFile, ofn.nFileOffset);
						int nDirLength = wcslen(defaultDir);
						
						nLen = lstrlen(szOpenFileNames);

						if (szOpenFileNames[nDirLength])
						{
							vecNames.push_back(szOpenFileNames);
						}
						else
						{
							for (wchar_t *fname=szOpenFileNames+nLen+1; *fname; fname += wcslen(fname) +1)
							{
								if (MakePath(path, szOpenFileNames, fname) >= MAX_PATH)
								{
									continue;						
								}
								else
								{
									vecNames.push_back(path);
								}
							}
						}
					}

					if (!vecNames.empty())
					{
						m_strLastOpenFolder = WIN32_WC2CU(defaultDir);
						emit requestStoreDefaultSendFolder(m_strLastOpenFolder);

						Device device;
						node2Device(0,&device);
						QString strDevName = device[DP_NAME].toString();

						QString strRecvType;
						if (m_ipToType.end() != m_ipToType.find(strIp))
						{
							strRecvType = m_ipToType.value(strIp);
						}
						else
						{
							strRecvType = "Windows";
						}

						QString devName = dev[DP_NAME].toString();
						if ("" == devName)
						{
							devName = dev[DP_IP].toString();
						}

						if (1 == vecNames.size())
						{
							const wchar_t *strFile = vecNames.at(0).c_str();
							m_fileNotifyManager->sendFile(strIp.toStdString().c_str(), 7000, strFile, strDevName.toStdString().c_str(), "Windows", devName.toStdString().c_str(), strRecvType.toStdString().c_str());
						}
						else if (0 != vecNames.size())
						{
							m_fileNotifyManager->sendMultiFiles(strIp.toStdString().c_str(),7000,vecNames,strDevName.toStdString().c_str(), "Windows", devName.toStdString().c_str(),strRecvType.toStdString().c_str());
						}
					}
				}
				else if(id == DMI_SEND_FOLDER)
				{
					IMalloc	*iMalloc = NULL;	
					if (!SUCCEEDED(SHGetMalloc(&iMalloc)))
						return ;

					if ("" == m_strLastOpenFolder)
					{
						emit queryDefaultSendFolder(&m_strLastOpenFolder);
					}

					TCHAR szBuffer[MAX_PATH] = L"";
					BROWSEINFO bi;
					std::wstring strLastFolder = m_strLastOpenFolder.toStdWString();
					ZeroMemory(&bi,sizeof(BROWSEINFO));   
					bi.hwndOwner = HWND(winId());   
					bi.pidlRoot = 0;
					bi.pszDisplayName = szBuffer;
					bi.ulFlags = BIF_RETURNONLYFSDIRS;
					bi.lParam = (LPARAM)(strLastFolder.c_str());
					bi.lpfn = BrowseDirDlgProc;
					bi.iImage = 0;

					LPITEMIDLIST	pidlBrowse;
					BOOL ret = FALSE;
					if ((pidlBrowse = SHBrowseForFolder(&bi)) != NULL)
					{
						ret = SHGetPathFromIDList(pidlBrowse, szBuffer);
						iMalloc->Free(pidlBrowse);
						if (!ret)
						{
							iMalloc->Release();
							return;
						}
					}
					else
					{
						iMalloc->Release();
						return;
					}

					iMalloc->Release();

					m_strLastOpenFolder = WIN32_WC2CU(szBuffer);
					emit requestStoreDefaultSendFolder(m_strLastOpenFolder);

					Device device;
					node2Device(0,&device);
					QString strDevName = device[DP_NAME].toString();
					
					QString strIp=dev[DP_IP].toString();
					QString strRecvType;
					if (m_ipToType.end() != m_ipToType.find(strIp))
					{
						strRecvType = m_ipToType.value(strIp);
					}
					else
					{
						strRecvType = "Windows";
					}

					QString devName = dev[DP_NAME].toString();
					if ("" == devName)
					{
						devName = dev[DP_IP].toString();
					}
			
					m_fileNotifyManager->sendFolderFiles(strIp.toStdString().c_str(),7000,szBuffer,strDevName.toStdString().c_str(),"Windows", devName.toStdString().c_str(),strRecvType.toStdString().c_str());
				}
				/* else if(id == DMI_EXTENDER_MAP)
				{
				m_bExtendMap = true;
				QString strIp=dev[DP_IP].toString();
				unsigned long extenderIp = inet_addr(strIp.toStdString().c_str());
				mCurrentExtenderIp = extenderIp;
				emit subscribeDataRequest(m_netcardPanel->m_currentAdapterId,0,MAP_DEVICECOUNT_PERPAGE,m_bExtendMap,extenderIp);
				}
				else if(id == DMI_NETWORK_MAP)
				{
				m_bExtendMap = false;
				mCurrentExtenderIp = 0;
				emit subscribeDataRequest(m_netcardPanel->m_currentAdapterId,0,MAP_DEVICECOUNT_PERPAGE,m_bExtendMap,0);
				}*/
				else if(id == DMI_BLOCK_ENABLE)
				{
					bdm->enableDevice(dev[DP_PHYSICALADDR].toString());
				}
				else if(id ==DMI_BLOCK_DISABLE)
				{
					bdm->disableDevice(dev[DP_PHYSICALADDR].toString());
				}
#ifdef USE_RDP
				else if(id ==DMI_RDT)
				{
					OSVERSIONINFO info ;
					info.dwOSVersionInfoSize = sizeof (info) ;
					GetVersionEx (&info) ;
					QStringList mstsc_paras;
					QString adminpara=(info.dwMajorVersion<6)?"/console":"/admin";
					mstsc_paras << "/f" << adminpara<< QString("/v:%1").arg(dev[DP_IP].toString());
					QProcess::startDetached("mstsc",mstsc_paras);
				}
#endif
			}
#ifdef USE_PUTTY
			else
			{
				QList<QVariant> list=act->data().toList();
				if(list.count()>=3 )
				{
					bool bok;
					int cmd=list[0].toInt(&bok);
					if(bok)
					{
						if(DMI_PUTTY == cmd)
						{
							startPutty(list[1].toInt(),list[2].toString());
						}
						else if(DMI_WINSCP == cmd)
						{
							startWSCP(list[1].toInt(),list[2].toString());
						}
					}
				}
			}
#endif
		}
		
	}
}

void NetworkMapView::onViewDetailsOver(const Device &dev , bool modified)
{
    m_topologyView->setEnabled(true);

    qDebug () << "View device details over ,data is"
              << (modified ? "modified" : "not modified");

    if(modified)
    {
        emit deviceDataChanged(dev);
    }

    delete m_ddvm;
    m_ddvm = 0;

}

void NetworkMapView::onNewData(const QList<Device *> &data)
{

    m_topologyView->onNewData(data);

    //    if(m_ddvm)
    //    {
    //        m_ddvm->onNewData(data);
    //    }

    m_beingWaitting = (data.size() == 0);
    updateUiElements();
}

void NetworkMapView::onInternetStateChanged(bool connected)
{
    qDebug() << "Network map view on internet state changed state = " << connected;

    m_topologyView->onInternetStateChanged(connected);

#ifdef ENABLE_POWERLINE
    m_powerlinePage->notifyInternetState(connected);
#endif
}

void NetworkMapView::onSendSuccess()
{
    m_urls.clear();;
}

void NetworkMapView::removeService(QString strServiceName, QString strIp)
{
	mutex.lock();

//	QMap<QString,QString>::iterator it1 = m_onLineHostToIp.find(strServiceName);
	bool bExist = false;
	QString serviceIp;
	QVector<TurboService>::iterator itTurbo = m_vecTurboSrv.begin();
	for(itTurbo; itTurbo!=m_vecTurboSrv.end(); itTurbo++)
	{
		if ("" != strIp)
		{
			if(itTurbo->strService == strServiceName &&
				itTurbo->strIp == strIp)
			{
				serviceIp = strIp;
				m_vecTurboSrv.erase(itTurbo);
				bExist = true;
				break;
			}	
		}
		else
		{
			if(itTurbo->strService == strServiceName &&
				itTurbo->bBonjour == true)
			{
				serviceIp = itTurbo->strIp;
				m_vecTurboSrv.erase(itTurbo);
				bExist = true;
				break;
			}
		}
	}

	if (bExist)
	{
		QString ip = serviceIp;

		Device dev;
		emit ip2Device(serviceIp,&dev);
		dev[DP_TURBOFLAG] = false;

		std::vector<std::string>::iterator it = m_onLineServiceIp.begin();
		for (it; it != m_onLineServiceIp.end(); it++)
		{
			if (*it == ip.toStdString())
			{
				m_onLineServiceIp.erase(it);
				break;
			}
		}

		if (m_ipToType.end() != m_ipToType.find(serviceIp))
		{
			m_ipToType.remove(ip);
		}

		m_topologyView->onTurboStateChanged(&dev,serviceIp,false);
	}
	mutex.unlock();
}

void NetworkMapView::addService(QString strIp, QString strServiceName, bool bBonjour)
{
	mutex.lock();
	bool mTypeChanged = false;

	std::vector<std::string>::iterator it = m_onLineServiceIp.begin();
	bool bExist = false;
	for (it; it != m_onLineServiceIp.end(); it++)
	{
		if (*it == strIp.toStdString())
		{
			bExist = true;
			break;
		}
	}

	if (!bExist)
	{
		Device dev;
		emit ip2Device(strIp,&dev);

		if (!dev[DP_ISLOCALHOST].toBool())
		{
			m_onLineServiceIp.push_back(strIp.toStdString());
			//pUdt->TTSPing(m_onLineServiceIp);

			QStringList strList = strServiceName.split("[");
			int iCount = strList.count();
			if (iCount > 1)
			{
				QString strName = strList.at(iCount-1);
				QStringList strTempList = strName.split("]");
				int iTempCount = strTempList.count();
				if (iTempCount == 2)
				{
					QString strType = strTempList.at(0);
					qDebug() << "--------find device type:" << strType;
					QMap<QString,int>::iterator it = m_devNameToType.find(strType);
					if (it != m_devNameToType.end())
					{
						qDebug() << "type changed";
						dev[DP_TYPE] = it.value();
						mTypeChanged = true;
					}

					if (m_ipToType.end() == m_ipToType.find(strIp))
					{
						m_ipToType.insert(strIp,strType);
					}

					TurboService turboService;
					bool bExistService = false;
					QVector<TurboService>::iterator itTurbo = m_vecTurboSrv.begin();
					for(itTurbo; itTurbo!=m_vecTurboSrv.end(); itTurbo++)
					{
						if(itTurbo->strService == strServiceName &&
							itTurbo->strIp == strIp)
						{		
							bExistService = true;
							if(bBonjour)
							{
								itTurbo->bBonjour = true;						
								break;
							}
							
						}
					}
					if (!bExistService)
					{
						turboService.strService = strServiceName;
						turboService.strIp = strIp;
						if(bBonjour)
						{
							turboService.bBonjour = true;
						}
						m_vecTurboSrv.push_back(turboService);
					}
					

					/*if (!m_onLineHostToIp.contains(strServiceName))
					{
						m_onLineHostToIp.insert(strServiceName,strIp);
					}*/
				}
			}

			dev[DP_TURBOFLAG] = true;
			if (mTypeChanged)
			{
				//emit bonjourTypeChanged(dev);
				m_topologyView->onTurboStateChanged(&dev,strIp,true,dev[DP_TYPE].toInt());
			}
			else
			{
				m_topologyView->onTurboStateChanged(&dev,strIp,true);
			}

		}	
	}
	else
	{
		Device dev;
		emit ip2Device(strIp,&dev);

		QStringList strList = strServiceName.split("[");
		int iCount = strList.count();
		if (iCount > 1)
		{
			QString strName = strList.at(iCount-1);
			QStringList strTempList = strName.split("]");
			int iTempCount = strTempList.count();
			if (iTempCount == 2)
			{
				QString strType = strTempList.at(0);
				qDebug() << "--------find device type:" << strType;
				QMap<QString,int>::iterator it = m_devNameToType.find(strType);
				if (it != m_devNameToType.end())
				{
					qDebug() << "type changed";
					dev[DP_TYPE] = it.value();
					mTypeChanged = true;
				}

				if (m_ipToType.end() == m_ipToType.find(strIp))
				{
					m_ipToType.insert(strIp,strType);
				}

				/*if (!m_onLineHostToIp.contains(strServiceName))
				{
				m_onLineHostToIp.insert(strServiceName,strIp);
				}*/
				TurboService turboService;
				bool bExistService = false;
				QVector<TurboService>::iterator itTurbo = m_vecTurboSrv.begin();
				for(itTurbo; itTurbo!=m_vecTurboSrv.end(); itTurbo++)
				{
					if(itTurbo->strService == strServiceName &&
						itTurbo->strIp == strIp)
					{		
						bExistService = true;
						if(bBonjour)
						{
							itTurbo->bBonjour = true;								
							break;
						}

					}
				}
				if (!bExistService)
				{
					turboService.strService = strServiceName;
					turboService.strIp = strIp;
					if(bBonjour)
					{
						turboService.bBonjour = true;
					}
					m_vecTurboSrv.push_back(turboService);
				}
			}
		}

		dev[DP_TURBOFLAG] = true;
		if (mTypeChanged)
		{
			//emit bonjourTypeChanged(dev);
			m_topologyView->onTurboStateChanged(&dev,strIp,true,dev[DP_TYPE].toInt());
		}
	}
	mutex.unlock();
}

void NetworkMapView::onDlnaChange(QString strIp,QString name,int type)
{
	if (0 == type)
	{
		qDebug() << "dlna add service:" << name << " service ip:" << strIp;
		addService(strIp,name);
	}
	else
	{
		qDebug() << "dlna remove service:" << name << " service ip:" << strIp;
		removeService(name,strIp);
	}
}

void NetworkMapView::onRemoveService(QString strServiceName)
{
    qDebug() << "bonjour remove service:" << strServiceName;

	removeService(strServiceName);
	
}

void NetworkMapView::onAddService(QString strServiceName, QString strIp)
{
	qDebug() << "bonjour add service:" << strServiceName << " service ip:" << strIp;

	addService(strIp, strServiceName, true);
}

void NetworkMapView::onBonjourTypeChanged(const Device &dev)
{
    emit bonjourTypeChanged(dev);
}

//add end

void NetworkMapView::onInfInformation(SVT_NotifyType data)
{
    //net cards panel update
    m_netcardPanel->onNetcardsChanged(data);
    //add by caiyujun
    if (m_howlpublish && m_howlpublish->cancelPublish())
    {
        Sleep(1000);
        m_howlpublish->start();
    }
    //add end
}

void NetworkMapView::onDeviceCountChanged(int count)
{
    m_devTotalCount = count;
    m_pageTotalCount = (m_devTotalCount <= 2 ? 1 : (m_devTotalCount - 1 - 2) / MAP_DEVICECOUNT_PERPAGE + 1);

    if(m_currentPageIndex > m_pageTotalCount)
    {
        m_currentPageIndex = m_pageTotalCount;
        //subscribeCurrentPageData();
        processPageing();
    }

    m_beingWaitting = (m_devTotalCount == 0);
    retranslateUi();
    updateUiElements();

}

void NetworkMapView::onOtherDeviceCountChanged(QString deviceIp,int count)
{
		Device dev;
		bool bShowDevCount = true;
		emit ip2Device(deviceIp,&dev);
		if (m_bExtendMap)
		{
			QString strIp = dev[DP_IP].toString();
			unsigned long ulIp = inet_addr(strIp.toStdString().c_str());
			if (mCurrentExtenderIp == ulIp)
			{
				bShowDevCount = false;
			}
		}
		else
		{
			if (dev[DP_ISDEFAULTGATEWAY].toBool())
			{
				bShowDevCount = false;
			}
		}
		m_topologyView->otherDevCountChanged(&dev,count,bShowDevCount);
}


void NetworkMapView::refresh(bool nic_changed)
{
    m_devTotalCount     = 0;

    m_pageTotalCount    = 0;
    m_currentPageIndex  = 1;

    //add by caiyujun
    //if (NULL != client)
    //{
        //DNSServiceRefDeallocate(client);
        //m_onLineServiceToHost.clear();
        //m_onLineHostToIp.clear();
        //pthis->m_topologyView->clearTurboOnlineIp();
        //test();
    //}
    //add end
qDebug()<<"NetworkMapView::refresh(bool nic_changed)";
    //subscribe first page data
	//modify by caiyujun
    //emit subscribeDataRequest(m_netcardPanel->m_currentAdapterId,0,MAP_DEVICECOUNT_PERPAGE);
	m_bExtendMap = false;
	mCurrentExtenderIp = 0;
	emit subscribeDataRequest(m_netcardPanel->m_currentAdapterId,0,MAP_DEVICECOUNT_PERPAGE,m_bExtendMap,0);
	//modify end

	dlnaProxy->refreshDevices(DLNACore::FlushMode_All);

    if(!nic_changed)
    {
        emit refreshRequest();
    }


}

void NetworkMapView::btnClicked()
{
    QPushButton *btn = qobject_cast<QPushButton*>(sender());
#ifdef ENABLE_POWERLINE
    if(btn == m_prevPageBtn)
    {
        if(!m_plcPagingNow){
            if(m_currentPageIndex > 1)
            {
                --m_currentPageIndex;
                processPageing();
            }
        }else{
            m_powerlinePage->prevPage();
        }
    }
    else if(btn == m_nextPageBtn)
    {
        if(!m_plcPagingNow){
            if(m_currentPageIndex < m_pageTotalCount)
            {
                ++m_currentPageIndex;
                processPageing();
            }
        }else{
            m_powerlinePage->nextPage();
        }

    }
    else if(btn == m_firstPageBtn)
    {
        if(!m_plcPagingNow){
            if(m_pageTotalCount > 0)
            {
                m_currentPageIndex = 1;
                processPageing();
            }
        }else{
            m_powerlinePage->firstPage();
        }
    }
    else if(btn == m_lastPageBtn)
    {
        if(!m_plcPagingNow){
            if(m_pageTotalCount > 0)
            {
                m_currentPageIndex = m_pageTotalCount;
                processPageing();
            }
        }else{
            m_powerlinePage->lastPage();
        }

    }
#else
    if(btn == m_prevPageBtn)
    {
        if(m_currentPageIndex > 1)
        {
            --m_currentPageIndex;
            processPageing();
        }
    }
    else if(btn == m_nextPageBtn)
    {
        if(m_currentPageIndex < m_pageTotalCount)
        {
            ++m_currentPageIndex;
            processPageing();
        }
    }
    else if(btn == m_firstPageBtn)
    {
        if(m_pageTotalCount > 0)
        {
            m_currentPageIndex = 1;
            processPageing();
        }
    }
    else if(btn == m_lastPageBtn)
    {
        if(m_pageTotalCount > 0)
        {
            m_currentPageIndex = m_pageTotalCount;
            processPageing();
        }
    }
#endif

    updateUiElements();
    retranslateUi();
}

void NetworkMapView::processPageing()
{
    subscribeCurrentPageData();
}


void NetworkMapView::onRefreshClicked()
{
#ifdef ENABLE_POWERLINE
    if(!m_powerlinePageValid || (m_netmapStackLayout->currentWidget() != m_powerlinePage)){
        m_beingWaitting = true;
        refresh(false);
    }

    if(!m_powerlinePageValid || (m_netmapStackLayout->currentWidget() == m_powerlinePage)){
        m_powerlinePage->refresh();
    }
#else
    m_beingWaitting = true;
    refresh(false);
#endif

}

void NetworkMapView::currrentNetcardChanged(int idx)
{
    qDebug () << "NetworkMapView currrentNetcardChanged id = " << idx;

    refresh(true);
    storeDefaultInterface();

    m_beingWaitting = true;

#ifdef ENABLE_POWERLINE
    m_netmapStackLayout->setCurrentWidget(m_mapPage);
    retranslateUi();
#endif

    updateUiElements();
}

void NetworkMapView::escKeyActived()
{
    if(m_ddvm)
    {
        m_ddvm->requestViewDeviceDetailsOver();
    }
    BlockDeviceManager *bdm=BlockDeviceManager::GetSingletonPtr();
    if(bdm)
    {
        bdm->keyEscape();
    }
}
#ifdef ENABLE_POWERLINE
void NetworkMapView::onPowerlineRefreshOver()
{
    if(m_powerlinePageValid != m_powerlinePage->dataAvailable()){

        m_powerlinePageValid = m_powerlinePage->dataAvailable();

        m_netcardPanel->setPLCBtnUsible(m_powerlinePageValid);

        retranslateUi();

        updateUiElements();
    }
}

void NetworkMapView::onGo2PLCPage(bool go)
{
    bool canGo = (go && m_powerlinePageValid);
    m_netmapStackLayout->setCurrentWidget(canGo ? m_powerlinePage : m_mapPage);


    if(canGo){
        m_powerlinePage->notifyRaise();
    }else{
        m_powerlinePage->notifyLower();
    }

    retranslateUi();
    updateUiElements();

    if(!canGo){
        subscribeCurrentPageData();
    }
}

void NetworkMapView::onBackClicked()
{
    m_powerlinePage->hideHubView();
    m_netcardPanel->setBackBtnUsible(false);

    m_plcPagingNow = false;

    updateUiElements();
    retranslateUi();
}

void NetworkMapView::onPLCPageChanged(int count)
{
    m_netcardPanel->setBackBtnUsible(count > 0);
    m_plcPagingNow = (count > 0);

    updateUiElements();
    retranslateUi();
}

#endif

void NetworkMapView::stateChanged(int state)
{
    emit newDeviceNotifyOptionChanged(state == Qt::Checked ? 1 : 0);
}

void NetworkMapView::subscribeCurrentPageData()
{
    Q_ASSERT(m_currentPageIndex >= 1 && m_currentPageIndex <= m_pageTotalCount);

    int begin = ((m_currentPageIndex - 1) * MAP_DEVICECOUNT_PERPAGE);

    int count = MAP_DEVICECOUNT_PERPAGE;

    qDebug () << "Subcribe adapter = " << m_netcardPanel->m_currentAdapterId
              << "from = " << begin << "count = " << count;

	//modify by caiyujun
	//emit subscribeDataRequest(m_netcardPanel->m_currentAdapterId,begin,count);
    emit subscribeDataRequest(m_netcardPanel->m_currentAdapterId,begin,count, m_bExtendMap, mCurrentExtenderIp);
	//modify end
}

void NetworkMapView::viewDeviceDetails(const Device &dev)
{
    m_topologyView->setEnabled(false);

    if(!m_ddvm)
    {
        m_ddvm = new DeviceDetailViewManager(this);
        connect(m_ddvm,SIGNAL(viewDetailsOver(Device,bool)),
                this,SLOT(onViewDetailsOver(Device,bool)));
        connect(m_ddvm,SIGNAL(translateText(int,QString*)),
                this,SIGNAL(translateText(int,QString*)));
        connect(m_ddvm,SIGNAL(requestRouterImagePath(QString,int,QString*)),
                this,SIGNAL(requestRouterImagePath(QString,int,QString*)));
    }

    m_ddvm->viewDeviceDetails(dev);
}

QWidget* NetworkMapView::createPageBtnsAndLabelPanel()
{
    m_prevPageBtn = new QPushButton(this);
    m_nextPageBtn = new QPushButton(this);

    m_firstPageBtn = new QPushButton(this);
    m_lastPageBtn  = new QPushButton(this);
    m_prevPageBtn->setFlat(true);
    m_nextPageBtn->setFlat(true);

    m_firstPageBtn->setFlat(true);
    m_lastPageBtn->setFlat(true);



    /********2012-3-19 add*************/
    m_prevPageBtn->setObjectName("prevPageBtn");
    m_nextPageBtn->setObjectName("nextPageBtn");
    m_firstPageBtn->setObjectName("firstPageBtn");
    m_lastPageBtn->setObjectName("lastPageBtn");
    /**************END****************/

    m_pagesLabel  = new QLabel(this);

    m_pagesLabel->setAlignment(Qt::AlignCenter);

    m_prevPageBtn->setFocusPolicy(Qt::NoFocus);
    m_nextPageBtn->setFocusPolicy(Qt::NoFocus);

    m_firstPageBtn->setFocusPolicy(Qt::NoFocus);
    m_lastPageBtn->setFocusPolicy(Qt::NoFocus);

    m_prevPageBtn->setFlat(true);
    m_nextPageBtn->setFlat(true);
    m_firstPageBtn->setFlat(true);
    m_lastPageBtn->setFlat(true);

    connect(m_prevPageBtn,SIGNAL(clicked()),this,SLOT(btnClicked()));
    connect(m_nextPageBtn,SIGNAL(clicked()),this,SLOT(btnClicked()));
    connect(m_firstPageBtn,SIGNAL(clicked()),this,SLOT(btnClicked()));
    connect(m_lastPageBtn,SIGNAL(clicked()),this,SLOT(btnClicked()));


    m_prevPageBtn->installEventFilter(this);
    m_nextPageBtn->installEventFilter(this);

    m_firstPageBtn->installEventFilter(this);
    m_lastPageBtn->installEventFilter(this);

    QWidget *panel = new QWidget(this);
    QHBoxLayout *layOut = new QHBoxLayout(panel);
    layOut->setSpacing(2);
    layOut->setContentsMargins(0,0,0,0);

    layOut->addWidget(m_firstPageBtn);
    layOut->addWidget(m_prevPageBtn);
    layOut->addWidget(m_pagesLabel);
    layOut->addWidget(m_nextPageBtn);
    layOut->addWidget(m_lastPageBtn);

    retranslateImages();

    return panel;
}


void NetworkMapView::updateUiElements()
{
#ifdef ENABLE_POWERLINE
    if(m_plcPagingNow){
        m_pageNavPanel->setVisible(m_powerlinePage->pageCount() >= 2);
    }else{
        m_pageNavPanel->setVisible(m_pageTotalCount >= 2 && m_currentPageIndex >= 1 && (m_netmapStackLayout->currentWidget() != m_powerlinePage));
    }
#else
    m_pageNavPanel->setVisible(m_pageTotalCount >= 2 && m_currentPageIndex >= 1);
#endif

#ifdef ENABLE_POWERLINE
    if(m_plcPagingNow){
        m_prevPageBtn->setEnabled(!m_powerlinePage->isCurFisrtPage());
        m_nextPageBtn->setEnabled(!m_powerlinePage->isCurLastPage());

        m_firstPageBtn->setEnabled(!m_powerlinePage->isCurFisrtPage());
        m_lastPageBtn->setEnabled(!m_powerlinePage->isCurLastPage());
    }else{
        m_prevPageBtn->setEnabled(m_currentPageIndex > 1);
        m_nextPageBtn->setEnabled(m_currentPageIndex < m_pageTotalCount);

        m_firstPageBtn->setEnabled(m_currentPageIndex > 1);
        m_lastPageBtn->setEnabled(m_currentPageIndex < m_pageTotalCount);
    }
#else
    m_prevPageBtn->setEnabled(m_currentPageIndex > 1);
    m_nextPageBtn->setEnabled(m_currentPageIndex < m_pageTotalCount);

    m_firstPageBtn->setEnabled(m_currentPageIndex > 1);
    m_lastPageBtn->setEnabled(m_currentPageIndex < m_pageTotalCount);
#endif


#ifdef ENABLE_POWERLINE
    if(m_beingWaitting){
        m_mapPageLayout->setCurrentWidget(m_waittingPanel);
        m_waittingPanel->start();
    }else{
        m_mapPageLayout->setCurrentWidget(m_topologyView);
        m_waittingPanel->stop();
    }

    if(!m_powerlinePageValid || /*m_currentPageIndex != 1*/(m_netmapStackLayout->currentWidget() != m_powerlinePage)){
    }else{
        m_topologyView->hideAllNodes();
    }

    m_notifyNewDevCB->setVisible(!m_powerlinePageValid || /*m_currentPageIndex != 1*/(m_netmapStackLayout->currentWidget() != m_powerlinePage));

#else
    if(m_beingWaitting){
        m_netmapStackLayout->setCurrentWidget(m_waittingPanel);
        m_waittingPanel->start();
    }else{
        m_netmapStackLayout->setCurrentWidget(m_topologyView);
        m_waittingPanel->stop();
    }
#endif

    if(m_ddvm)
    {
        m_ddvm->raise();
    }
    BlockDeviceManager *bdm=BlockDeviceManager::GetSingletonPtr();
    if(bdm)
    {
#ifdef ENABLE_POWERLINE
        bdm->UpdateMiniOptUiVisible(!((m_powerlinePageValid && /*(m_currentPageIndex == 1)*/(m_netmapStackLayout->currentWidget() == m_powerlinePage)) || m_beingWaitting));
#else
        bdm->UpdateMiniOptUiVisible(!m_beingWaitting);
#endif
        bdm->raiseLoginFrame();
    }
}


#ifdef ENABLE_POWERLINE
void NetworkMapView::notifyRaise() const
{
    qDebug () << "NetworkMapView::notifyRaise()";
    if(m_netmapStackLayout->currentWidget() == m_powerlinePage){
        m_powerlinePage->notifyRaise();
    }
}

void NetworkMapView::notifyLower() const
{
    m_powerlinePage->notifyLower();
}
#endif

bool NetworkMapView::eventFilter(QObject *obj, QEvent *event)
{
    if ((obj == m_prevPageBtn) || (obj == m_nextPageBtn) ||
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
    }

    return QWidget::eventFilter(obj, event);

}

void NetworkMapView::addBlockDeviceControlPanel(QWidget *ctrPanel)
{
    Q_ASSERT(m_layOut && ctrPanel);

    if(m_layOut->indexOf(ctrPanel) != -1){
        return;
    }
#ifdef Q_OS_MAC
    m_layOut->setSpacing(2);
#endif
    m_layOut->insertWidget(2,ctrPanel);
    updateGeometry();
}
