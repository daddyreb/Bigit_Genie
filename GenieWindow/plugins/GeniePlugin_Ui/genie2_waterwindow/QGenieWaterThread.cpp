#include "QGenieWaterWindow.h"
#include "QGenieWaterThread.h"

//#define BG_IMAGE ":/bg_about.png"
#include "QGenieTopoNodePaintStrategy.h"
#include "genie2_interface.h"

const int cThreshold=8;
const int cFrameTime=60;

const int cSpanShift=0;
const int cPointsInspan=(1<<cSpanShift);

#define ARITHMETIC  2

QGenieWaterThread::QGenieWaterThread(const QString &bgimage, const QString &watermark, const int &topPadding)
//    :mBGImageName(bgimage)
//    ,mWatermark(watermark)
    :mTopPadding(topPadding)
//    ,mExitFlag(true)
//    ,mLocked(false)
    ,mWantUpdatePointNum(0)
//    ,mCurTime(0)
    ,mRandomCounter(0)
    ,mExitFlag(false)
{
    resetBG(bgimage,watermark);




    /* get libretroshare version */
/*    std::map<std::string, std::string>::iterator vit;
    std::map<std::string, std::string> versions;
    const RsConfig &conf = rsiface->getConfig();
    bool retv = rsDisc->getDiscVersions(versions);
    if (retv && versions.end() != (vit = versions.find(conf.ownId)))
    {
            QString version = QString::fromStdString("NETGEAR genie version : \n") + QString::fromStdString(vit->second);
    //  p.drawText(QRect(10, 20, width()-10, 60), version);
        p.drawText(QRect(10, 20, 10, 60), version);

    }
    p.end();
*/

    mDensity = 5;
    mPage = 0;

    mTime=new QTime();
    mTime->start();
    start();
}

QGenieWaterThread::~QGenieWaterThread()
{
//    mMutex.lock();
//    mAbort = false;
//    mMutex.unlock();
    mDrawMutex.lock();
    mExitFlag=true;
    mDrawMutex.unlock();
    delete (mTime);
    qDebug() << "about thread ended";
}

//void QGenieWaterThread::stopThread()
//{
//    if(!mLocked)
//    {
//        mLock.lock();
//        mLocked=true;
//    }
//}

//void QGenieWaterThread::startThread()
//{
//    if(mLocked)
//    {
//        resetBG();
//        mLock.unlock();
//        mLocked=false;
//    }
//}

void QGenieWaterThread::resetBG(const QString &bgimage,const QString &watermark)
{
    Q_ASSERT(QThread::currentThread()==qApp->thread());
    QImage image(bgimage);
    mImage1=mImage2=image;

    emit signal_ResizeWnd(image.size());
    int w = image.width();
    int h = image.height();
    mHeightField1.clear();
    mHeightField2.clear();
    mImageW=w;
    mImageH=h;
    mHeightW=(w>>cSpanShift)+1;
    mHeightH=(h>>cSpanShift)+1;

    mFullHeight.resize(mImageW*mImageH);
    mHeightField1.resize(mHeightW*mHeightH);
    mHeightField2.resize(mHeightW*mHeightH);
    mWantUpdatePoint.resize(mImageW*mImageH);

//    QGeniePluginProxyManager *proxymanager=QGeniePluginProxyManager::GetSingletonPtr();
//    Q_ASSERT(proxymanager);

//    QStringList templist=QString(VERSION_STRING).split(".");
//    Q_ASSERT(templist.size()>=4);
//    QString simpver=QString("%1.%2.%3.%4").arg(QString::number(templist.at(0).toInt()),
//                                               QString::number(templist.at(1).toInt()),
//                                               QString::number(templist.at(2).toInt()),
//                                               QString::number(templist.at(3).toInt())
//                                               );
//    QString ver=QString("%1 %2 %3").arg(GET_TEXT(L_MAIN_VERSION),simpver,VERSION_STR_SUFFIX);


//    int flags=Qt::TextWordWrap;//(qApp->isLeftToRight())?Qt::AlignLeft:Qt::AlignRight;
    const int cWidth=260;
    const int cBaseY=8;
    QPainter p(&mImage1);

    QFont font;
    font.setPointSize(13);
    p.setFont(font);
    p.setPen(QColor(140,140,140));
   // p.drawText(10,cBaseY+0,cWidth,20,flags,stemp);
    p.drawText(10,cBaseY,cWidth,90,Qt::TextWordWrap,watermark);

#if 1


    QString rmodel=GENIE2_HANDLER_CMD(HCMD_ASK_SUBSCRIBED_MESSAGE,SCCMD_ROUTER_MODEL).toString();
    if(rmodel.isEmpty())
    {
        rmodel=GENIE2_GET_SETTING(PLUGIN_ROUTER_MODEL).toString();
    }
    QGenieTopoNodePaintStrategy paintstrategy(AT_BIG3,NULL,rmodel);

    const QSize &siz=paintstrategy.getSize();
    int x=((mImage1.width()-siz.width())>>1);
    p.translate(x,150);
    paintstrategy.paint_B1_B3(&p,false);
#endif
    //    p.drawText(10,70,VERSTION_STR_DATE" "VERSTION_STR_TIME);
//    QFont font2("Arial",10);
//    p.setFont(font2);
//    p.drawText(10,cBaseY+40,cWidth,50,flags,GET_TEXT(L_MAIN_POWERED_BY));
    p.end();

    mRandomCounter=0;

}

