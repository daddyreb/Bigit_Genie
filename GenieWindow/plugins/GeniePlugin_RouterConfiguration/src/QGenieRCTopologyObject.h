#ifndef QGENIERCTOPOLOGYOBJECT_H
#define QGENIERCTOPOLOGYOBJECT_H

#include <QtCore>
#include <QtGui>
#include <QtWidgets>
#include "Singleton.h"
class QGenieTopoNode;
class QPiObject;

class QGenieTopoDeviceCache
{
public :
    enum DEVICE_CACHE_FLAGS
    {
        DCF_WIRED=(1<<0)
        ,DCF_BLOCKED=(1<<1)
        ,DCF_ISVM=(1<<2)
        ,DCF_HAVE_SIGNAL_DATA=(1<<3)
        ,DCF_HAVE_LINK_RATE=(1<<4)
    };
    enum NODE_TYPE
    {
        NT_INTERNET
        ,NT_CENTERROUTER
        ,NT_GENIE
        ,NT_NETWORKDEVICE
        ,NT_WINDOWSPC
        ,NT_MACBOOK
        ,NT_IMAC
        ,NT_MACMINI
        ,NT_MACPRO
        ,NT_LINUXPC
        ,NT_UNIXPC
        ,NT_GAMECONSOLE
        ,NT_MEDIADEV
        ,NT_CAMERA
        ,NT_SMARTPHONE
        ,NT_TABLET
        ,NT_PRINTER
        ,NT_SCANNER
        ,NT_STORAGE
        ,NT_ROUTER
        ,NT_IPAD
        ,NT_ITOUCH
        ,NT_IPHONE
        ,NT_BRIDGE
        ,NT_REPEATER
        ,NT_CABLESTB
        ,NT_SATELLITESTB
        ,NT_SLINGBOX
        ,NT_OTHERSTB
        ,NT_TV
        ,NT_SWITCH
        ,NT_ANDROIDTABLET
        ,NT_ANDROIDPHONE
        ,NT_ANDROIDDEVICE
        ,NT_BLUERAYPLAYER
        ,NT_DVR
        ,NT_POWERLINE
        ,NT_NUM
    };

    QGenieTopoDeviceCache();
    void setFlag(DEVICE_CACHE_FLAGS flag,bool bSet);
    NODE_TYPE mType;
    QString mIp;
    QString mName;
    QString mMac;
    quint32 mFlags;
    int mLinkSpeed;
    int mSignalStrength;


};

class QGenieRCTopoDetailViewManager:public QObject,public Singleton<QGenieRCTopoDetailViewManager>
{
    Q_OBJECT
public:
    QGenieRCTopoDetailViewManager();
    ~QGenieRCTopoDetailViewManager();
    void setContentText(QWidget *w,QGenieTopoDeviceCache *cache);
    void translateUi();
protected:
    QWidget *mW;
    QShortcut *mShortcut;
    void translateButtons();
public slots:
    void slot_startShowDetail(QGenieTopoDeviceCache *cache);
    void slot_btnClicked();
    void slot_escapKey();
};

class QGenieRCTopologyObject : public QObject,public Singleton<QGenieRCTopologyObject>
{
    Q_OBJECT
public:
    explicit QGenieRCTopologyObject(QObject *parent = 0);
    ~QGenieRCTopologyObject();
    QWidget *getUi();
    void updateControlPanel(int start, int count, int allcount);
    void setLockState(bool lock);
protected:
    QWidget *mW;
    QWidget *mControlPanel;
    QGraphicsView *mGraphicsView;
    QGraphicsScene *mScene;
    int mRefreshSession;
    QList<QGenieTopoNode *> mForeGroundList;
    QList<QGenieTopoNode *> mBackgroundList;
    QGenieTopoNode *getBackgroundNode();
    void addNodeToBackground(QGenieTopoNode *node);
    void refreshUiData1(const QStringList &lst);
    void refreshUiData2(int start,int count);
    QList<QSharedPointer<QGenieTopoDeviceCache> > mDataCaches;
    QStackedWidget *mStack;
    QPiObject *mPi;
    int mStart;
    QGenieRCTopoDetailViewManager *mDetailViewManager;
    QSharedPointer<QGenieTopoDeviceCache> mRouterData;
    QSharedPointer<QGenieTopoDeviceCache> mInternetData;
signals:
    
public slots:
    void slot_RefreshData(bool remote);
    void slot_DataReturned(int session,const QStringList &lst);
    void slot_btnClicked();
    
};

#endif // QGENIERCTOPOLOGYOBJECT_H
