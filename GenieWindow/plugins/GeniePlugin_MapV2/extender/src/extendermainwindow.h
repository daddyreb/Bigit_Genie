#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <QtNetwork/QtNetwork>
#include <QMouseEvent>
#include <QWidget>
#include <QVBoxLayout>
#include <QTableWidgetItem>
#include <QModelIndex>
#include <QList>
#include <QRadioButton>
#include <QPushButton>
#include <QMessageBox>
#include <QTranslator>
#include <QDialog>
#include <QFrame>
#include <QPaintEvent>
#include <QRect>

#include <windows.h>
#include <windowsx.h>
#include "extendersoapapi.h"
#include "waittingwidget.h"
#include "progresswidget.h"

namespace Ui {
class MainWindow;
}

class ExtenderMainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit ExtenderMainWindow(QWidget *parent = 0);
    ~ExtenderMainWindow();

public slots:
    // configrate for wizard set
    void slt_authenticate(QNetworkReply* reply);

    void slt_getDeviceInfo(QNetworkReply* reply);

    void slt_getAPInfo2G(QNetworkReply* reply);
    void slt_getAPInfo5G(QNetworkReply* reply);

    void slt_configurationStarted(QNetworkReply* reply);
    void slt_setExtenderMode(QNetworkReply* reply);

    void slt_setRouterWLANNoSecurity(QNetworkReply* reply);
    void slt_setRouterWLANWEPByKeys(QNetworkReply* reply);
    void slt_setRouterWLANWPAPSKByPassphrase(QNetworkReply* reply);

    //void slt_getRouterWLANVerifyResult(QNetworkReply* reply);

    void slt_setWLANNoSecurity(QNetworkReply* reply);
    void slt_setWLANWEPByKeys(QNetworkReply* reply);
    void slt_setWLANWPAPSKByPassphrase(QNetworkReply* reply);

    void slt_setEnable(QNetworkReply* reply);
    void slt_configurationFinished(QNetworkReply* reply);

    void sltResetToFactoryDefault(QNetworkReply* reply);

    void slt_getAPListFinished();
    void slt_setRouterWLANFinished();
    void slt_setExtenderWLANFinished();
    void slt_extendFinish();

public slots:
    // configrate for advanced set
    void sltGetRouterWLANInfo2G(QNetworkReply* reply);
    void sltGetRouterWLANInfo5G(QNetworkReply* reply);
    void sltGetRouterWLANWPAInfo2G(QNetworkReply* reply);
    void sltGetRouterWLANWPAInfo5G(QNetworkReply* reply);
    void sltGetRouterWLANWEPInfo2G(QNetworkReply* reply);
    void sltGetRouterWLANWEPInfo5G(QNetworkReply* reply);
    void sltGetWLANInfo2G(QNetworkReply* reply);
    void sltGetWLANInfo5G(QNetworkReply* reply);
    void sltGetWLANWPAInfo2G(QNetworkReply* reply);
    void sltGetWLANWPAInfo5G(QNetworkReply* reply);
    void sltGetWLANWEPInfo2G(QNetworkReply* reply);
    void sltGetWLANWEPInfo5G(QNetworkReply* reply);
    void sltGetRouterInfoFinished();
    void sltGetWLANInfoFinished();
    void sltBackChoosePage();

    void translate();


public:
    bool verifyManualSetOK();
    void showFirstPage();

private:
    QString routerStatus2G;
    QString routerStatus5G;
    QString routerSSID2G;
    QString routerSSID5G;
    QString routerEncryptionModes2G;
    QString routerEncryptionModes5G;
    QString routerPassword2G;
    QString routerPassword5G;
    QString routerWPAPassphrase2G;
    QString routerWPAPassphrase5G;
    QString routerWEPPassphrase2G;
    QString routerWEPPassphrase5G;
    QString wlanStatus2G;
    QString wlanStatus5G;
    QString wlanSSID2G;
    QString wlanSSID5G;
    QString wlanEncryptionModes2G;
    QString wlanEncryptionModes5G;
    QString wlanPassword2G;
    QString wlanPassword5G;
    QString wlanWPAPassphrase2G;
    QString wlanWPAPassphrase5G;
    QString wlanWEPPassphrase2G;
    QString wlanWEPPassphrase5G;

    QString wlanPassphrase2G;
    QString wlanPassphrase5G;

    int getRouterSetCount;
    int getWLANSetCount;
    int waitting_text2_idx;

    QString RebootTime;
    QString m_serialNumber;
    int bIsResetToDefault;
    bool advancedSet;
    bool bAdvancedSetBack;

