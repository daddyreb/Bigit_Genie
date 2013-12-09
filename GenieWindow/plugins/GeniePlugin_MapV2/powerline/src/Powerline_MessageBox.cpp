#include "Powerline_MessageBox.h"
#include "ui_Powerline_MessageBox.h"
#include "Powerline_mapuitilities.h"
#include "PowerlineDefine.h"



#include <QFile>



Powerline_MessageBox::Powerline_MessageBox(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::Powerline_MessageBox),
	m_bMove(false),
	m_chkBox(false),
	m_nResult(PL_MB_CANCEL)
{
	ui->setupUi(this);

	initValue();
	initSlot();
	initDlgStyle();
	initLanguage();

	///////
	this->resize(DEFAULT_DLG_WIDTH, DEFAULT_DLG_HEIGHT);
}

Powerline_MessageBox::~Powerline_MessageBox()
{
    delete ui;
}


//===============================================
//  initialization funtion
//===============================================
void Powerline_MessageBox::initLanguage()
{
	POWERLINE_TRANSLATE_BUTTON(ui->but_ok, L_POWERLINE_BUT_OK, "OK");
	POWERLINE_TRANSLATE_BUTTON(ui->but_cancel, L_POWERLINE_BUT_CANCEL, "Cancel");
}
void Powerline_MessageBox::initValue()
{
    ui->label_title->clear();
    ui->lab_content->clear();
    ui->lab_pic->clear();

	ui->checkBox->setVisible(m_chkBox);

	m_nResult = PL_MB_CANCEL;


	//////////
	ui->lab_content->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

}
void Powerline_MessageBox::initSlot()
{
	connect(ui->but_cancel, SIGNAL(clicked()), this, SLOT(slot_btn_cancel()));
	connect(ui->but_ok, SIGNAL(clicked()), this, SLOT(slot_btn_ok()));
	connect(ui->checkBox, SIGNAL(stateChanged(int)), this, SLOT(slot_chkBox(int)));

}
void Powerline_MessageBox::initDlgStyle()
{
//    QWidget *w;
 //   m_Widget = static_cast<QWidget *>(this);
//    setFramelessStyle(m_Widget);


//    QDialog *dlg=qobject_cast<QDialog *>(w);
 //   Qt::WindowFlags flag= (this)?(Qt::Tool| Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint):(Qt::FramelessWindowHint);

 //   this->setWindowFlags(flag);
 //   this->setAttribute(Qt::WA_TranslucentBackground);

 //   //enableMove(w);

 //   QFile f(/*GENIE2_RES(*/":styles/framelessframe.qss")/*)*/;
 //   bool btemp=f.open(QIODevice::ReadOnly);
 //   Q_ASSERT(btemp);
 //   QString stemp=f.readAll();
 //   
	//f.close();

 //   this->setStyleSheet(stemp);
}
//===============================================
//  public funtion
//===============================================
void Powerline_MessageBox::setTitle(const QString &str)
{
    ui->label_title->setText(str);
}

void Powerline_MessageBox::setPic(const QString &str)
{
    if(str.isEmpty())
    {
        return;
    }

    QPixmap tPix;
    tPix.load(str);
    ui->lab_pic->setPixmap(tPix);
}
void Powerline_MessageBox::setContent(const QString &str)
{
    ui->lab_content->setText(str);
}


