#ifndef BLOCKDEVICEMANAGER_H
#define BLOCKDEVICEMANAGER_H

#include <QObject>
#include <QtGui>
#include <QtWidgets>
#include "Singleton.h"
class TopologyView;
class NetworkMapView;
class SVT_ControlProxy;
class NetScanningController;
class ILanguageManager;
class IManagerHandler;
class QProgressIndicator;
class QStackedLayout;

class BlockDeviceManager : public QObject,public Singleton<BlockDeviceManager>
{
    Q_OBJECT
public:
    explicit BlockDeviceManager(NetworkMapView *view,NetScanningController*controlproxy);
    ~BlockDeviceManager();
    inline int state() const
    {
        return mFSMState;
    }
    void enableDevice(const QString &mac);
    void disableDevice(const QString &mac);
    void cancelLoginDialog();
    void retranslateUi();
    void resetUi();

//    inline void setManagerHandler(IManagerHandler *lam){
//        mMHandler=lam;
//    }
    void raiseLoginFrame();
    void keyEscape();
    QLabel *mDbgLabel;
protected:
    int mCmdAfterLoginWidgetOff;
    QVariant mParasAfterLoginWidgetOff;
    bool mLogedIn;
    int mFSMState;
    TopologyView *mTopologyView;
    QCheckBox *mCheckBox;
    QWidget *mTopLeftW;
    QLabel *mTopLeftLabel;
    QWidget *mTopLeftLabelFrame;

    /*QGraphicsProxyWidget *mTopLeftPanProxy;*/
    SVT_ControlProxy *mControlProxy;
    QWidget *mLoginFrame;
//    QGraphicsProxyWidget *mLoginFrameProxy;
    QLineEdit *mEdtUsr;
    QLineEdit  *mEdtPwd;
    QPushButton *mButAccept;
    QPushButton *mButCancel;
    QCheckBox *mChkRemember;
    QLabel      *mDefaultPwd;

#ifdef SIMPLE_TOPLEFT_TEXT
    QProgressIndicator *mPi;
#endif

    QStackedLayout      *m_panelLayout;
//    QGraphicsProxyWidget *mDbgLabelProxy;
private:
    int                 m_oldState;
    int             &mDeviceCount;

    void popLoginWidget(int cmd,const QVariant &para=QVariant());
    void enableOrDisable(const QString &mac,int block);
    void translateTopLeftMsg();
    void showTopLeftMessage(int lanidx);
//    IManagerHandler *mMHandler;
    int mTopLeftTextIdx;
    bool eventFilter ( QObject * watched, QEvent * event );
signals:
//    void signal_BlockDeviceCmd(int cmd,const QVariant para);
public slots:
    void onFSMStateChanged(int s,int sold);
    void onCheckBoxStateChanged(int s);
    void onLoginUiClicked();
    void onChkRememberStateChange(int s);

public:
    void UpdateMiniOptUiVisible(bool visible);
private:
    bool m_ctrPanelCanVisible;

private slots:
    void onLoginFailedUiTimeout();

};

#endif // BLOCKDEVICEMANAGER_H