void QGenieWaterThread::prepareDraw()
{
    //
    mWantUpdatePointNum=0;
    for(int i=0;i< (mHeightW-1);i++)
        for(int j=mTopPadding;j<(mHeightH-1);j++)
    {
        const int *ptr=&mHeightField1.front();
        const int x0= i<<cSpanShift;
        const int y0 = j<<cSpanShift;
        const int f1 = ptr[i+ j*mHeightW];
        const int f2 = ptr[i+1+j*mHeightW];
        const int f3 = ptr[i+(j+1)*mHeightW];
        const int f4 = ptr[i+1 +(j+1)*mHeightW];
        int *targetptr=&mFullHeight.front();

        const int right = (((i+1)<<cSpanShift) > (mImageW))?mImageW:((i+1)<<cSpanShift);
        const int botton = (((j+1)<< cSpanShift) > (mImageH))?mImageH:((j+1)<< cSpanShift);

        for(int x=x0;x<right;x++)
            for(int y=y0;y< botton;y++)
        {
            int height;
#if  ARITHMETIC ==1
            if((y-y0 )> (x-x0))
            {
                //button triangle
                height=f1+(f2-f1)*(x-x0)+(f3-f2)*(y-y0);
            }
            else
            {
                //top triangle
                height=f1+(f4-f1)*(y-y0)+(f3-f4)*(x-x0);
            }
#elif ARITHMETIC == 2
            //http://hi.chinaunix.net/?14015773/viewspace-43794
            const int cFloatShift =8;
            const int u         = ((x-x0)<<cFloatShift)>>cSpanShift;
            const int v         = ((y-y0)<<cFloatShift)>>cSpanShift;
            const int one_sub_u = ((((i+1)<<cSpanShift)-x)<<cFloatShift)>>cSpanShift;
            const int one_sub_v = ((((j+1)<<cSpanShift)-y)<<cFloatShift)>>cSpanShift;
            height = (one_sub_u*one_sub_v*f1
                     +u*one_sub_v*f2
                     +one_sub_u*v*f3
                     +u*v*f4)
                       >>(cFloatShift*2);

#else
            height=0;
#endif
            targetptr[x+y*mImageW]=height;

            if(x >0 && y >0)
            {
                const int lastH=targetptr[x-1+(y-1)*mImageW];
                const int dy=lastH-targetptr[x-1+y*mImageW];
                const int dx = lastH -targetptr[x+(y-1)*mImageW];
                if(dy <-cThreshold || dy >cThreshold ||dx <-cThreshold || dx >cThreshold)
                {
                    mWantUpdatePoint[mWantUpdatePointNum++]=(x-1)+(y-1)*mImageW;

                }
            }

        }

    }
}

