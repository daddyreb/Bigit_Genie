#include "QGenieWaterWindow.h"
#include "QGenieWaterThread.h"
#include "genie2_interface.h"
#include "language_Ui.h"



QGenieWaterWindow::QGenieWaterWindow(QWidget *parent)
    :QWidget(parent)
    ,mTopPadding (73)
{
    //    QDesktopWidget* desktop = QApplication::desktop();
    //    move((desktop->width() - this->width())/2,
    //                            (desktop->height() - this->height())/2);
    //    this->setWindowTitle(tr("Draw Water"));
    setMouseTracking( true );


    //È¡°æ±¾ºÅ
    QString t=GENIE2_TELL_OTHERPLUGIN(FRAME_UUID,I2CMD_VERSIONNUMBER,0).toString();
    //convert version number
    QStringList templist=t.split(".");
    t="";
    int c=templist.count();
    for(int i=0;i<c;i++)
    {
        bool btemp;
        int itemp=templist[i].toInt(&btemp);
        t=t+QString((t.isEmpty())?"":".")+(btemp?QString::number(itemp):templist[i]);
    }


    QString strMark = QString("%1 %2\n%3\n%4\n%5")
            .arg(GENIE2_GET_TEXT(L_UI_VERSION)).arg(t)
            .arg(GENIE2_GET_TEXT(L_UI_COPYRIGHT))
            .arg(GENIE2_GET_TEXT(L_UI_ALLRIGHTS_RESERVE))
            .arg(GENIE2_GET_TEXT(L_UI_POWERED_BY));



    bool bSMUnlocked=GENIE2_GET_SETTING(PLUGIN_SMARTNETWORK_UNLOCKED).toBool();
QString bkg=bSMUnlocked?GENIE2_RES("images/bg_about_gadd.png"):GENIE2_RES("images/bg_about.png");
    m_thread = new QGenieWaterThread(bkg,strMark,mTopPadding);
    connect(m_thread, SIGNAL(sendupdate()), this, SLOT(update()));
    connect(m_thread,SIGNAL(signal_ResizeWnd(QSize)),this,SLOT(slot_SetSize(QSize)));

    setFixedSize(m_thread->mImage1.size());
    //    m_thread->stopThread();
    m_thread->start();


}

QGenieWaterWindow::~QGenieWaterWindow()
{
    //    m_thread->startThread();
    m_thread->selfstop();
    m_thread->wait();
    delete (m_thread) ;
}

//void QGenieWaterWindow::setBG(const QString &bg)
//{
//    mBGImageName=bg;
//}

//void QGenieWaterWindow::setWaterMark(const QString &watermark)
//{
//    mWatermark=watermark;
//}

//void QGenieWaterWindow::setTopPadding(int padding)
//{
//    mTopPadding=padding;
//}

void QGenieWaterWindow::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);

    if(m_thread)
    {
        m_thread->mDrawMutex.lock();
        QPainter p(this);
        int h=m_thread->mImage1.height();
        int w=m_thread->mImage1.width();
        if(mTopPadding >0)
        {
            p.drawImage(0,0,m_thread->mImage1,0,0,w,mTopPadding);
        }


        p.drawImage(0, mTopPadding, m_thread->mImage2,
                    0,mTopPadding,w,h-mTopPadding);
        //        p.setPen(Qt::yellow);
        //        p.drawLine(0,sStaticTitle,w,sStaticTitle);
        m_thread->mDrawMutex.unlock();
    }
}


void QGenieWaterWindow::mouseMoveEvent(QMouseEvent *event)
{
    QPoint point = event->pos();
    if(point.y()>mTopPadding)
    {
        m_thread->addWave(point.x() - 15,point.y(), 5, 400);
    }
    QWidget::mouseMoveEvent(event);
}

//void QGenieWaterWindow::hideEvent ( QHideEvent * event )
//{
//    Q_UNUSED(event);
//    if(m_thread)
//    {
//        m_thread->stopThread();
//    }
//}

//void QGenieWaterWindow::showEvent ( QShowEvent * event )
//{
//    Q_UNUSED(event);
//    if(m_thread)
//    {
//        m_thread->startThread();
//    }
//}

void QGenieWaterWindow::slot_SetSize(const QSize &siz )
{
    setFixedSize(siz);
    qDebug()<<" QGenieWaterWindow::slot_SetSize"<<siz;
}
