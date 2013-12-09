#ifndef TOPOLOGYVIEW_H
#define TOPOLOGYVIEW_H

#include <QGraphicsView>
#include <QList>
#include <QtCore>

class Device;
class TopologyNode;
class TopologyEdge;
class QGraphicsScene;
class QResizeEvent;
class QTimerEvent;

/*********************************************
The objects of class TopologyView and class
NetScanningController must run in the same thread
***********************************************/



class TopologyView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit TopologyView(QWidget *parent = 0);
    ~TopologyView();
private:
    QList<TopologyNode *> m_nodesList;


    QGraphicsScene        *m_scene;
    TopologyNode          *m_InternetNode;
    TopologyEdge          *m_InternetEdge;

//    bool                  m_internetConnected;
    int                   m_routerRefreshTimerId;
private:
    void itemsFromDevice(TopologyNode *node , TopologyEdge *edge , Device *dev);
    void positionItems();
    void verifyInternetNodeEdge();
    void verifyEdgeTooltip(TopologyEdge *edge);
    void retranslateUi();

#ifdef POWERLINE_REVISION
    bool detectPowerLine(TopologyNode *node , TopologyEdge *edge , Device *dev);
#endif

private:
    QString verifyNodeName(const Device &dev);
    int parseDeivceSignalLevel(const Device &dev);
    int parseDeivceSignalLevel2(const Device &dev);

    void verifyRouterRefreshTimer();

protected:
    virtual void resizeEvent(QResizeEvent *event);
    virtual void timerEvent(QTimerEvent *event);
    virtual void changeEvent(QEvent *event);

    //add by caiyujun
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);
    int getDeviceType(QString strIp);
    //add end

signals:
    void nodeClicked(int idx);
    void nodeContextMenu(int idx, const QPoint &pos);
    //add by caiyujun
    void mouseInNode(bool,int idx);
    void turboStateOffline(QString strIp);
    void bonjourTypeChanged(const Device &dev);
    //add end


public:
    void onNewData(const QList<Device *> &data);
    //add by caiyujun
    void onTurboStateChanged(Device *data, QString ip, bool bOnLine,int iType = 0);
	void otherDevCountChanged(Device *data, int count, bool showDeviceCount);
    void clearTurboOnlineIp();
	quint32 getNodeFlag(QString mac, bool &bResult);
    //add end
    void onInternetStateChanged(bool connected);
#ifdef USE_NODE_ANIMATION
    int getNodeFromMac(const QList<TopologyNode *> &lst,const QString &mac);
    QTimer mAnimTimer;
    QSet<TopologyNode *> mBackGroundNodes;
    QList<TopologyNode *> mAllNodes;
    TopologyNode *getNewNode();
    void hideAllNodes();

protected slots:
    void slot_animTimout();
#else
    QList<TopologyEdge *> m_edgesList;
#endif

signals:
    void translateText(int idx ,QString *text);
private:
    QString translateText(int idx, const QString &defaultText = QString());

    //add by caiyujun
    //QStringList m_TurBoOnLineIp;
    QMap<QString,int> m_TurBoOnLineIp;
    QMap<QString,int> m_ipToType;
    //add end

private slots:
    void onNodeClicked(TopologyNode *);
    void onNodeContextMenu(TopologyNode *,const QPoint&);
    //add by caiyujun
    void onMouseInNode(bool,TopologyNode*);
    //add end


    friend class TopologyNode;
    friend class NetworkMapView;
    friend class BlockDeviceManager;
};

#endif // TOPOLOGYVIEW_H
