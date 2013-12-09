#include "PowerlineWirelessSet.h"
#include "ui_PowerlineWirelessSet.h"
#include "PowerlineDefine.h"
#include "language_Map.h"
#include <QMouseEvent>

#include <QDebug>



static const char* rdtType[] =
{
	"None",
	"WPA2-PSK",
	"WPA-PSK[TKIP]+WPA2-PSK[AES]"
};

PowerlineWirelessSet::PowerlineWirelessSet(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::PowerlineWirelessSet)
	,m_bHaveKey(false)
	,m_bMove(false)
{
	ui->setupUi(this);

	init();
	initLanguage();
	retranslateUi();
}
PowerlineWirelessSet::~PowerlineWirelessSet()
{
	delete ui;

	qDebug() << "PowerlineWirelessSet exit";
}
//=================================================
//  initialization funtion
//=================================================
void PowerlineWirelessSet::initLanguage()
{
	POWERLINE_TRANSLATE_BUTTON(ui->but_apply, L_POWERLINE_BUT_APPLY, "Apply");
	POWERLINE_TRANSLATE_BUTTON(ui->but_cancel, L_POWERLINE_BUT_CANCEL, "Cancel");
	POWERLINE_TRANSLATE_LABEL(ui->label_title, L_POWERLINE_WIRELESS_TITLE, "Wireless Security Setting");
	POWERLINE_TRANSLATE_LABEL(ui->lab_ssid, L_POWERLINE_WIRELESS_SSID, "SSID");
	POWERLINE_TRANSLATE_LABEL(ui->lab_key, L_POWERLINE_WIRELESS_KEY, "Network Key");
	//POWERLINE_TRANSLATE_LABEL(ui->lab_channel, L_POWERLINE_WIRELESS_CHANNEL, "Channel");
	POWERLINE_TRANSLATE_LABEL(ui->lab_words_limit, L_POWERLINE_WIRELESS_TIP, autoWrap("8 and 64 characters8 and 64 characters8 and 64 characters"));
	//QString strTmp = POWERLINE_GET_TEXT(L_POWERLINE_WIRELESS_TYPE, "Channel");
	//strTmp += ":";
	POWERLINE_TRANSLATE_LABEL(ui->lab_sec, L_POWERLINE_WIRELESS_TYPE, "Security Selection");
	//ui->lab_sec->setText(strTmp);
}
void PowerlineWirelessSet::init()
{
	initValue();
	initSlot();
}
void PowerlineWirelessSet::initValue()
{
	ui->edt_key->clear();
	ui->edt_ssid->clear();
	m_bHaveKey = false;


	ui->edt_key->setMaxLength(64);
	ui->edt_ssid->setMaxLength(32);
	ui->edt_key->setValidator(new QRegExpValidator(QRegExp("([A-Z]|[a-z]|[0-9]){64}"), this));

	ui->but_apply->setDisabled(true);


}
void PowerlineWirelessSet::initSlot()
{
	connect(ui->but_apply, SIGNAL(clicked()), this, SLOT(slot_btn_apply()));
	connect(ui->but_cancel, SIGNAL(clicked()), this, SLOT(slot_btn_cancel()));
	connect(ui->rdt_none, SIGNAL(clicked()), this, SLOT(slot_rdo_none()));
	connect(ui->rdt_mix, SIGNAL(clicked()), this, SLOT(slot_rdo_other()));
	connect(ui->rdt_aes, SIGNAL(clicked()), this, SLOT(slot_rdo_other()));

	connect(ui->edt_ssid, SIGNAL(textChanged(QString)), this, SLOT(slot_edt_chge(QString)));
	connect(ui->edt_key, SIGNAL(textChanged(QString)), this, SLOT(slot_edt_chge(QString)));
}
//=================================================
//  public interface funtion
//=================================================
void PowerlineWirelessSet::initializeValue(WLANInfo &data)
{
	ui->but_cancel->setFocus();

	ui->edt_ssid->setText(data.m_ssid);
	ui->cob_channel->setCurrentIndex(data.m_channel.toInt());

	setRadioCheck(chkRdoValue(data.m_basicEncryptionModes));
	ui->edt_key->setText(data.m_wlanPassphrase);

	m_wlanInfo = data;

	ui->but_apply->setDisabled(true);

}
//=================================================
//  other funtion
//=================================================	
QString PowerlineWirelessSet::autoWrap(const QString &s, int limit) 
{
	if(s.length()<limit)
	{
		return s;
	}
	int ruler=limit*5;
	QString ret=s;
	while(ruler>0)
	{
		if(ret.length()>ruler)
		{
			ret.insert(ruler,"\n");
		}
		ruler-=limit;
	}

	return ret;
}

