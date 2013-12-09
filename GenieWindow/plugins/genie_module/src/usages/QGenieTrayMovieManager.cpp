#include "stable.h"

QGenieTrayMovieManager::QGenieTrayMovieManager(QGenieTrayIco_In *traymanager)
    :mTrayIco(traymanager)
    ,mOn(false)
{

    mTimeLine.setFrameRange(0, 100);
    mTimeLine.setUpdateInterval(200);
    connect(&mTimeLine,SIGNAL(frameChanged(int)),this,SLOT(slot_FrameUpdate(int)));
//    connect(&mTimeLine,SIGNAL(finished()),this,SLOT(slot_Finished()));

}

void QGenieTrayMovieManager::startMovie(int msec)
{

    QGenieFrameMain *main=QGenieFrameMain::GetSingletonPtr();
    if(!main){
        return;
    }
#if MOVIE_PROPOSAL==0
    mMovie.setFileName(filename);
#endif


    mTimeLine.setStartFrame(0);
    mTimeLine.setDirection(QTimeLine::Forward);
    if(msec==0)
    {
        mTimeLine.setLoopCount(1000000);
        mTimeLine.setDuration(10000);
    }
    else
    {
        mTimeLine.setDuration(msec);
    }
    mTimeLine.start();
    mOn=true;
   // mMovie.start();
}

void QGenieTrayMovieManager::stopMovie()
{
    QGenieFrameMain *main=QGenieFrameMain::GetSingletonPtr();
    if(!main){
        return;
    }
    if(mTimeLine.state()==QTimeLine::Running || mTimeLine.state()==QTimeLine::Paused)
    {
        mTimeLine.stop();
        mOn=false;
        mTrayIco->setIcon(mTrayIco->mIcoCache4Movie);

        main->setWindowIcon(QIcon(mTrayIco->mIcoCache4Movie));
    }
}

bool QGenieTrayMovieManager::isOn()
{
    return mOn;
}

void QGenieTrayMovieManager::slot_FrameUpdate(int val)
{
    qDebug()<<"QGenieTrayMovieManager::slot_FrameUpdate";
    QGenieFrameMain *main=QGenieFrameMain::GetSingletonPtr();
    if(!main){
        return;
    }

#if MOVIE_PROPOSAL==0

//    static bool btest=true;
//    if(btest)
//    {
        mMovie.jumpToNextFrame();
//        btest=false;
//    }
    mTrayManager->mTrayIcon->setIcon(QIcon(mMovie.currentPixmap()));
    qDebug()<<mMovie.frameCount()<<mMovie.currentFrameNumber();
#elif MOVIE_PROPOSAL ==1
    static int ro=0;
    ro+=1;
    ro=ro%(12);
    QString img=QString(":/movie/%1.ico").arg(QString::number(ro+1));
    mTrayIco->mTrayIcon->setIcon(QIcon(img));


    ASSERT(main);
    main->setWindowIcon(QIcon(img));

//    QApplication::setWindowIcon(QIcon(img));



#endif
}




