#ifndef NETWORKMAPVIEW_H
#define NETWORKMAPVIEW_H

#include <QWidget>
#include <QList>
#include "topology.h"
//add by caiyujun
#include "udt.h"
#include "api.h"
#include "dns_sd.h"
#include <QVector>
#include <QMutex>

//add end

class Device;
class TopologyView;
class DeviceDetailViewManager;
class NetcardsPanel;
class WaittingPanel;

class UDTProxy;
class Browser;
class howlpublish;
class dnsservice;
class FileNotifyManager;
class DLNAProxy;

struct TurboService
{
	QString strService;
	QString strIp;
	bool bBonjour;

	TurboService()
	{
		bBonjour = false;
	}
};


#ifdef ENABLE_POWERLINE
class PowerLinePage;
#endif

class QLabel;
class QPushButton;
class QCheckBox;
class QComboBox;
class QStackedLayout;
class QVBoxLayout;


class NetworkMapView : public QWidget
{
    friend class NetcardsPanel;

    Q_OBJECT
private:
    enum NavBtnState
    {
        BTNSTATE_NORMAL = 1 << 0,
        BTNSTATE_HOVER = 1 << 1,
        BTNSTATE_DISABLED = 1 << 2
    };

public:
    explicit NetworkMapView(QWidget *parent = 0);
    //add by caiyujun
    //qint64 countSize(QString filePath);

//    static void HandleEvents();
//    static void DNSSD_API zonedata_browse(DNSServiceRef sdref, const DNSServiceFlags flags, uint32_t ifIndex, DNSServiceErrorType errorCode,
//            const char *replyName, const char *replyType, const char *replyDomain, void *context);
//    static void DNSSD_API reg_reply(DNSServiceRef service, DNSServiceFlags flags, DNSServiceErrorType errCode,
//                                                  const char* name, const char* regtype, const char* domain, void* context);
//    static void DNSSD_API zonedata_resolve(DNSServiceRef sdref, const DNSServiceFlags flags, uint32_t ifIndex, DNSServiceErrorType errorCode,
//                                          const char *fullname, const char *hosttarget, uint16_t opaqueport, uint16_t txtLen, const unsigned char *txt, void *context);
//    static void DNSSD_API addrinfo_reply(DNSServiceRef sdref, DNSServiceFlags flags, uint32_t interfaceIndex, DNSServiceErrorType errorCode, const char *hostname, const struct sockaddr *address, uint32_t ttl, void *context);
    void test();
    void setExit();
    void initDeviceNameToType();
    bool unsupportedSendFolderType(int iType);
    //add end
    ~NetworkMapView();
#ifdef USE_NODE_ANIMATION
#ifdef REBOUNCE_FOR_RAISE	
    void reBounce();
    void fadeoutAll();
#endif
#endif

#ifdef ENABLE_POWERLINE
    void notifyRaise() const;
    void notifyLower() const;
#endif

protected:
    TopologyView                *m_topologyView;
    DeviceDetailViewManager     *m_ddvm;

    QLabel              *m_devNumberLabel;
    QPushButton         *m_refreshBtn;
    QCheckBox           *m_notifyNewDevCB;

    QPushButton         *m_prevPageBtn;
    QPushButton         *m_nextPageBtn;
    QPushButton         *m_firstPageBtn;
    QPushButton         *m_lastPageBtn;

    QLabel              *m_pagesLabel;
    QWidget             *m_pageNavPanel;
    NetcardsPanel       *m_netcardPanel;
    WaittingPanel       *m_waittingPanel;

#ifdef ENABLE_POWERLINE
    QWidget             *m_mapPage;
    PowerLinePage       *m_powerlinePage;

    QStackedLayout      *m_mapPageLayout;
#endif

    QStackedLayout      *m_netmapStackLayout;

    int                 m_prevPageBtnState;
    int                 m_nextPageBtnState;
    int                 m_firstPageBtnState;
    int                 m_lastPageBtnState;

    QVBoxLayout         *m_layOut;

private:
    QWidget *createPageBtnsAndLabelPanel();
    void updateUiElements();
    void subscribeCurrentPageData();
    void viewDeviceDetails(const Device &dev);
protected:
    int                 m_devTotalCount;
    int                 m_pageTotalCount;
    int                 m_currentPageIndex;
    bool                m_beingWaitting;
    //add by caiyujun
    int                 m_currentNodeIndex;
    QList<QUrl>         m_urls;
    bool                m_isInNode;
    //add end

#ifdef ENABLE_POWERLINE
    bool                m_powerlinePageValid;
    bool                m_plcPagingNow;
#endif

protected:
    virtual bool eventFilter(QObject *obj, QEvent *event);
    virtual void changeEvent(QEvent *);
    virtual void showEvent(QShowEvent *);

