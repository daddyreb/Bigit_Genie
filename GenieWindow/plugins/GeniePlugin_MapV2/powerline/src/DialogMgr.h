#ifndef DIALOGMGR_H
#define DIALOGMGR_H

#include <QObject>
#include <QDialog>


#include "PowerlineHandler.h"
#include "PowerlinePrompt.h"
#include "PowerlineSecurity.h"
#include "PowerlineDeviceName.h"
#include "PowerlineWirelessSet.h"
#include "Powerline_MessageBox.h"
#include "Powerline_DetailDlg.h"

class PowerlineTimeLineBox;

#include "Singleton.h"

#ifdef ENABLE_SMARTWIFIWIZARD
#include "smartwirelesssettingwizard.h"
#endif


class DialogMgr : public QObject, public Singleton<DialogMgr>
{
    Q_OBJECT
public:
    DialogMgr(QObject *parent = 0);
	~DialogMgr();

public:
    int showFactorySetTip(const QString &title, const QString &context, const QString &imgPath);
    int showApplyDialog(QList<PLDevice *> &,QDialog *dlg = NULL);
    int showDevName(const QString &name, const QString &type);
    int showWirelessSet(const QString &model, WLANInfo &);

    bool setOperMac(const QString &);
	void setOperIp(const QString &);

    void ModifyDevName(const QString &name);
    void TurnLedState(unsigned int nState);

    int showMsgBox(const QString &title,  const QString &content, int nType = PL_MB_OK);
	int showMsgBox(const QString &title,  QString &content, const QString &strChkBox, int nType = PL_MB_OK);
	void showTimeLineBox(int titleID, int contentID, unsigned int timeLine, QWidget *parent = NULL);
	int getChkboxState() const;

    bool refreshScanDev();
	void chkUpdate();

	void showDlgDevLst(PowerlineNode *topNode, PowerlineEdge *topEdge, const QList<AttachDeviceEntry> &);
	void showDlgDevLst(PowerlineNode *topNode, PowerlineEdge *topEdge, const QList<QPair<QString,QString> > &);

	void showDlgRate(PowerlineNode *topNode, PowerlineEdge *topEdge, const QList<PLDevice *> &lstPL);

	void setDlgParent(QWidget *w);

	void showApplyDlgSec(QList<QStringList> strList);
	inline bool isFirstStartup()
	{
		return m_bFirstStart;
	};
    enum OPER_TYPE
    {
        OT_SUCCESS,
        OT_PENDING,
        OT_FAILED,
		
		OT_UPDATE_SUCCESS,
		OT_UPDATE_FAILED,
		OT_UPDATE_NOUP,
    };
    enum OPER_RET
    {
        OR_MODIFY_NAME,
        OR_TURN_LED,
        OR_MODIFY_KEY,
        OR_BACKTO_FACTORY,
		OR_WIRELESS_SET,
		OR_CHKUPDATE,
    };
protected:
    PowerlineSecurity		*m_power;
    PowerlinePrompt			*m_diaPrompt;
    PowerlineHandler		m_plHandler;
    PowerlineDeviceName		*m_dlgDevName;
	PowerlineWirelessSet	*m_wirelessDlg;
	//Powerline_MessageBox	m_msgBox;
	//Powerline_DetailDlg		m_dlgDevList;
	//PowerlineSecurity_PWD	*m_powerSec;

#ifdef ENABLE_SMARTWIFIWIZARD
    SmartWirelessSettingWizard *m_wifiWizard;
    QWidget *m_wizardParentWidget;
    unsigned long m_searchRouterTag;
#endif

    QString m_strMac;
	QString m_strIp;

	QWidget *m_widget;

	int			m_chkboxState;
	bool		m_bFirstStart;
	//////////////////////////////
private:
	QMap<QString, QString> m_mapSecKeyInfo;
public:
	QMap<QString, QString> getRetSecInfo() const;
	/////////////////////////////
private:
    void init();
    bool isValidValue(const QString &);
	void openUrl(const QString &);
	void positionCenter(QWidget *w, QWidget *parentW = NULL);
signals:
    void onNewData(TopologyDataTransfer);
    void commitOper(int nType);
    void operRet(int, bool);
	void timeLineDlgOver(int);
private slots:
    void onBackFactory();
    void onSecurity(const QString &, const QList<QPair<QString, QString> >&);
    void onDevName(QString);
    void onTurnLed(int nState);
	void onWirelessSet(WLANInfo &wlanInfo);

    void slot_Finished(TransactionPointer);
    void slot_Scanned(TopologyDataTransfer);

#ifdef ENABLE_SMARTWIFIWIZARD
private:
    void verfyWizard();
private slots:
    void OnSearchRouter();
    void OnRequestApplySetting(const QString &ip, const WLANInfo *wlanInfo);
    void OnWizardCancel();

public:
    void setWiFiWizardParentWidget(QWidget *parent);
#endif

private:
	PowerlineTimeLineBox *m_timelineBox;
private slots:
	void slot_timelineover(int);

};

#endif // DIALOGMGR_H
