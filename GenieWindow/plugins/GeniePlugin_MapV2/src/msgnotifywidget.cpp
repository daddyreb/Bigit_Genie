#include "msgnotifywidget.h"
#include <QClipBoard>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QTextEdit>
#include <QRect>
#include <QPropertyAnimation>
#include <QApplication>
#include <QDesktopWidget>
#include <QEvent>
#include <QPainter>
#include <QPainter>
#include <QMouseEvent>

#include "genie2_interface.h"
#include "language_Map.h"

#define NDNW_BACKGROUDIMAGE "map/others/NewDeviceNotifyWidgetBk.png"
#define NEWDEVICENOTIFYWIDGET_STYLEFILE "ui/newdevicenotifywidget.qss"

#define DELAY_COUNTER_MAX 10
#define DELAYINTERVAL_MS 1000

msgnotifywidget::msgnotifywidget(QWidget *parent) :
    QWidget(parent),
	m_canMove(false),
	m_timeoutCounter(DELAY_COUNTER_MAX),
	m_notifyAnimation(NULL),
	m_timerId(0)
{
	setWindowFlags(Qt::FramelessWindowHint | Qt::ToolTip);
    setAttribute(Qt::WA_TranslucentBackground);
	setAttribute(Qt::WA_DeleteOnClose);

	m_closeBtn = new QPushButton(this);
    m_closeBtn->setFlat(true);
	m_closeBtn->setObjectName("NewFileNotifyWidgetCloseButton");
	m_okBtn = new QPushButton(this);
	connect(m_okBtn,SIGNAL(clicked()),this,SLOT(okBtnClicked()));

	m_titleLabel = new QLabel(this);
    m_titleContentLabel = new QLabel(this);
    m_bodyEdit = new QTextEdit(this);
    m_bodyEdit->setObjectName("NewDeviceNotifyWidgetEdit");
    m_bodyEdit->setEnabled(true);
    m_bodyEdit->setReadOnly(true);
	m_titleLabel->setObjectName("NewDeviceNotifyWidgetTitle");

	m_titleLabel->setAlignment(Qt::AlignLeft | Qt::AlignBottom);
    m_titleContentLabel->setAlignment(Qt::AlignRight | Qt::AlignBottom);
    m_bodyEdit->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

    QPixmap pixDetecter(GENIE2_RES(NDNW_BACKGROUDIMAGE) );
    setFixedSize(pixDetecter.size());

    QHBoxLayout *btnLayout = new QHBoxLayout();
#ifdef Q_OS_MAC
    btnLayout->setContentsMargins(0,2,0,0);
#endif
//    btnLayout->addStretch(1);
    btnLayout->addWidget(m_closeBtn);

	QHBoxLayout *okbtnLayout = new QHBoxLayout();
    okbtnLayout->setContentsMargins(0,0,0,0);
    okbtnLayout->addStretch(1);
	okbtnLayout->addWidget(m_okBtn);

	QHBoxLayout *titleLayout = new QHBoxLayout();
    titleLayout->setContentsMargins(20,0,20,0);
    //titleLayout->addWidget(m_progressBar);
    titleLayout->addWidget(m_titleLabel);
    titleLayout->addWidget(m_titleContentLabel);

    QVBoxLayout *aboveLayout = new QVBoxLayout();
#ifdef Q_OS_MAC
    aboveLayout->setContentsMargins(0,0,0,0);
#endif
    aboveLayout->setSpacing(9);
    aboveLayout->addLayout(btnLayout);
    aboveLayout->addLayout(titleLayout);

    QVBoxLayout *belowLayout = new QVBoxLayout();
    belowLayout->setContentsMargins(10,6,10,6);
    belowLayout->addWidget(m_bodyEdit);
    belowLayout->addLayout(okbtnLayout);

    QVBoxLayout *layOut = new QVBoxLayout(this);
#ifdef Q_OS_MAC
    layOut->setContentsMargins(9,2,4,2);
#else
    layOut->setContentsMargins(9,12,9,2);
#endif
    layOut->addLayout(aboveLayout);
    layOut->addLayout(belowLayout);

	
	applyStyle();
    retranslateUi();
}

void msgnotifywidget::notifyNewDevice(QString msg, QString host)
{
	QString strText;
	strText += "Receive message \"";
	strText += msg;
	strText += "\" from ";
	strText += host;
	strText += " and auto save to clipboard";

	m_bodyEdit->setText(strText);
	m_titleLabel->setText("Turbo Transfer");

	QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(msg);

	if(!m_notifyAnimation)
	{
		m_notifyAnimation = new QPropertyAnimation(this, "geometry",this);
	}

	setVisible(true);
	QRect rect = QApplication::desktop()->screenGeometry(-1);
	int x = rect.width() - width() - 10;

	m_notifyAnimation->setDuration(1000);
	m_notifyAnimation->setStartValue(QRect(x, rect.height(), width(), height()));
	m_notifyAnimation->setEndValue(QRect(x, rect.height() - height() - 40, width(), height()));
	m_notifyAnimation->start();

	if (0 == m_timerId)
	{
		m_timerId = startTimer(DELAYINTERVAL_MS);
	}
}

void msgnotifywidget::timerEvent(QTimerEvent *event)
{
    if(event->timerId() != m_timerId)
    {
        return;
    }

    if(--m_timeoutCounter <= 0)
    {
        killTimer(m_timerId);
        m_timerId = 0;
        close();
    }
    else
    {
        retranslateUi();
    }
}

void msgnotifywidget::changeEvent(QEvent *event)
{
    if(event->type() == QEvent::LanguageChange)
    {
        retranslateUi();
    }

    QWidget::changeEvent(event);
}

void msgnotifywidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QPainter painter(this);
    painter.drawPixmap(0,0,QPixmap(GENIE2_RES(NDNW_BACKGROUDIMAGE)));
}

void msgnotifywidget::mousePressEvent(QMouseEvent *event)
{
    m_pos = event->globalPos();
    m_canMove = true;
}

void msgnotifywidget::mouseReleaseEvent(QMouseEvent *)
{
    m_canMove = false;
}

void msgnotifywidget::mouseMoveEvent(QMouseEvent *event)
{
    if(m_canMove && (event->buttons() & Qt::LeftButton))
    {
        move(pos().x() - (m_pos.x() - event->globalPos().x()),
             pos().y() - (m_pos.y() - event->globalPos().y()));

        m_pos = event->globalPos();
    }
}


void msgnotifywidget::retranslateUi()
{
	QString tmpText = "OK";
	if(m_timeoutCounter > 0)
	{
		tmpText += "(";
		tmpText += QString::number(m_timeoutCounter);
		tmpText += ")";
	}
	m_okBtn->setText(tmpText);

}

void msgnotifywidget::applyStyle()
{
    QFile cssFile(GENIE2_RES(NEWDEVICENOTIFYWIDGET_STYLEFILE));
    if (cssFile.open(QIODevice::ReadOnly)) {
        setStyleSheet(QString::fromUtf8(cssFile.readAll().data()));
    }
}

void msgnotifywidget::okBtnClicked()
{
	if(m_timerId)
	{
		killTimer(m_timerId);
		m_timerId = 0;
	}
	close();
}