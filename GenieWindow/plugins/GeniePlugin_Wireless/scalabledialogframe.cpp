#include "scalabledialogframe.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSpacerItem>
#include <QFrame>
#include <QFile>
#include <assert.h>
#include "genie2_interface.h"

#define SCALABLEDIALOG_STYLEFILE ":/css/default.css"

ScalableDialogFrame::ScalableDialogFrame(ScalableDialogContentWidget* content,QWidget *parent) :
        QDialog(parent),
        m_content_widget(content),
        m_bDrag(false)
{
    init_dim();

    init_frame();

    init_container();

    applystyle();

}

void ScalableDialogFrame::setFrameTitle(const QString &title)
{
    m_dlgtitle_lb->setText(title);
}

void ScalableDialogFrame::init_dim()
{
    m_dlg_width                     = 228;
    m_dlg_height                    = 117;
    m_dlgtitle_lb_height            = 26;
    m_dlgtitle_lb_spaceitemw        = 45;
    m_frame_lr_margin               = 10;
    m_frame_tb_margin               = 11;

    resize(m_dlg_width,m_dlg_height);
}

void ScalableDialogFrame::init_frame()
{
    this->setFocusProxy(m_content_widget);
    m_content_widget->setFocusPolicy(Qt::StrongFocus);
    GENIE2_SET_FOCUS(m_content_widget);

#ifdef Q_OS_WIN32
    this->setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);
#elif defined Q_OS_MACX
    this->setWindowFlags(Qt::Sheet | Qt::FramelessWindowHint);
#endif
    this->setAttribute(Qt::WA_TranslucentBackground);
}

void ScalableDialogFrame::init_container()
{
    QFrame *framecontainer = new QFrame(this);
    framecontainer->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
#ifdef Q_OS_WIN32
    framecontainer->setObjectName(QString("frame_container"));
#elif defined Q_OS_MACX
    framecontainer->setObjectName(QString("frame_container_macx"));
#endif

    framecontainer->setAutoFillBackground(true);

    QVBoxLayout  *dlg_layout = new QVBoxLayout(this);
    dlg_layout->setContentsMargins(0,0,0,0);
    dlg_layout->addWidget(framecontainer);

    m_dlgtitle_lb = new QLabel(framecontainer);
    m_dlgtitle_lb->setObjectName(QString("dlg_title_lb"));
    m_dlgtitle_lb->setFixedHeight(m_dlgtitle_lb_height);
    m_dlgtitle_lb->setAlignment(Qt::AlignVCenter);


    QSpacerItem  *title_spitem = new QSpacerItem(m_dlgtitle_lb_spaceitemw,0,QSizePolicy::Fixed);
    QHBoxLayout  *title_layout  = new QHBoxLayout();
    title_layout->addItem(title_spitem);
    title_layout->addWidget(m_dlgtitle_lb);


    QVBoxLayout   *container_layout = new QVBoxLayout(framecontainer);
    container_layout->setContentsMargins(m_frame_lr_margin,m_frame_tb_margin,m_frame_lr_margin,m_frame_tb_margin);
    container_layout->setSpacing(1);
    container_layout->addLayout(title_layout);
    m_content_widget->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    container_layout->addWidget(m_content_widget);

    container_layout->setStretch(0,0);
    container_layout->setStretch(1,10);
}

void ScalableDialogFrame::applystyle()
{
    QFile cssFile(QString::fromUtf8(SCALABLEDIALOG_STYLEFILE));
    if (cssFile.open(QIODevice::ReadOnly)) {
        setStyleSheet(QString::fromUtf8(cssFile.readAll().data()));
    }
}


void ScalableDialogFrame::mouseMoveEvent(QMouseEvent *event)
{
    if((event->buttons() & Qt::LeftButton) && m_bDrag)
    {
        QPoint newpos = event->globalPos();
        QPoint upleft = pos0 + newpos - last;
        this->move(upleft);
    }
}

void ScalableDialogFrame::mousePressEvent(QMouseEvent *event)
{
    last = event->globalPos();
    pos0 = event->globalPos() - event->pos();
    m_bDrag = true;
}

void ScalableDialogFrame::mouseReleaseEvent(QMouseEvent *event)
{
    m_bDrag = false;
}


//--------------------content widget--------------------

ScalableDialogContentWidget::ScalableDialogContentWidget(QWidget *parent) :
    QWidget(0),
    m_container_dlg(0)
{
    m_container_dlg = new ScalableDialogFrame(this,parent);
}

ScalableDialogContentWidget::~ScalableDialogContentWidget()
{
    m_container_dlg->deleteLater();
}

