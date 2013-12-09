#ifndef NETCARDSPANEL_H
#define NETCARDSPANEL_H

#include <QWidget>
#include <QList>
#include "topology.h"

class QPushButton;
class QButtonGroup;
class QHBoxLayout;
class NetworkMapView;

#ifdef POWERLINE_REVISION
#define TIME_OUT 5*1000
#define ENVIROMENT "ProgramFiles"
#define PROGRAM_DIRCTORY "\\NETGEAR\\NETGEAR Powerline Utility\\"
#define PROGRAM_PATH "\\NETGEAR\\NETGEAR Powerline Utility\\NETGEAR_Powerline_Utility.exe"
#define INT6K_PATH "int6k.exe"

class PowerlineHandler:public QObject,public Singleton<PowerlineHandler>
{
    Q_OBJECT
public:
    PowerlineHandler();
    ~PowerlineHandler();
    QProcess mProcess;
    QTimer mTimer;

protected slots:
    void slot_Timeout();
    void slot_but_clicked();
    //void finished(int exitCode,QProcess::ExitStatus exitStatus);
private:
    //void getInterfacesIndex(QList<int> &indexList);
public:
    void launchUtitity();
signals:
    void powerLineStateChanged(int totalPowerLine);
};
#endif

class NetcardsPanel : public QWidget
{
    friend class NetworkMapView;

    Q_OBJECT
public:
    explicit NetcardsPanel(NetworkMapView *parent = 0);
    ~NetcardsPanel();
private:
    int                     m_currentAdapterId;
    bool                    m_currentAdapterNotifyed;
    int                     m_netcardsCount;
    unsigned int            m_netcardAblityMask;
    bool                    m_netcardsVisible;
    QList<QPushButton*>     m_netcardBtnList;
    QButtonGroup            *m_btnGroup;
    QPushButton             *m_refreshBtn;

#ifdef ENABLE_POWERLINE
    bool                    m_plcBtnUsible;
    bool                    m_backBtnUsible;
    QPushButton             *m_goPLCBtn;
    QPushButton             *m_backBtn;
#endif

private:
    void applyStyle();
    bool verifyUsableNetcard();
    void notifyCurrentNetcardChanged();
    QHBoxLayout *preparePanelLayout();
    void layoutBtns(QHBoxLayout *panelLayout);
protected:
    void changeEvent(QEvent *e);
    QTimer mDelayCurrrentSettingTimer;
signals:
    void currentNetcardChanged(int idx);
    void refreshClicked();
#ifdef ENABLE_POWERLINE
    void requestGoPLCPage(bool go);
    void backClicked();
#endif

public slots:
    void onNetcardsChanged(const SVT_NotifyType &data);
    void slot_RefreshCurrentSetting();

    //void setNetcardsVisible(bool visible);
#ifdef ENABLE_POWERLINE
    void setPLCBtnUsible(bool usible);
    void setBackBtnUsible(bool usible);
    void setBackBtnText(const QString &text);
private:
     void updateUiElements();
#endif

private slots:
    void buttonClicked(int id);
};

#endif // NETCARDSPANEL_H
