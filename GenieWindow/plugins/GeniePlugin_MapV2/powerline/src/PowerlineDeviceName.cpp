#include "PowerlineDeviceName.h"
#include "ui_PowerlineDeviceName.h"
#include "PowerlineDefine.h"
#include "DialogMgr.h"


#include <QMouseEvent>
#include <QDebug>



PowerlineDeviceName::PowerlineDeviceName(QWidget *parent) :
QDialog(parent)
,ui(new Ui::PowerlineDeviceName)
,m_bMove(false)
{
	ui->setupUi(this);


	Init();
	slotInit();
	initLanguage();
	retranslateUi();

}

PowerlineDeviceName::~PowerlineDeviceName()
{
	qDebug() << "PowerlineDeviceName exit";
	delete ui;
}
//===============================================
//  public funtion
//===============================================
void PowerlineDeviceName::InitializedData(const QString &data, const QString &type)
{

	Q_UNUSED(type)
	m_name = (data.isEmpty()) ? "un-named" : data;
	ui->edt_newname->setText(m_name);

	ui->but_close->setFocus();

	//if(type == "XAV5001" || type == "XWN5001" )
	//{
	//}
	//else
	//{
	//	ui->edt_newname->setValidator(new QRegExpValidator(QRegExp(QString("([A-Z]|[a-z]|[0-9]){%1}").arg(TEXT_MAX_LEN)), this));
	//}
}
//===============================================
//  slot funtion
//===============================================

void PowerlineDeviceName::slot_btn_close()
{
	emit signPDNok("");

	this->reject();
}
void PowerlineDeviceName::slot_btn_ok()
{
	QString strName;



	strName = ui->edt_newname->text();
	emit signPDNok(strName);

	this->accept();
}
void PowerlineDeviceName::slot_text(QString data)
{

	if(data != m_name)
	{
		ui->but_OK->setDisabled(data.isEmpty());
	}
	else
	{
		ui->but_OK->setDisabled(true);
	}
}
//===============================================
//  Initialization funtion
//===============================================
void PowerlineDeviceName::initLanguage()
{
	POWERLINE_TRANSLATE_LABEL(ui->label_title, L_POWERLINE_DEVNAME_TITLE, "Modify Device Name");
	//POWERLINE_TRANSLATE_LABEL(ui->lab_newname, L_POWERLINE_DEVNAME_TEXT, "Device Name:");
	POWERLINE_TRANSLATE_BUTTON(ui->but_close, L_POWERLINE_BUT_CANCEL, "Cancel");
	POWERLINE_TRANSLATE_BUTTON(ui->but_OK, L_POWERLINE_BUT_OK, "Ok");
}
void PowerlineDeviceName::slotInit()
{
	connect(ui->but_close, SIGNAL(clicked()), this, SLOT(slot_btn_close()));
	connect(ui->but_OK, SIGNAL(clicked()), this, SLOT(slot_btn_ok()));

	connect(ui->edt_newname, SIGNAL(textChanged(QString)), this, SLOT(slot_text(QString)));
}
void PowerlineDeviceName::Init()
{
	ui->but_OK->setDisabled(true);

	ui->edt_newname->setMaxLength(TEXT_MAX_LEN);


	//////Value clear
	ui->edt_newname->clear();
	m_name.clear();

	/*ui->edt_newname->setValidator(new QRegExpValidator(QRegExp("([A-Z]|[a-z]|[0-9]){11}"), this));*/

}

//===============================================
//  other funtion
//===============================================
void PowerlineDeviceName::retranslateUi()
{

	QString strDevName = POWERLINE_GET_TEXT(L_POWERLINE_DEVNAME_TEXT, "Device Name:");
	strDevName += ":";
	ui->lab_newname->setText(strDevName);
}
//====================================================
//  event function
//====================================================
void PowerlineDeviceName::mouseMoveEvent(QMouseEvent *evt)
{
	if((evt->buttons() & Qt::LeftButton) && m_bMove && !isFullScreen())
	{
		move(evt->globalPos() - m_position);
		evt->accept();
	}
}
void PowerlineDeviceName::mousePressEvent(QMouseEvent *evt)
{
	if(evt->button() == Qt::LeftButton)
	{
		m_bMove = true;
		m_position = evt->globalPos() - frameGeometry().topLeft();
	}
}
void PowerlineDeviceName::mouseReleaseEvent(QMouseEvent *evt)
{
	if(evt->button() == Qt::LeftButton)
	{
		m_bMove = false;
	}
}
void PowerlineDeviceName::changeEvent(QEvent *evt)
{
	if(evt->type() == QEvent::LanguageChange)
	{
		retranslateUi();
	}
	QDialog::changeEvent(evt);
}
