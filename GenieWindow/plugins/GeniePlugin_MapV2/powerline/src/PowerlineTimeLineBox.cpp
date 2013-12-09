#include "PowerlineTimeLineBox.h"
#include "ui_PowerlineTimeLineBox.h"
#include "language_Map.h"
#include "PowerlineDefine.h"

PowerlineTimeLineBox::PowerlineTimeLineBox(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PowerlineTimeLineBox),
	m_applyWaitTime(0),
	m_isEmit(false),
	m_titleID(-1),
	m_contentID(-1)
{
	init();


    ui->setupUi(this);
	initLanguage();
}

PowerlineTimeLineBox::~PowerlineTimeLineBox()
{

	m_TimerLine.stop();


	if(!m_isEmit)
	{
		emit dlgOver(0);  //cancel
	}

    delete ui;
}


void PowerlineTimeLineBox::init()
{
	m_TimerLine.setFrameRange(0, 20000);
	m_TimerLine.setUpdateInterval(400);
	m_TimerLine.setCurveShape(QTimeLine::LinearCurve);
	connect(&m_TimerLine, SIGNAL(frameChanged(int)), this, SLOT(slot_TimeLineOut(int)));
	connect(&m_TimerLine, SIGNAL(finished()), this, SLOT(slot_TimeLineFinish()));
}
void PowerlineTimeLineBox::initLanguage()
{
	//POWERLINE_TRANSLATE_BUTTON(ui->but_ok, L_POWERLINE_BUT_OK, "OK");
	//POWERLINE_TRANSLATE_BUTTON(ui->but_cancel, L_POWERLINE_BUT_CANCEL, "Cancel");
}

void PowerlineTimeLineBox::retranslateUi()
{
	QString strTitle = POWERLINE_GET_TEXT(m_titleID, "Information");
	QString strMsg = POWERLINE_GET_TEXT(m_contentID, "Please Wait...");
	ui->label_title->setText(strTitle);
	ui->lab_content->setText(strMsg);
}

void PowerlineTimeLineBox::showTimeLineDlg(int titleID, int contentID, unsigned int timeNum)
{
	//this->setWindowFlags(Qt::SubWindow);
	//this->raise();

	m_titleID	= titleID;
	m_contentID = contentID;
	retranslateUi();

	//initValue();
	ui->lab_content->setAlignment(Qt::AlignCenter);
	ui->but_cancel->hide();
	ui->but_ok->show();
	ui->but_ok->setDisabled(true);
	//calculateDlgSize(content);


	m_TimerLine.setDuration(timeNum * 1000);
	m_applyWaitTime = timeNum * 1000;
	m_TimerLine.setFrameRange(0, timeNum * 1000);
	m_TimerLine.setStartFrame(0);
	m_TimerLine.setDirection(QTimeLine::Forward);
	m_TimerLine.start();
}
void PowerlineTimeLineBox::slot_TimeLineOut(int sec)
{
	int remain	= (m_applyWaitTime - sec) / 1000 + 1;
	remain		= (remain == 0) ? 1 : remain;

	QString mode("%1 Sec");
	QString msg = mode.arg(QString::number(remain));

	//ui->lab_content->setText(msg);
	ui->but_ok->setText(msg);
}
void PowerlineTimeLineBox::slot_TimeLineFinish()
{
	m_TimerLine.stop();

	m_isEmit = true;
	emit dlgOver(1);
}

void PowerlineTimeLineBox::changeEvent(QEvent *evt)
{
	if(evt->type() == QEvent::LanguageChange)
	{
		retranslateUi();
	}

	QWidget::changeEvent(evt);
}