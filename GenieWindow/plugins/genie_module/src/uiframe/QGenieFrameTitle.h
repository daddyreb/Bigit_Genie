#ifndef QGenieFrameTitle_H
#define QGenieFrameTitle_H
#ifndef IN_STABLE_H
#include <QObject>
#include <QFrame>
#include "Singleton.h"
#include "ui_QGenieFrameTitle.h"
#include <QTimer>
#include "QGenieFrameAbout.h"
#endif


#define TITLE_WIFI_TOOLTIP_MODE "<span style=\" color:rgb(0,113,215);\"><b>%1:</b></span> %2"
#define TITLE_WIFI_TEXT_MODE "<span style=\" color:rgb(249,253,251);\"><b>%1:</b></span> <span style=\" color:rgb(78,167,13);\"><b>%2</b></span>"

class QGenieFrameTitle:public QFrame,public Singleton<QGenieFrameTitle>
{
    Q_OBJECT
public:
    QGenieFrameTitle(QWidget *parent=NULL);
    ~QGenieFrameTitle();
//    void setRouteText(QString s);
    void setFirmwareText(QString s);
    Ui::QGenieFrameTitle mUi;
    void setSoftWareUpdateState(bool show);
    void setFirmwareUpdateState(bool show);
   // QString mRouteType;
    void setWifiName(const QString &wname);
    QString mRouteFirmware;
    QString mWifiType;
private:

    void hideUselessButton(bool maximize);
    bool mSoftFirst;
protected:
    virtual void	showEvent ( QShowEvent * event );
protected:
    void changeEvent(QEvent* event);
    void updateUpdateIcon();

    QTimer mTimerCheckUpdate;
    QTimer mTimerSoftwareOrFirmware;
#ifdef SHOW_ABOUT_BUG
    QTimer testtimer;
#endif
    int mUpdateState;
    QGenieFrameAbout *mAbout;
    virtual bool	eventFilter ( QObject * watched, QEvent * event );
    bool softOrFirmware();
protected slots:
    void slot_ButMaximizeClicked();
    void slot_ButMinimizeclicked();
    void slot_ButCloseClicked();
    void slot_ButNormalizeClicked();
    void slot_checkUpdateTimeout();
    void slot_SoftwareOrFirmwareTimeout();
    void on_but_about_clicked();
    void on_but_update_clicked();
    void on_but_getpwd_clicked();
#ifdef SHOW_ABOUT_BUG
    void slot_testTimeout();
#endif
//public slots:

    void on_but_title_clicked();
};

#endif // QGenieFrameTitle_H