public:    
    void setRouterWLANNoSecurity(QString);
    void setRouterWLANWEPByKeys(QString);
    void setRouterWLANWPAPSKByPassphrase(QString);

    void setRouterWLAN();
    void setRouterWLANManual(QString);
    void setRouterWLANAuto(QString);

    // functions for control app
    void startAuthenticate();
    void jumpPG_connectToExtender();
    void jumpPG_wait();
    void setWaittingText(int idx);

    void initialLanguage();

    QString getSSIDFromWifiList(QString radioModel, int rowNumber);
    QString getSecuritytypeFromWifiList(QString radioModel, int rowNumber);

    void showInvalidPwdMsgBox();
    void showInvalidSSIDMsgBox();

    bool verifyExtenderToRouterPageOK();
    bool verifyExtenderSetPageOK();
    void UiRecordAndShow();

    bool validSSID(const QString &SSID);
    bool validPassord(const QString &password);

signals:
    void authenticateFinished();
    void setRouterWLANFinished();
    void setExtenderWLANFinished();

    void authenticateResult(bool);
    void connectNeedGenieHelp();
    void connectManuallyOK();

    void emitSetCurrentLanguage(int);
    void emitCurrentSetting();

    void emitGetRouterInfo();
    void emitGetWLANInfo();
    void emitGetError();

    void emitExit();

    void emitRestart();
protected:
    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *);
    //bool winEvent(MSG *message, long *result);

    void paintEvent(QPaintEvent *);
    virtual void changeEvent(QEvent *event);
    void retranslateUi();

protected:
    ProgressWidget *m_refresh_progress_panel;
    
private slots:
    void on_btn_refresh_clicked();

    void on_tblwdt_wifiList2G_itemSelectionChanged();

    void on_tblwdt_wifiList5G_itemSelectionChanged();

    void on_btn_extenderToRouter_clicked();

    void on_btn_extenderSet_clicked();

    void on_rbtn_none2G_clicked(bool checked);

    void on_rbtn_WEP2G_clicked(bool checked);

    void on_rbtn_WPAPSKTKIP2G_clicked(bool checked);

    void on_rbtn_WPA2PSKAES2G_clicked(bool checked);

    void on_rtbn_WPAandWAP22G_clicked(bool checked);

    void on_rbtn_none5G_clicked(bool checked);

    void on_rbtn_WEP5G_clicked(bool checked);

    void on_rbtn_WPAPSKTKIP5G_clicked(bool checked);

    void on_rbtn_WPA2PSKAES5G_clicked(bool checked);

    void on_rtbn_WPAandWPA25G_clicked(bool checked);

    void on_cmb_bit2G_currentIndexChanged(int index);

    void on_cmb_bit5G_currentIndexChanged(int index);

    void on_btn_pwdGenerator5G_clicked();

    void on_btn_pwdGenerator2G_clicked();

    void on_chk_usingCurrentSetting_stateChanged(int arg1);

    void on_btn_close_clicked();

    void on_btn_minimize_clicked();

    void on_btn_about_clicked();

    void on_head_clicked();

    void on_btn_extendFinish_clicked();

    void on_btn_funcChoose_clicked();

    void on_cmb_bitAutoSet2G_currentIndexChanged(int index);

    void on_btn_pwdGeneratorAuto2G_clicked();

    void on_rbtn_key1AutoSet2G_clicked(bool checked);

    void on_rbtn_key2AutoSet2G_clicked(bool checked);

    void on_rbtn_key3AutoSet2G_clicked(bool checked);

    void on_rbtn_key4AutoSet2G_clicked(bool checked);

    void on_cmb_bitAutoSet5G_currentIndexChanged(int index);

    void on_btn_pwdGeneratorAuto5G_clicked();

    void on_rbtn_key1AutoSet5G_clicked(bool checked);

    void on_rbtn_key2AutoSet5G_clicked(bool checked);

    void on_rbtn_key3AutoSet5G_clicked(bool checked);

    void on_rbtn_key4AutoSet5G_clicked(bool checked);

    void on_cbx_securityTypeChooseManual2G_currentIndexChanged(int index);

    void on_cmb_bitManualSet2G_currentIndexChanged(int index);

    void on_btn_pwdGeneratorManual2G_clicked();

    void on_rbtn_key1ManualSet2G_clicked(bool checked);

    void on_rbtn_key2ManualSet2G_clicked(bool checked);

    void on_rbtn_key3ManualSet2G_clicked(bool checked);

    void on_rbtn_key4ManualSet2G_clicked(bool checked);

    void on_cbx_securityTypeChooseManual5G_currentIndexChanged(int index);

    void on_cmb_bitManualSet5G_currentIndexChanged(int index);

    void on_btn_pwdGeneratorManual5G_clicked();

    void on_rbtn_key1ManualSet5G_clicked(bool checked);

    void on_rbtn_key2ManualSet5G_clicked(bool checked);

    void on_rbtn_key3ManualSet5G_clicked(bool checked);

    void on_rbtn_key4ManualSet5G_clicked(bool checked);

    void on_btn_backExtenderToRouter_clicked();

    void on_btn_extenderToRouterContinue_clicked();

    void on_rbtn_key12G_clicked(bool checked);

    void on_rbtn_key22G_clicked(bool checked);

    void on_rbtn_key32G_clicked(bool checked);

    void on_rbtn_key42G_clicked(bool checked);

    void on_rbtn_key15G_clicked(bool checked);

    void on_rbtn_key25G_clicked(bool checked);

    void on_rbtn_key35G_clicked(bool checked);

    void on_rbtn_key45G_clicked(bool checked);



    void on_rbtn_WPAPSKAES5G_clicked(bool checked);

    void on_rbtn_WPA2PSKTKIP5G_clicked(bool checked);

    void on_rbtn_WPAPSKAES2G_clicked(bool checked);

    void on_rbtn_WPA2PSKTKIP2G_clicked(bool checked);


    void on_btn_cancel_clicked();

    void on_btn_ignore_clicked();

    void on_rbtn_auto2G_clicked(bool checked);

    void on_rbtn_manual2G_clicked(bool checked);

    void on_rbtn_auto5G_clicked(bool checked);

    void on_rbtn_manual5G_clicked(bool checked);

    void on_btn_connectContinue_clicked();

    void on_rbtn_connectManually_clicked(bool checked);

    void on_rbtn_needGenieHelp_clicked(bool checked);

