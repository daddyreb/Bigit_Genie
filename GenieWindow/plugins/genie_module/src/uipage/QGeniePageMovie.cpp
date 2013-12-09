#include "stable.h"


#define MOVIE_TIME 10
#define FRAME_RANGE 100
#define UPDATE_INTERVAL 90
const qreal cMinSize=0.60;
const int cSizeHint=70;
const int threadhood=((FRAME_RANGE-cSizeHint)>>1);

qreal calSize(int i)
{
	qreal ret=0.0;

	if(i<threadhood)
	{
		ret=(threadhood-i)*(1-cMinSize)/threadhood+cMinSize;
	}
	else if (FRAME_RANGE-i<threadhood)
	{
		ret=(threadhood-(100-i))*(1-cMinSize)/threadhood+cMinSize;
	}
	else
	{
		ret=cMinSize;
	}
	return ret;

};

qreal calPos(int i,qreal siz)
{

	QGenieFrameMain *framemain=QGenieFrameMain::GetSingletonPtr();
	ASSERT(framemain);
	QSize stack_sz=framemain->getStackSize();
	int w=stack_sz.width();
	qreal ret=0.0;

	if(i<threadhood)
	{
		ret=(1-siz)*stack_sz.width()/2;
	}
	else if (FRAME_RANGE-i<threadhood)
	{
		ret=(1+siz)*(-w)/2;;
	}
	else
	{
		ret=(i-threadhood)*w/(2*threadhood-FRAME_RANGE)+w*(1-siz)/2;
	}
	return ret;


};

QGeniePageMovie::QGeniePageMovie(QWidget *parent)
    :QGraphicsView(parent)
    ,mItemCurPage(NULL)
    ,mItemNextPage(NULL)
    ,mScene(NULL)

{
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setCacheMode(CacheBackground);
    setViewportUpdateMode(FullViewportUpdate);
    setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform
                   | QPainter::TextAntialiasing);


    mScene = new QGraphicsScene( this);
    mScene->setItemIndexMethod(QGraphicsScene::NoIndex);
    setScene(mScene);

    mTimeLine=new QTimeLine(MOVIE_TIME,this);
    mTimeLine->setFrameRange(0, FRAME_RANGE);
    mTimeLine->setUpdateInterval(UPDATE_INTERVAL);
    connect(mTimeLine,SIGNAL(frameChanged(int)),this,SLOT(slot_FrameUpdate(int)));
    connect(mTimeLine,SIGNAL(finished()),this,SLOT(slot_Finished()));

    setProperty(STYLE_PROPERTY_HINT_SUB,SHSE_FRAME_MOVIE);
}

void QGeniePageMovie::initPixmap()
{
    QGenieStackManager *manager=QGenieStackManager::GetSingletonPtr();
    ASSERT(manager);

    mScene->clear();

    QWidget *cv=manager->getCurView();
    QWidget *nv=manager->getNextView();
    QGenieFrameMain *framemain=QGenieFrameMain::GetSingletonPtr();
    ASSERT(framemain);
    QSize sz=framemain->getStackSize();
    cv->resize(sz);
    nv->show();
    nv->resize(sz);

    QPixmap pixmap1(sz);
    cv->render(&pixmap1);

    mItemCurPage=mScene->addPixmap(pixmap1);

//    mItemCurPage->setPos(-mScene->width()/2,-mScene->height()/2 );

    QPixmap pixmap2(sz);
    nv->render(&pixmap2);
    mItemNextPage=mScene->addPixmap(pixmap2);
    mItemNextPage->setPos( mScene->width(),0 );
}

void QGeniePageMovie::setNext()
{
    initPixmap();
}

void QGeniePageMovie::showEvent ( QShowEvent * event )
{
    mTimeLine->setStartFrame(0);
    mTimeLine->setDirection(QTimeLine::Forward);
    mTimeLine->start();
    qDebug("showEvent");
}

void QGeniePageMovie::slot_FrameUpdate(int val)
{
  //  qDebug("slot_FrameUpdate %d",val);

	qreal siz=calSize(val);
  //  qreal pos=(FRAME_RANGE/2-val)*width()/FRAME_RANGE-width()*siz/2;
	qreal pos=calPos(val,siz);
    if(mItemCurPage && mItemNextPage)
    {
        mItemCurPage->setScale(siz);
        mItemNextPage->setScale(siz);

        QGenieFrameMain *framemain=QGenieFrameMain::GetSingletonPtr();
        ASSERT(framemain);
        QSize stack_sz=framemain->getStackSize();

        int posy=(stack_sz.height()*(1-siz))/2;
        //	qDebug("pos y is %d",posy);

        mItemCurPage->setPos(pos,posy );
        mItemNextPage->setPos(width()+pos,posy );
    }
}

void QGeniePageMovie::slot_Finished()
{
   // qDebug("slot_Finished %d");
    QGenieStackManager *manager=QGenieStackManager::GetSingletonPtr();
    ASSERT(manager);
    manager->realChangeStackViewIdx();

}