void Powerline_MessageBox::showDlg(const QString &title, const QString &content, int nType)
{
	initValue();
	calculateDlgSize(content);
    
	ui->label_title->setText(title);

	if(content.count() < 15)
	{
		ui->lab_content->setAlignment(Qt::AlignCenter);
	}
	

	switch(nType)
	{
	case PL_MB_OK:
		ui->but_ok->show();
		ui->but_cancel->hide();
		ui->but_ok->setFocus();
		break;
	case PL_MB_CANCEL:
		ui->but_cancel->show();
		ui->but_ok->hide();
		ui->but_cancel->setFocus();
		break;
	case (PL_MB_OK | PL_MB_CANCEL):
		ui->but_ok->show();
		ui->but_cancel->show();
		ui->but_cancel->setFocus();
		break;
	default:
		ui->but_ok->show();
		ui->but_cancel->hide();
		ui->but_ok->setFocus();
		break;
	}

//    m_Widget->show();
}
void Powerline_MessageBox::information(const QString &title, const QString &content, int nType)
{
	Q_UNUSED(title)
	Q_UNUSED(content)
	Q_UNUSED(nType)
}
//===============================================
//  other funtion
//===============================================
void Powerline_MessageBox::openChkBox(const QString &strChk)
{
	m_chkBox = true;
	//ui->checkBox->setVisible(true);

	ui->checkBox->setText(strChk);
}
void Powerline_MessageBox::calculateDlgSize(const QString &str_const)
{

    QSize retSize;
    int nWidth = 0, nHeight = 0;

    QString str(str_const);
	str.replace("\n", "");

	
	ui->lab_content->setText(str);

    QFont tmpFont(this->font());
    QFontMetrics tmpFtMe(tmpFont);
    nWidth = tmpFtMe.width(str);
	nHeight = tmpFtMe.height();
	int nLeading = tmpFtMe.leading();
	nHeight += nLeading;


	const int MIN_STR_LEN = 295;
	const int MAX_STR_LEN = 450;
	int ADD_DLG_HEIGHT = 110;

	
	ADD_DLG_HEIGHT = (m_chkBox) ? (ADD_DLG_HEIGHT + 27) : ADD_DLG_HEIGHT;
	int nDefaultHeight = (m_chkBox) ? DEFAULT_DLG_HEIGHT_EX : DEFAULT_DLG_HEIGHT;




	if(nWidth < MIN_STR_LEN)
	{
		ui->lab_content->setAlignment(Qt::AlignCenter);
		this->resize(MIN_STR_LEN + 50, nDefaultHeight);
	}
	else if(nWidth <= 2 * MIN_STR_LEN)
	{
		this->resize(MIN_STR_LEN + 50, nDefaultHeight);
	}
	else if(nWidth <= 3 * MIN_STR_LEN)
	{
		int num = nWidth / MIN_STR_LEN;
		int yNum = nWidth % MIN_STR_LEN;
		if(yNum != 0)
		{
			num += 1;
		}
		//int tmp = 2 * MIN_STR_LEN + (nWidth % MIN_STR_LEN) / 2;
		this->resize(MIN_STR_LEN + 50, ADD_DLG_HEIGHT + num * nHeight);
	}
	else
	{
		int num = nWidth / MAX_STR_LEN;
		int yNum = nWidth % MAX_STR_LEN;
		if(yNum != 0)
		{
			num += 1;
		}
		this->resize(MAX_STR_LEN + 50, ADD_DLG_HEIGHT + num * nHeight);
	}


	//if(nWidth >= 2 * DEFAULT_DLG_WIDTH)
	//{
	//	this->resize(nWidth / 2 + 15, DEFAULT_DLG_HEIGHT);
	//}
	//else
	//{
	//	this->resize(DEFAULT_DLG_WIDTH, DEFAULT_DLG_HEIGHT);
	//}


}
void Powerline_MessageBox::retranslateUi()
{
	
}
int Powerline_MessageBox::retChkboxState() const
{
	return m_nResult;
}
//===============================================
//  slot funtion
//===============================================
void Powerline_MessageBox::slot_btn_ok()
{
	m_nResult ^= PL_MB_CANCEL;
	m_nResult |= PL_MB_OK;

	this->accept();
	//setResult(DialogCode::Accepted);
 //   this->close();
}
void Powerline_MessageBox::slot_btn_cancel()
{
	m_nResult ^= PL_MB_CANCEL;
	m_nResult |= PL_MB_CANCEL;

	this->reject();
	//setResult(DialogCode::Rejected);
 //   this->close();
}
void Powerline_MessageBox::slot_chkBox(int nState)
{
	m_nResult ^= PL_MB_CANCEL;
	if(nState == Qt::Checked)
	{
		m_nResult = PL_MB_CHKED;
	}
	else
	{
		m_nResult = PL_MB_NOCHKED;
	}
}

//====================================================
//  event function
//====================================================
void Powerline_MessageBox::mouseMoveEvent(QMouseEvent *evt)
{
    if((evt->buttons() & Qt::LeftButton) && m_bMove && !isFullScreen())
    {
        move(evt->globalPos() - m_position);
        evt->accept();
    }
}
void Powerline_MessageBox::mousePressEvent(QMouseEvent *evt)
{
    qDebug()<<"Powerline_MessageBox::mousePressEvent(QMouseEvent *evt)";
    if((evt->button() == Qt::LeftButton) && !m_bMove)
    {
        m_bMove = true;
        m_position = evt->globalPos() - frameGeometry().topLeft();
    }

}
void Powerline_MessageBox::mouseReleaseEvent(QMouseEvent *evt)
{
    if(evt->button() == Qt::LeftButton)
    {
        m_bMove = false;
    }

}
void Powerline_MessageBox::changeEvent(QEvent *evt)
{
	if(evt->type() == QEvent::LanguageChange)
	{
		retranslateUi();
	}
	QDialog::changeEvent(evt);
}
