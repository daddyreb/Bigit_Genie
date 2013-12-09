#include "PowerlinePrompt.h"
#include "ui_PowerlinePrompt.h"
#include <QMouseEvent>
#include <QDebug>
#include <QPixmap>
#include "PowerlineDefine.h"

PowerlinePrompt::PowerlinePrompt(QWidget *parent) :
QDialog(parent),
ui(new Ui::PowerlinePrompt)
,m_bMove(false)
{
	ui->setupUi(this);

	init();
	initLanguage();
}

PowerlinePrompt::~PowerlinePrompt()
{
	qDebug() << "PowerlinePrompt exit";
	delete ui;
}
//====================================================
//  Initialization function
//====================================================
void PowerlinePrompt::initLanguage()
{
	POWERLINE_TRANSLATE_BUTTON(ui->but_cancel, L_POWERLINE_BUT_CANCEL, "Cancel");
	POWERLINE_TRANSLATE_BUTTON(ui->but_ok, L_POWERLINE_BUT_OK, "Ok");
}
void PowerlinePrompt::init()
{
	connect(ui->but_cancel, SIGNAL(clicked()), this, SLOT(btn_cancel()));
	connect(ui->but_ok, SIGNAL(clicked()), this, SLOT(btn_ok()));
}
void PowerlinePrompt::clearData()
{

}
//====================================================
//  Slot function
//====================================================
void PowerlinePrompt::btn_cancel()
{
	this->reject();
}
void PowerlinePrompt::btn_ok()
{
	emit btnOK();
	this->accept();
}

//====================================================
//  public function
//====================================================
void PowerlinePrompt::setTitle(QString strTitle)
{
	ui->label_title->setText(strTitle);
}
void PowerlinePrompt::setPic(QString strPath)
{
	QPixmap tPix;
	tPix.load(strPath);
	ui->lab_pic->setPixmap(tPix);
}
void PowerlinePrompt::setContext(QString strTip)
{
	ui->lab_context->setText(strTip);

	ui->but_cancel->setFocus();
}
//====================================================
//  event function
//====================================================
void PowerlinePrompt::mouseMoveEvent(QMouseEvent *evt)
{
	if((evt->buttons() & Qt::LeftButton) && m_bMove && !isFullScreen())
	{
		move(evt->globalPos() - m_position);
		evt->accept();
	}
}
void PowerlinePrompt::mousePressEvent(QMouseEvent *evt)
{
	if(evt->button() == Qt::LeftButton)
	{
		m_bMove = true;
		m_position = evt->globalPos() - frameGeometry().topLeft();
	}
}
void PowerlinePrompt::mouseReleaseEvent(QMouseEvent *evt)
{
	if(evt->button() == Qt::LeftButton)
	{
		m_bMove = false;
	}
}