void PowerlineWirelessSet::retranslateUi()
{

	QString strTmp = POWERLINE_GET_TEXT(L_POWERLINE_WIRELESS_CHANNEL, "Channel");
	strTmp += " :";
	ui->lab_channel->setText(strTmp);
}
void PowerlineWirelessSet::setRadioCheck(RADIO_MODE rdtChoice)
{
	switch(rdtChoice)
	{
	case RM_NONE:
		ui->rdt_none->setChecked(true);
		ui->rdt_mix->setChecked(false);
		ui->rdt_aes->setChecked(false);
		slot_rdo_none();
		break;
	case RM_WPA2:
		ui->rdt_none->setChecked(false);
		ui->rdt_mix->setChecked(false);
		ui->rdt_aes->setChecked(true);
		slot_rdo_other();
		break;
	case RM_WPA_WPA2:
		ui->rdt_none->setChecked(false);
		ui->rdt_mix->setChecked(true);
		ui->rdt_aes->setChecked(false);
		slot_rdo_other();
		break;
	case RM_UNKNOWN:
	default:
		ui->rdt_none->setChecked(false);
		ui->rdt_mix->setChecked(false);
		ui->rdt_aes->setChecked(false);
		slot_rdo_other();
		break;
	}
}
PowerlineWirelessSet::RADIO_MODE PowerlineWirelessSet::chkRdoValue(const QString &str)
{
	if(str == "None")
	{
		return RM_NONE;
	}
	else if(str == "WEP" || str == "WPA2-PSK")
	{
		return RM_WPA2;
	}
	else if(str == "WPA-PSK" || str == "WPA-PSK/WPA2-PSK")
	{
		return RM_WPA_WPA2;
	}
	else
	{
		return RM_UNKNOWN;
	}
}
//=================================================
//  slot funtion
//=================================================
void PowerlineWirelessSet::slot_btn_apply()
{

	m_wlanInfo.m_ssid		= ui->edt_ssid->text();

	m_wlanInfo.m_channel	= QString(ui->cob_channel->currentText());
	if(m_bHaveKey)
	{
		m_wlanInfo.m_wlanPassphrase		= ui->edt_key->text();
	}
	m_wlanInfo.m_wpaEncryptionModes		= rdtType[m_rdt];
	m_wlanInfo.m_basicEncryptionModes	= rdtType[m_rdt];


	emit signalApply(m_wlanInfo);
	//this->close();
	this->accept();
}
void PowerlineWirelessSet::slot_btn_cancel()
{
	//this->close();
	this->reject();
}
void PowerlineWirelessSet::slot_rdo_none()
{
	m_bHaveKey = false;
	m_rdt = RM_NONE;
	ui->fr_pwd->hide();
	ui->edt_key->clear();

	if(!ui->edt_ssid->text().isEmpty())
	{
		ui->but_apply->setDisabled(false);
	}

	
}
void PowerlineWirelessSet::slot_rdo_other()
{
	m_bHaveKey = true;
	ui->edt_key->clear();
	ui->but_apply->setDisabled(true);
	if(ui->rdt_mix->isChecked())
	{
		m_rdt = RM_WPA_WPA2;
	}
	else
	{
		m_rdt = RM_WPA2;
	}
	ui->fr_pwd->show();
	ui->edt_key->setFocus();
}
void PowerlineWirelessSet::slot_edt_chge(QString str)
{
	if(m_rdt != RM_NONE)
	{
		if(str.isEmpty() || (str.length() < 8 || str.length() > 64))
		{
			ui->but_apply->setDisabled(true);
			return;
		}
	}
	else
	{
		if(!str.isEmpty() && (m_wlanInfo.m_ssid != str || m_wlanInfo.m_wlanPassphrase != str))
		{
			ui->but_apply->setDisabled(false);
		}
		else
		{
			ui->but_apply->setDisabled(true);
		}
	}

}
//====================================================
//  event function
//====================================================
void PowerlineWirelessSet::mouseMoveEvent(QMouseEvent *evt)
{
	if((evt->buttons() & Qt::LeftButton) && m_bMove && !isFullScreen())
	{
		move(evt->globalPos() - m_position);
		evt->accept();
	}
}
void PowerlineWirelessSet::mousePressEvent(QMouseEvent *evt)
{
	if(evt->button() == Qt::LeftButton)
	{
		m_bMove = true;
		m_position = evt->globalPos() - frameGeometry().topLeft();
	}
}
void PowerlineWirelessSet::mouseReleaseEvent(QMouseEvent *evt)
{
	if(evt->button() == Qt::LeftButton)
	{
		m_bMove = false;
	}
}
void PowerlineWirelessSet::changeEvent(QEvent *evt)
{
	if(evt->type() == QEvent::LanguageChange)
	{
		retranslateUi();
	}
	QDialog::changeEvent(evt);
}
