#include "newdevicenotifywidget.h"
#include "language_Map.h"
 

#include <QTextEdit>
#include <QLabel>
#include <QPushButton>
#include <QEvent>
#include <QPainter>
#include <QMouseEvent>
#include <QFile>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QApplication>
#include <QPropertyAnimation>
#include <QDesktopWidget>
#include <QTimerEvent>
#include "genie2_interface.h"

#define NDNW_BACKGROUDIMAGE "map/others/NewDeviceNotifyWidgetBk.png"
#define NEWDEVICENOTIFYWIDGET_STYLEFILE "ui/newdevicenotifywidget.qss"
#define DELAYINTERVAL_MS 1000
#define DELAY_COUNTER_MAX 15

NewDeviceNotifyWidget::NewDeviceNotifyWidget(QWidget *parent) :
        QWidget(parent),
        m_canMove(false),
        m_notifyAnimation(0),
        m_timeoutCounter(DELAY_COUNTER_MAX),
        m_timerId(0)
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::ToolTip);
    setAttribute(Qt::WA_TranslucentBackground);


    m_closeBtn = new QPushButton(this);
    m_closeBtn->setFlat(true);
    m_okBtn = new QPushButton(this);
//    SET_STYLE_PROPERTY(FLEX_BUTTON,m_okBtn);
    m_closeBtn->setObjectName("NewDeviceNotifyWidgetCloseButton");

    m_titleLabel = new QLabel(this);
    m_titleContentLabel = new QLabel(this);
    m_bodyEdit = new QTextEdit(this);
    m_bodyEdit->setObjectName("NewDeviceNotifyWidgetEdit");

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
    btnLayout->addStretch(1);
    btnLayout->addWidget(m_closeBtn);

    QHBoxLayout *okbtnLayout = new QHBoxLayout();
    okbtnLayout->setContentsMargins(0,0,0,0);
    okbtnLayout->addStretch(1);
    okbtnLayout->addWidget(m_okBtn);

    QHBoxLayout *titleLayout = new QHBoxLayout();
    titleLayout->setContentsMargins(20,0,20,0);
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
    belowLayout->setContentsMargins(20,6,20,6);
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

    connect(m_closeBtn,SIGNAL(clicked()),this,SLOT(btnClicked()));
    connect(m_okBtn,SIGNAL(clicked()),this,SLOT(btnClicked()));

    applyStyle();
    retranslateUi();
}

NewDeviceNotifyWidget::~NewDeviceNotifyWidget()
{
    if(m_timerId != 0)
    {
        killTimer(m_timerId);
    }
}

void NewDeviceNotifyWidget::timerEvent(QTimerEvent *event)
{
	setAttribute(Qt::WA_Mapped);
    if(event->timerId() != m_timerId)
    {
        return;
    }

    if(--m_timeoutCounter <= 0)
    {
        killTimer(m_timerId);
        m_timerId = 0;
        emit timeout();
    }
    else
    {
        retranslateUi();
    }
}

void NewDeviceNotifyWidget::changeEvent(QEvent *event)
{
    if(event->type() == QEvent::LanguageChange)
    {
        retranslateUi();
    }

    QWidget::changeEvent(event);
}

void NewDeviceNotifyWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QPainter painter(this);
    painter.drawPixmap(0,0,QPixmap(GENIE2_RES(NDNW_BACKGROUDIMAGE)));
}

void NewDeviceNotifyWidget::retranslateUi()
{
    QString tmpText = translateText(L_MAP_YES,QString("OK"));
    if(m_timeoutCounter > 0)
    {
        tmpText += "(";
        tmpText += QString::number(m_timeoutCounter);
        tmpText += ")";
    }
    m_okBtn->setText(tmpText);
    m_titleLabel->setText(translateText(L_MAP_NEWDEVICE,QString("New device")));
    m_bodyEdit->setText(translateText(L_MAP_NEWDEVICENOTIFY_MSG,QString("Default content too long to place here")));
}

void NewDeviceNotifyWidget::applyStyle()
{
    QFile cssFile(GENIE2_RES(NEWDEVICENOTIFYWIDGET_STYLEFILE));
    if (cssFile.open(QIODevice::ReadOnly)) {
        setStyleSheet(QString::fromUtf8(cssFile.readAll().data()));
    }
}

QString NewDeviceNotifyWidget::translateText(int idx, const QString &defaultText)
{
    QString text;
    emit translateText(idx,&text);
    return text.isEmpty() ? defaultText : text;
}

void NewDeviceNotifyWidget::mousePressEvent(QMouseEvent *event)
{
    m_pos = event->globalPos();
    m_canMove = true;
}

void NewDeviceNotifyWidget::mouseReleaseEvent(QMouseEvent *)
{
    m_canMove = false;
}

void NewDeviceNotifyWidget::mouseMoveEvent(QMouseEvent *event)
{
    if(m_canMove && (event->buttons() & Qt::LeftButton))
    {
        move(pos().x() - (m_pos.x() - event->globalPos().x()),
             pos().y() - (m_pos.y() - event->globalPos().y()));

        m_pos = event->globalPos();
    }
}

void NewDeviceNotifyWidget::btnClicked()
{
    if(m_timerId)
    {
        killTimer(m_timerId);
        m_timerId = 0;
    }
    emit completed(m_timeoutCounter);
}

void NewDeviceNotifyWidget::notifyNewDevice(const QString &title,int defaultCount)
{
    m_titleContentLabel->setText(title);

    setVisible(true);

    QRect rect = QApplication::desktop()->screenGeometry(-1);
    int x = rect.width() - width() - 10;

    if(!m_notifyAnimation)
    {
        m_notifyAnimation = new QPropertyAnimation(this, "geometry",this);
    }

    m_notifyAnimation->setDuration(1000);
    m_notifyAnimation->setStartValue(QRect(x, rect.height(), width(), height()));
    m_notifyAnimation->setEndValue(QRect(x, rect.height() - height() - 40, width(), height()));
    m_notifyAnimation->start();

    //timer
    if(m_timerId != 0)
    {
        killTimer(m_timerId);
        m_timerId = 0;
        m_timeoutCounter = (defaultCount <= 0 ? DELAY_COUNTER_MAX : defaultCount);
    }
    m_timerId = startTimer(DELAYINTERVAL_MS);
}
