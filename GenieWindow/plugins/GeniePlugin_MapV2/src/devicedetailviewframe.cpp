#include "devicedetailviewframe.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSpacerItem>
#include <QFrame>
#include <QFile>
#include <QMouseEvent>
#include <QtDebug>
#include <QKeyEvent>
#include <QApplication>
#include "genie2_interface.h"

#define DETAILVIEWFRAME_STYLEFILE "ui/detailviewframe.qss"
#define FRAMETITLE_HEIGHT   36
#define FRAMETITLE_INDENT   50
#define CORNER_MARGIN       36
#define FRAME_MINISIZE      QSize(420,360)

DeviceDetailViewFrame::DeviceDetailViewFrame(QWidget* content,QWidget *parent) :
        QWidget(parent),
        m_logoLabel(0),
        m_frameTitleLabel(0),
        m_contentWidget(content),
        m_frameContainer(0)
{
    prepareUi();
    initFrame();
    applyStyle();

}

void DeviceDetailViewFrame::setFrameTitle(const QString &title)
{
    m_frameTitleLabel->setText(title);
}

void DeviceDetailViewFrame::setFrameLogo(const QPixmap &logo)
{
    if(qApp->layoutDirection() == Qt::LeftToRight)
    {
        m_logoLabel->setGeometry(0,0,logo.width(),logo.height());
    }
    else
    {
        m_logoLabel->setGeometry(width() - logo.width(),0,logo.width(),logo.height());
    }

    m_logoLabel->setPixmap(logo);
}

void DeviceDetailViewFrame::prepareUi()
{
    this->setFocusProxy(m_contentWidget);
    //m_contentWidget->setFocusPolicy(Qt::StrongFocus);
    //m_contentWidget->setFocus();

    this->setAttribute(Qt::WA_TranslucentBackground);

    this->setWindowFlags(Qt::SubWindow);
	




    resize(FRAME_MINISIZE);
}

void DeviceDetailViewFrame::initFrame()
{
    m_frameContainer = new QFrame(this);
    m_frameContainer->setFocusPolicy(Qt::NoFocus);

    m_frameContainer->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
#ifdef Q_OS_WIN32
    m_frameContainer->setObjectName(QString("frameContainer"));
#elif defined Q_OS_MACX
    m_frameContainer->setObjectName(QString("frameContainerMacx"));
#endif
    //m_frameContainer->setAutoFillBackground(false);
	//m_frameContainer->setStyleSheet("background-color:red");

    m_contentWidget->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

    m_frameTitleLabel = new QLabel(m_frameContainer);
    m_frameTitleLabel->setObjectName(QString("frameTitleLabel"));
    m_frameTitleLabel->setFixedHeight(FRAMETITLE_HEIGHT);
    m_frameTitleLabel->setAlignment(Qt::AlignVCenter);
    QSpacerItem  *titleIdentItem = new QSpacerItem(FRAMETITLE_INDENT,0,QSizePolicy::Fixed);
    QHBoxLayout  *titleLayout  = new QHBoxLayout();
    titleLayout->addItem(titleIdentItem);
    titleLayout->addWidget(m_frameTitleLabel);


    QVBoxLayout   *containerLayout = new QVBoxLayout(m_frameContainer);
#ifdef Q_OS_MACX
    containerLayout->setContentsMargins(0,2,0,0);
#else
    containerLayout->setContentsMargins(0,6,0,0);
#endif
    containerLayout->setSpacing(1);

    containerLayout->addLayout(titleLayout);
    containerLayout->addWidget(m_contentWidget);


    QHBoxLayout *horLayout = new QHBoxLayout();
    horLayout->setContentsMargins(0,0,0,0);
    horLayout->setSpacing(0);
    QSpacerItem  *leftSpaceItem = new QSpacerItem(CORNER_MARGIN,0,QSizePolicy::Fixed);
    horLayout->addItem(leftSpaceItem);;
    horLayout->addWidget(m_frameContainer);
    QSpacerItem  *topSpaceItem = new QSpacerItem(0,CORNER_MARGIN,QSizePolicy::Fixed);
    QVBoxLayout  *frameLayout = new QVBoxLayout(this);
    frameLayout->setContentsMargins(0,0,0,0);
    frameLayout->setSpacing(0);
    frameLayout->addItem(topSpaceItem);
    frameLayout->addLayout(horLayout);

    m_logoLabel = new QLabel(this);
}

void DeviceDetailViewFrame::applyStyle()
{
    QFile cssFile(GENIE2_RES(DETAILVIEWFRAME_STYLEFILE));
    if (cssFile.open(QIODevice::ReadOnly)) {
        setStyleSheet(QString::fromUtf8(cssFile.readAll().data()));
    }
}


//void DeviceDetailViewFrame::mousePressEvent(QMouseEvent *event)
//{
//    m_pos = event->globalPos();
//    m_canMove = true;
//}

//void DeviceDetailViewFrame::mouseReleaseEvent(QMouseEvent *)
//{
//    m_canMove = false;
//}

//void DeviceDetailViewFrame::mouseMoveEvent(QMouseEvent *event)
//{
////    if(m_canMove && (event->buttons() & Qt::LeftButton))
////    {
////        move(pos().x() - (m_pos.x() - event->globalPos().x()),
////             pos().y() - (m_pos.y() - event->globalPos().y()));

////        m_pos = event->globalPos();
////    }
//}



void DeviceDetailViewFrame::centerInParent()
{
    show();

    QWidget *viewParent = qobject_cast<QWidget*>(parent());
    if(viewParent)
    {
        move((viewParent->width() - m_frameContainer->width()) / 2 - CORNER_MARGIN,
             (viewParent->height() - m_frameContainer->height()) / 2 - CORNER_MARGIN);
    }
}