void QGenieWaterThread::run()
{
    int test=0;

    while(1)
    {
        try
        {
//            mMutex.lock();
//            bool abort=mExitFlag;
//            mMutex.unlock();
            if(mExitFlag)
            {
                break;
            }
            if((test++)%20 ==0)
            {
                qDebug() << "thread is running!!" << test;
            }

            //block this thread when window not show
            mLock.lock();
            mLock.unlock();
            int curTime=0;
            if(mTime->elapsed() - curTime >cFrameTime )
            {
                curTime=mTime->elapsed();
                prepareDraw();
                mDrawMutex.lock();
                if(!mExitFlag)
                {
                    drawWater((QRgb*)mImage1.bits(), (QRgb*)mImage2.bits());
                }
                mDrawMutex.unlock();
            }


            updateWater(mPage, mDensity);
            mPage ^= 1;

            //   for(int i=0;i<4;i++)
            {
                const int cFramesAlwaysGenWave=4;
                if ((qrand()  % 128 == 0) ||(mRandomCounter++)<cFramesAlwaysGenWave) {
                    int r = 3 + qRound(qreal(qrand() * 4 / RAND_MAX));
                    int h = 300 + qrand() * 200 / RAND_MAX;
                    int x = 1 + r + qrand()%(mHeightW -2*r-1);
                    int y = 1 + r + qrand()%(mHeightH-2*r-1);
                    addWave(x, y, r, h);
                }
            }
            emit sendupdate();
            msleep(20);
        }catch(...){
            Q_ASSERT(0);
        }
    }
}
void QGenieWaterThread::selfstop()
{
//    mMutex.lock();
    mExitFlag = true;
//    mMutex.unlock();
}
void QGenieWaterThread::updateWater(int npage, int density) {
  //  int w = (mImage1.width())>>1;
  //  int h = (mImage1.height()) >>1;

#if 1
    int count = mHeightW + 1+(mTopPadding*mHeightW);


    // Set up the pointers
    int *newptr;
    int *oldptr;
    if(npage == 0) {
        newptr = &mHeightField1.front();
        oldptr = &mHeightField2.front();
    //    mWantUpdatePointNum=0;
    } else {
        newptr = &mHeightField2.front();
        oldptr = &mHeightField1.front();
    }

    for (int y = (mHeightH-1)*mHeightW; count < y; count += 2) {
        for (int x = count+mHeightW-2; count < x; count++) {
            // This does the eight-pixel method.  It looks much better.
            int newh  = ((oldptr[count + mHeightW]
            + oldptr[count - mHeightW]
            + oldptr[count + 1]
            + oldptr[count - 1]
            + oldptr[count - mHeightW - 1]
            + oldptr[count - mHeightW + 1]
            + oldptr[count + mHeightW - 1]
            + oldptr[count + mHeightW + 1]
            ) >> 2 ) - newptr[count];

            newptr[count] =  newh - (newh >> density);

        }
    }
#endif
}

void QGenieWaterThread::addWave(int x, int y, int radius, int height) {
    x=x>>cSpanShift;
    y=y>>cSpanShift;
    radius=radius>>cSpanShift;
    height=height>>cSpanShift;

    // Set up the pointers
    int *newptr;
    int *oldptr;
    if (mPage == 0) {
        newptr = &mHeightField1.front();
        oldptr = &mHeightField2.front();
    } else {
        newptr = &mHeightField2.front();
        oldptr = &mHeightField1.front();
    }

    int rquad = radius * radius;

    int left=-radius, right = radius;
    int top=-radius, bottom = radius;

    // Perform edge clipping...
    if (x - radius < 1+mTopPadding) left -= (x-radius-1);
    if (y - radius < 1+mTopPadding) top  -= (y-radius-1-mTopPadding);
    if (x + radius > mHeightW-1) right -= (x+radius-mHeightW+1);
    if (y + radius > mHeightH-1) bottom-= (y+radius-mHeightH+1);

    for(int cy = top; cy < bottom; cy++) {
        int cyq = cy*cy;
        for(int cx = left; cx < right; cx++) {
            if (cx*cx + cyq < rquad) {
                newptr[mHeightW*(cy+y) + (cx+x)] += height;
            }
        }
    }
}



void QGenieWaterThread::drawWater(QRgb* srcImage,QRgb* dstImage) {

//    int W=  mImage1.width();
//    int H = mImage1.height();
//    int w = W>>1;
//    int h = H>>1;
#if 1
    memcpy(dstImage,srcImage,mImageW*mImageH*sizeof(QRgb));


    int offset ; //offset in height cache
    int lBreak = mImageW * mImageH;

    int *ptr = &mFullHeight.front();

    for(int i=0;i<mWantUpdatePointNum;i++)
    {

        offset=mWantUpdatePoint[i];
        Q_ASSERT(offset >=0 && offset < lBreak);
        int dx = ptr[offset] - ptr[offset+1];
        int dy = ptr[offset] - ptr[offset+mImageW];

        int tempidx=offset+mImageW*(dy>>3) + (dx>>3);
        if(tempidx > mTopPadding*mHeightW && tempidx < lBreak)
        {
            QRgb c = srcImage[tempidx];
             c = shiftColor(c, dx);
      //  dstImage[tempoffset] = c;
             dstImage[offset]=c;
         }


    }
#endif
}
