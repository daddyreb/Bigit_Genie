#ifndef POWERLINEWIRELESSSET_H
#define POWERLINEWIRELESSSET_H

#include <QDialog>
#include "mainpipedefines.h"

namespace Ui {
    class PowerlineWirelessSet;
}

class PowerlineWirelessSet : public QDialog
{
    Q_OBJECT

public:
    explicit PowerlineWirelessSet(QWidget *parent = 0);
    ~PowerlineWirelessSet();

public:
	void initializeValue(WLANInfo &);
protected:
	void mousePressEvent(QMouseEvent *);
	void mouseReleaseEvent(QMouseEvent *);
	void mouseMoveEvent(QMouseEvent *);
	void changeEvent(QEvent *);

	void retranslateUi();



	bool    m_bMove;
	QPoint  m_position;
private:
	void init();
	void initValue();
	void initSlot();
	void initLanguage();
	QString autoWrap(const QString &s, int limit=28);

	enum RADIO_MODE
	{
		RM_NONE,
		RM_WPA2,
		RM_WPA_WPA2,
		RM_UNKNOWN
	};
	RADIO_MODE chkRdoValue(const QString &);
	void setRadioCheck(RADIO_MODE rdtChoice);


	WLANInfo		m_wlanInfo;
	bool			m_bHaveKey;
	RADIO_MODE		m_rdt;
signals:
	void signalApply(WLANInfo &);
private slots:
	void slot_btn_apply();
	void slot_btn_cancel();
	void slot_rdo_none();
	void slot_rdo_other();

        void slot_edt_chge(QString);
private:
    Ui::PowerlineWirelessSet *ui;
};

#endif // POWERLINEWIRELESSSET_H