    //add by caiyujun
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);
    //add end

private:
    void checkNewDeviceNotifyCB();
    void applyUiElementsStyle();
    void retranslateUi();
    void retranslateImages();

    QString translateText(int idx, const QString &defaultText = QString());

    void refresh(bool nic_changed);

private:
    int getDefaultInterface();
    void storeDefaultInterface();

	void removeService(QString strServiceName, QString strIp = "");
	void addService(QString strIp, QString strServiceName, bool bBonjour = false);
signals:
    void languageChanged();

    void queryDefaultInterface(QString *infIdx);    
    void requestStoreDefaultInterface(const QString &infIdx);

	//add by caiyujun
	void queryDefaultRecvFolder(QString *folder);
	void requestStoreDefaultRecvFolder(const QString &folder);

	void queryDefaultSendFolder(QString *folder);
	void requestStoreDefaultSendFolder(const QString &folder);
	//add end


    void queryNewDeviceNotifyOption(int *option);
    void newDeviceNotifyOptionChanged(int option);

#ifdef ENABLE_POWERLINE
    void queryPLCDevicePassword(const QString &devMac, QString *pwd);
    void storePLCDevicePassword(const QString &devMac, const QString &pwd);
#endif

    void translateText(int idx ,QString *text);
    void requestRouterImagePath(const QString &model,int imageRole,QString *imagePath);


signals:
    void node2Device(int idx , Device *dev);
    //add by caiyujun
    void ip2Device(QString strIp, Device *device);
    void sendFileTriggered();
    void sendFolderTriggered();
    void bonjourTypeChanged(const Device &dev);
    //add end
    void deviceDataChanged(const Device &dev);
    void refreshRequest();
	//modify by caiyujun
    void subscribeDataRequest(int adpIdx,int startIdx,int count,bool bExtendMap,unsigned long extenderIp);
	//modify end


    void deviceSetupMenuItemTriggered();
private slots:
    void onNewData(const QList<Device *> &data);

    void onNodeClicked(int idx);
    void onNodeContextMenu(int idx,const QPoint &pos);
    //add by caiyujun
    void onMouseInNode(bool,int idx);
    //add end

    void onViewDetailsOver(const Device &dev , bool modified);

    void onInfInformation(SVT_NotifyType data);
    void onDeviceCountChanged(int count);
	//add by caiyujun
	void onOtherDeviceCountChanged(QString deviceIp,int count);
	//add end

    void btnClicked();
    void onRefreshClicked();
    void stateChanged(int state);

    void currrentNetcardChanged(int idx);

    void escKeyActived();

#ifdef Q_OS_MAC
    //add by caiyujun
private slots:
    void socketData();
    void socketData1();
    //add end
#endif


#ifdef ENABLE_POWERLINE
    void onPowerlineRefreshOver();
    void onGo2PLCPage(bool go);
    void onBackClicked();
    void onPLCPageChanged(int count);
#endif

public slots:
    void onInternetStateChanged(bool connected);
    friend class BlockDeviceManager;
    //add by caiyujun
    void onSendSuccess();
    void onRemoveService(QString strServiceName);
    void onAddService(QString strServiceName, QString strIp);
	void onDlnaChange(QString strIp, QString name, int ip);
    void onBonjourTypeChanged(const Device &dev);
    //add end

private:
    void addBlockDeviceControlPanel(QWidget *ctrPanel);
    void processPageing();

    QList<QString> m_fileList;
    QList<QString> m_emptyFolderList;
    QMap<QString,int> m_devNameToType;
	QMap<QString,QString> m_ipToType;
    QVector<int> m_unableSendFolderType;
    QString m_strLastOpenFolder;
#ifdef Q_OS_MAC
    static QMap<QString,QString> m_onLineServiceToHost;
	static std::vector<std::string> m_onLineServiceIp;
	static NetworkMapView *pthis;
#else ifdef Q_OS_WIN
	QMap<QString,QString> m_onLineServiceToHost;
	std::vector<std::string> m_onLineServiceIp;
#endif
    Browser *m_browser;
    howlpublish *m_howlpublish;
    dnsservice *m_service;
	bool m_bExtendMap;
	unsigned long mCurrentExtenderIp;
	UDTProxy * pUdt;
	FileNotifyManager * m_fileNotifyManager;
	DLNAProxy *dlnaProxy;
	QVector<TurboService> m_vecTurboSrv;
	QMutex mutex;
};

#endif // NETWORKMAPVIEW_H