private:
    Ui::MainWindow *ui;

    QTimer *timer;

    QSettings *languageSettings;

    WaittingWidget *waitPanel;
    //AboutDialog *aboutDialog;
    ExtenderSoapApi *soapapi;

    QPoint last;
    bool moving;

    QString m_isExtender;
    bool flagWizard;
    bool flagAdvanced;

    QString NewSessionID;
    QString curLanguage;
    int cTime;

    int connectExtenderCount;
    int btn_connectContinue_text_idx;
    int waitting_text_idx;

    // Auto Set
    int m_setRouterWLANOK;

    bool flagWifiAutoSel2G;
    bool flagWifiAutoSel5G;
    bool flagWifiManualSel2G;
    bool flagWifiManualSel5G;

    bool flagWifiAutoSel2GRecord;
    bool flagWifiAutoSel5GRecord;
    bool flagWifiManualSel2GRecord;
    bool flagWifiManualSel5GRecord;

    bool flagWifiOFFSel2G;
    bool flagWifiWEPSel2G;
    bool flagWifiWPAPSKSel2G;
    bool flagWifiOFFSel5G;
    bool flagWifiWEPSel5G;
    bool flagWifiWPAPSKSel5G;

    bool flagWifiSelected2G;
    bool flagWifiSelected5G;

    bool flagGetAPListFinished2G;
    bool flagGetAPListFinished5G;

    bool m_bWifiListRefreshing;
    bool m_isRefreshed;

    int flagRouterWLANSet;
    int flagRouterWLANSetRight;
    int flagRouterWLANSetError;
    int flagExtenderWLANSet;
    int flagExtenderWLANSetRight;
    int flagExtenderWLANSetError;

    QString m_newStatus;

    QString m_SSID2G;
    QString m_SSID5G;
    QString m_securitytype2G;
    QString m_securitytype5G;
    QString m_password2G;
    QString m_password5G;
    QString m_passwordWEP2G;
    QString m_passwordWEP5G;

    QString m_extenderSecuritytypeManual2G;
    QString m_extenderSecuritytypeManual5G;

    QString m_extenderSSID2G;
    QString m_extenderSSID5G;
    QString m_extenderSecuritytype2G;
    QString m_extenderSecuritytype5G;
    QString m_extenderPassword2G;
    QString m_extenderPassword5G;

    int m_newKeyIndex2G;
    int m_newKeyIndex5G;
    int m_newWEPLength2G;
    int m_newWEPLength5G;

    int m_extenderNewKeyIndex2G;
    int m_extenderNewKeyIndex5G;
    int m_extenderNewWEPLength2G;
    int m_extenderNewWEPLength5G;

    QString m_extenderPasswordWEP2G;
    QString m_extenderPasswordWEP5G;

    QString m_WEPKey12G;
    QString m_WEPKey22G;
    QString m_WEPKey32G;
    QString m_WEPKey42G;
    QString m_WEPKey15G;
    QString m_WEPKey25G;
    QString m_WEPKey35G;
    QString m_WEPKey45G;

    QString m_extenderWEPKey12G;
    QString m_extenderWEPKey22G;
    QString m_extenderWEPKey32G;
    QString m_extenderWEPKey42G;
    QString m_extenderWEPKey15G;
    QString m_extenderWEPKey25G;
    QString m_extenderWEPKey35G;
    QString m_extenderWEPKey45G;

    QString m_extenderRadioModel;

    int configurationFinishedResponsTime;

    QNetworkAccessManager AuthenticateAccessManager;

    QNetworkAccessManager GetDeviceInfoAccessManager;
    QNetworkAccessManager GetAPInfoAccessManager2G;
    QNetworkAccessManager GetAPInfoAccessManager5G;

    QNetworkAccessManager ConfigurationStartedAccessManager;

    QNetworkAccessManager SetExtenderModeAccessManager;

    QNetworkAccessManager SetRouterWLANNoSecurityAccessManager2G;
    QNetworkAccessManager SetRouterWLANWEPByKeysAccessManager2G;
    QNetworkAccessManager SetRouterWLANWPAPSKByPassphraseAccessManager2G;
    QNetworkAccessManager SetRouterWLANNoSecurityAccessManager5G;
    QNetworkAccessManager SetRouterWLANWEPByKeysAccessManager5G;
    QNetworkAccessManager SetRouterWLANWPAPSKByPassphraseAccessManager5G;

    QNetworkAccessManager SetWLANNoSecurityAccessManager2G;
    QNetworkAccessManager SetWLANNoSecurityAccessManager5G;
    QNetworkAccessManager SetWLANWEPByKeysAccessManager2G;
    QNetworkAccessManager SetWLANWEPByKeysAccessManager5G;
    QNetworkAccessManager SetWLANWPAPSKByPassphraseAccessManager2G;
    QNetworkAccessManager SetWLANWPAPSKByPassphraseAccessManager5G;

    QNetworkAccessManager SetEnableAccessManager;

    QNetworkAccessManager ConfigurationFinishedAccessManager;

    // get configration for advanced set
    QNetworkAccessManager ResetToFactoryDefaultManager;
    QNetworkAccessManager GetRouterWLANInfoManager2G;
    QNetworkAccessManager GetRouterWLANInfoManager5G;
    QNetworkAccessManager GetRouterWLANWPAInfoManager2G;
    QNetworkAccessManager GetRouterWLANWPAInfoManager5G;
    QNetworkAccessManager GetRouterWLANWEPInfoManager2G;
    QNetworkAccessManager GetRouterWLANWEPInfoManager5G;
    QNetworkAccessManager GetWLANInfoManager2G;
    QNetworkAccessManager GetWLANInfoManager5G;
    QNetworkAccessManager GetWLANWPAInfoManager2G;
    QNetworkAccessManager GetWLANWPAInfoManager5G;
    QNetworkAccessManager GetWLANWEPInfoManager2G;
    QNetworkAccessManager GetWLANWEPInfoManager5G;

private:
    void initialStyle();
    void initializeUi();
    void initializeVariable();

    void setWaittingUi(QVBoxLayout *vl);
    void setStepUi(int step);
    void showAPList2G(QString str);
    void showAPList5G(QString str);

private:
    bool bBackBtnClicked;

private slots:

    void on_btn_nextConnectManually_clicked();

    void on_btnBack_clicked();

    void on_btnApply_clicked();

    void on_cbxWlanAuthType2G_currentIndexChanged(int index);

    void on_cbxWlanAuthType5G_currentIndexChanged(int index);

    void on_btnDefaultExtender_clicked();

    void on_rbtn_haveConnected_clicked(bool checked);

    void on_cmb_language_currentIndexChanged(int index);

public slots:
    void onLanguageChanged();

signals:
    void translateText(int, QString *text);

private:
    QString translateText(int idx, const QString &defaultText = NULL);

    // For Exit Dialog
private slots:
    void sltExit();

private:
    QPoint mDragPoint;
    bool mDrag;
protected:
    void _setFramelessStyle(QWidget *w);
    virtual bool eventFilter(QObject *, QEvent *);
};

#endif // MAINWINDOW_H
