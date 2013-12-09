#include "stable.h"



QGenieWaterWindow::QGenieWaterWindow(QWidget *parent)
    :QWidget(parent)
{
    QDesktopWidget* desktop = QApplication::desktop();
    move((desktop->width() - this->width())/2,
                            (desktop->height() - this->height())/2);
    this->setWindowTitle(tr("Draw Water"));
    setMouseTracking( true );
    


    m_thread = new QGenieAboutThread();
    connect(m_thread, SIGNAL(sendupdate()), this, SLOT(update()));
    connect(m_thread,SIGNAL(signal_ResizeWnd(QSize)),this,SLOT(slot_SetSize(QSize)));

   // setFixedSize(m_thread->mImage1.size());
    m_thread->stopThread();
    m_thread->start();


}

QGenieWaterWindow::~QGenieWaterWindow()
{
    m_thread->startThread();
    m_thread->selfstop();
    m_thread->wait();
    m_thread->mDrawMutex.lock();
    SAFE_DELETE(m_thread) ;
}
void QGenieWaterWindow::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);

    if(m_thread)
    {
        m_thread->mDrawMutex.lock();
        QPainter p(this);
        int h=m_thread->mImage1.height();
        int w=m_thread->mImage1.width();
        if(STATIC_TITLE >0)
        {
            p.drawImage(0,0,m_thread->mImage1,0,0,w,STATIC_TITLE);
        }


        p.drawImage(0, STATIC_TITLE, m_thread->mImage2,
                    0,STATIC_TITLE,w,h-STATIC_TITLE);
//        p.setPen(Qt::yellow);
//        p.drawLine(0,sStaticTitle,w,sStaticTitle);
        m_thread->mDrawMutex.unlock();
    }
}


void QGenieWaterWindow::mouseMoveEvent(QMouseEvent *event)
{
    QPoint point = event->pos();
    if(point.y()>STATIC_TITLE)
    {
        m_thread->addWave(point.x() - 15,point.y(), 5, 400);
    }
    QWidget::mouseMoveEvent(event);
}

void QGenieWaterWindow::hideEvent ( QHideEvent * event )
{
    if(m_thread)
    {
        m_thread->stopThread();
    }
}

void QGenieWaterWindow::showEvent ( QShowEvent * event )
{
    if(m_thread)
    {
        m_thread->startThread();
    }
}

void QGenieWaterWindow::slot_SetSize(const QSize &siz )
{
    setFixedSize(siz);
	qDebug()<<" QGenieWaterWindow::slot_SetSize"<<siz;
}
