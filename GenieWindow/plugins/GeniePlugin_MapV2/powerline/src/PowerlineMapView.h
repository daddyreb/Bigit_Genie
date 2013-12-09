#ifndef NETWORKMAP_VIEW_H
#define NETWORKMAP_VIEW_H

#include <QWidget>
#include <QList>

#include "DialogMgr.h"
#include "Singleton.h"


class PLDevice;
class PowerlineView;
class PowerlineNode;
//class NetcardsPanel;
class WaitPanel;


class QLabel;
class QPushButton;
class QCheckBox;
class QComboBox;
class QStackedLayout;
class QVBoxLayout;
class Powerline_HubMapView;
//class DeviceDetailViewManager;


class PowerlineMapView : public QWidget, public Singleton<PowerlineMapView>
{
    Q_OBJECT
public:
    PowerlineMapView(QWidget *parent = 0);
    ~PowerlineMapView(void);


    void showSecurity();
    bool refresh();
    bool haveData();
    inline bool isShow(){
        if(m_nShowState == SS_SHOW)
            return true;
        else
            return false;
    };
    inline bool isTipShowed()
    {
        if(m_TipShowState == SDW_HAVE_SHOW)
        {
            return true;
        }
        else
        {
            return false;
        }

    };
    inline bool isWirelessShowed()
    {
        if(m_WirelessShowState == SDW_HAVE_SHOW)
        {
            return true;
        }
        else
        {
            return false;
        }
    };
    inline void initShowState(bool b = true)
    {
//        m_nShowState = SS_HIDE;
//        m_TipShowState = SDW_NO_SHOW;
        if(b)
            m_WirelessShowState = SDW_HAVE_SHOW;
        else
        {
            m_WirelessShowState = SDW_NO_SHOW;
        }
    }

    void notifyRaise();
    void notifyLower();
	void notifyInternetState(bool connected);

	void showHubView(PowerlineNode *node, PowerlineEdge *edge, const QList<QPair<QString,QString> > &lstUsbDev);
	void hideHubView();
	void pageBtnClick(int);
	bool isRouterOk() const;


	/////////////////////////////////////////////////////////
//	void  insertKey(QList<QPair<QString, QString> > &);
//	QString getKey(QString &mac);
//private:
//	void readIni();
//	void saveIni();
//	QMap<QString, QString> 	m_gKey;
	/////////////////////////////////////////////////////////
	bool isLocalDev(const QString &str) const
	{
		return (str == m_strLocalMac);
	}

public slots:
    void notifyInternetState();
private:
    enum NavBtnState
    {
        BTNSTATE_NORMAL     = 1 << 0,
        BTNSTATE_HOVER      = 1 << 1,
        BTNSTATE_DISABLED   = 1 << 2
    };


	bool	m_isRouterOk;
protected:
	QString			m_strLocalMac;
	
    PowerlineView	*m_topologyview;
	Powerline_HubMapView	*m_hubView;

    DialogMgr           m_dlgMgr;

    QString m_strCurMac;

    QList<PLCNodeData *>    m_dev;
    //QList<QPair<qint16,qint16> > m_devLinkRate;
    QList<PLDevice *>         m_subscribeDevsList;


    QLabel          *m_devNumberLabel;
    QPushButton     *m_refreshBtn;
    QPushButton     *m_prevPageBtn;
    QPushButton     *m_nextPageBtn;
    //QPushButton     *m_firstPageBtn;
    //QPushButton     *m_lastPageBtn;
    //QLabel          *m_pagesLabel;
    QWidget         *m_pageNavPanel;
    WaitPanel   *m_waittingPanel;
    QStackedLayout  *m_topWaittingLayout;
    QVBoxLayout     *m_layOut;

    //int     m_prevPageBtnState;
    //int     m_nextPageBtnState;
    //int     m_firstPageBtnState;
    //int     m_lastPageBtnState;

    int     m_devTotalCount;
    int     m_pageTotalCount;
    int     m_currentPageIndex;

    bool    m_bUpdateUi;
	bool	m_bAnimateStop;
    int	    m_nOperRetCode;
    int		m_nWhosOper;

#ifdef TEST_PL_CODE
	bool    m_isHubView;
#endif

protected:
    virtual bool eventFilter(QObject *obj, QEvent *event);
//    virtual void showEvent(QShowEvent *);
//    virtual void hideEvent(QHideEvent *);

    enum SHOW_STATE
    {
        SS_SHOW,
        SS_HIDE,
        SS_UNKNOW
    };
    SHOW_STATE	m_nShowState;
    enum SHOW_DLG_WHO
    {
        SDW_HAVE_SHOW,
        SDW_NO_SHOW,
        SDW_UNKNOW
    };
    SHOW_DLG_WHO m_TipShowState;
    SHOW_DLG_WHO m_WirelessShowState;
private:
    //void deviceFromRawData(PLDevice &dev,const PLCNodeData &data, QPair<qint16,qint16> &linkRate);
	void deviceFromRawData(PLDevice &dev,const PLCNodeData &data);
    void dealLinkPLCNode(QList<PLCNodeData *> &data, QList<PLDevice *> &dev);
    void dataDealByPage();

    void dealAllData();

    QList<PLDevice *> m_curPageDevList;



    QWidget *createPageBtnsAndLabelPanel();
    void updateUiElements(bool m_beingWaitting = false);
    void subscribeCurrentPageData();
    void retranslateImages();
public:
	void initControlBtn(bool b = false);


signals:
    void deviceSetupMenuItemTriggered();
    void refreshOver();
	void pageChange(int);

    void queryPLCDevicePasswordSignal(const QString &devMac, QString *pwd);
    void storePLCDevicePasswordSignal(const QString &devMac, const QString &pwd);
public:
    void loadPLCDevicePassword(const QString &devMac, QString *pwd);
    void storePLCDevicePassword(const QString &devMac, const QString &pwd);

private slots:
    void onNodeClicked(int idx);
    void onNodeContextMenu(int idx, const QPoint &pos);

    void slot_onNewData(TopologyDataTransfer);
    void slot_dataFinish(int);
    void slot_operRet(int,bool);

    void btnClicked();

    bool slot_btn_refresh();
	bool slot_timer_refresh();
    void slot_Btn_Click(int);		//0:preBtn, 1:nextBtn;

private:
	bool	m_bHaveIp;
private:
	void showTimelineDlg();
private slots:
	void slot_timelineDlgOver(int);

};

#endif
