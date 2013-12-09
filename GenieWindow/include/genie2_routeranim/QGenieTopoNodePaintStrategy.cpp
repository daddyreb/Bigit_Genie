#include "QGenieTopoNodePaintStrategy.h"
//#include "IInfoManager.h"
//#include "ILogManager.h"
//#include "cmd_defs.h"
//#include "info_defs.h"
#include "genie2_interface.h"
#include <QDebug>
#include <QUuid>
#include <assert.h>
#include <QImage>
#include <QFile>


const int MAX_LINE_PARA_COUNT=9;

#ifndef ASSERT
#define ASSERT assert
#endif



struct _t{
    char *routename;
    short linetype; //0 means vertical split
    short linecount;
    short lineparas[MAX_LINE_PARA_COUNT];
} ;


static const _t cRouteMovieParas_B1[]=
{
      {"xxxxx",0,2,{27,37}},
      {"xxxx",0,2,{50,80}}
};
static const int cRouteNum_B1=sizeof(cRouteMovieParas_B1)/sizeof(cRouteMovieParas_B1[0]);

static const _t cRouteMovieParas_B3[]=
{
#include "RouteAnimPara_B3.h"
};
static const int cRouteNum_B3=sizeof(cRouteMovieParas_B3)/sizeof(cRouteMovieParas_B3[0]);

static const _t cRouteMovieParas_Map[]=
{
#include "RouteAnimPara_Map.h"
};
static const int cRouteNum_Map=sizeof(cRouteMovieParas_Map)/sizeof(cRouteMovieParas_Map[0]);

QImageNameGengerator::QImageNameGengerator(const QString &s)
{
    const QString &dbg_model=QGenieIface2ShareObject::sSingleInstance->mDBGRouteModel;
    if(
            //            qApp->arguments().contains("-dbg_ui")&&
            !dbg_model.isEmpty()
            )
    {
        mRouterName=dbg_model;
    }
    else
    {
        mRouterName=s;
    }

   // qDebug()<<"QImageNameGengerator:dbg_model=="<<dbg_model;
  //  qDebug()<<"QImageNameGengerator:mRouterName=="<<mRouterName;
    int i1=mRouterName.indexOf(QRegExp("-|_"));
    if(i1 >0)
    {
        mRouterName=mRouterName.left(i1);
    }
    QRegExp regx("v(\\d+)",Qt::CaseInsensitive);
    mRouterName.replace(regx,"");
    mRouterName = mRouterName.toUpper(); //yankai added
//qDebug()<<"QImageNameGengerator:mRouterName.replace(regx,"")=="<<mRouterName;

    if(mRouterName=="" || mRouterName=="N/A")
    {
        mRouterName="DEFAULT_NONETGEAR";
    }
}

//QImageNameGengerator::QImageNameGengerator(const QString &routername)
//    :mRouterName(routername)
//{
//    if(mRouterName=="" || mRouterName=="N/A")
//    {
//        mRouterName="DEFAULT_NONETGEAR";
//    }
//}

QString QImageNameGengerator::getImageName(int type)
{
//    if(mInfoManager)
//    {
//        return mInfoManager->getInfo(type,idx);
//    }
    Q_ASSERT(!mRouterName.isEmpty());


    QString mode="";
    struct _{
        int name;
        const char *pic;
        //        int total;
    } cModeMap[]={
        {IE_B11,"images/route/%1big_1_1.png"}
        ,{IE_B12,"images/route/%1big_1_2.png"}
        ,{IE_B31,"images/route/%1big_3_1.png"}
        ,{IE_B32,"images/route/%1big_3_2.png"}
        ,{IE_NORMAL1,"images/route/%1Normal_1.png"}
        ,{IE_NORMAL2,"images/route/%1Normal_2.png"}
        ,{IE_SELECTED1,"images/route/%1Selected_1.png"}
        ,{IE_SELECTED2,"images/route/%1Selected_2.png"}
        ,{IE_OFFLINE,"images/route/%1OffLine_1.png"}
    };

    Q_ASSERT(type<IE_NUM);

    QString ret=GENIE2_RES( QString(cModeMap[type].pic).arg(mRouterName));
    if(!QFile::exists(ret))
    {
        ret=GENIE2_RES( QString(cModeMap[type].pic).arg("DEFAULT_NETGEAR"));
        Q_ASSERT(QFile::exists(ret));
    }
    return ret;

}

QString QImageNameGengerator::getRouterName()
{
    return mRouterName.isEmpty()?"DEFAULT_NONETGEAR":mRouterName;
}

//QGenieTopoNodePaintStrategy::QGenieTopoNodePaintStrategy(ANIM_TYPE type,IInfoManager *infomanager,QWidget *linkw)
//    :mMovieParaIdx(-1)
//    ,mLinkW(linkw)
//    ,mType(type)
//{
//    QImageNameGengerator imagename_gen(infomanager);
//    init(&imagename_gen);
//   // mImg.load(routeimg);
//}

QGenieTopoNodePaintStrategy::QGenieTopoNodePaintStrategy(ANIM_TYPE type,  QWidget *linkw,const QString &model)
    :mMovieParaIdx(-1)
    ,mLinkW(linkw)
    ,mType(type)
{
    QImageNameGengerator imagename_gen(model);
    init(&imagename_gen);

}

QGenieTopoNodePaintStrategy::~QGenieTopoNodePaintStrategy()
{
//     while (!mImgs.isEmpty())
//    {
//        delete mImgs.takeFirst();
//    }
}

void QGenieTopoNodePaintStrategy::paint(QPainter *painter,bool mouseover,bool online)
{
    if(mType==AT_MAP)
    {
        paint_Map(painter,mouseover,online);
    }
    else if(mType == AT_BIG1)
    {
        paint_B1_B3(painter,true);
    }
    else if(mType == AT_BIG3)
    {
        paint_B1_B3(painter,false);
    }
}

void QGenieTopoNodePaintStrategy::paint_B1_B3(QPainter *painter,bool b1)
{
    static int si=0;
    si=(si+1)%2;
    int dx=0;
    int dy=0;
    if(mLinkW)
    {
        dx=(mLinkW->width()-QPixmap(mImgs[0]).width())>>1;
        dy=(mLinkW->height()-QPixmap(mImgs[0]).height())>>1;
    }
    if(mMovieParaIdx <0 )
    {
        if(si<mImgs.count())
        {
//            qDebug()<<"QGenieTopoNodePaintStrategy::paint_B1";
//            painter->drawImage(dx,dy,*(mImgs[si]));
            painter->drawPixmap(dx,dy,QPixmap(mImgs[si]));
        }
    }
    else
    {
        int lines=qrand();
        const _t &moviepara=b1?cRouteMovieParas_B1[mMovieParaIdx]:cRouteMovieParas_B3[mMovieParaIdx];
        ASSERT(moviepara.linecount < MAX_LINE_PARA_COUNT);

        if(moviepara.linetype==0)
        {
            int lastpos=0;
            int H=QPixmap(mImgs[0]).height();
            for(int i=0;i<moviepara.linecount+1;i++)
            {
                if(lastpos >= H)
                {
                    break;
                }
                int y2=(i==moviepara.linecount)?H:moviepara.lineparas[i];
                int idx=(lines & (1<<i))?1:0;
                int y=lastpos;
                int x=0;
                int h=y2-y;
                int w=QPixmap(mImgs[idx]).width();

//                painter->drawImage(dx,dy+y,*(mImgs[idx]),x,y,w,h);
                painter->drawPixmap(dx,dy+y,QPixmap(mImgs[idx]),x,y,w,h);

                lastpos=y2;
            }
        }
        else
        {
            //vertical split
            int lastpos=0;
            int W=QPixmap(mImgs[0]).width();
            for(int i=0;i<moviepara.linecount+1;i++)
            {
                if(lastpos>=W)
                {
                    break;
                }
                int idx=(lines & (1<<i))?1:0;
                int x2=(i==moviepara.linecount)?W:moviepara.lineparas[i];
                int y=0;
                int x=lastpos;
                int h=QPixmap(mImgs[idx]).height();
                int w=x2-x;
//                painter->drawImage(dx+x,dy,*(mImgs[idx]),x,y,w,h);
                painter->drawPixmap(dx+x,dy,QPixmap(mImgs[idx]),x,y,w,h);
                lastpos=x2;
            }
        }
    }

}

const QSize &QGenieTopoNodePaintStrategy::getSize()
{
    return mSize;
}

const QString &QGenieTopoNodePaintStrategy::getImageName()
{
    return mImageName;
}

void QGenieTopoNodePaintStrategy::init(QImageNameGengerator *image_gener)
{

    const int type=mType;
    const int cTemparraysize=6;
    int tTemp[AT_NUM][cTemparraysize]=
    {
        {IE_B11,IE_B12,-1}
        ,{IE_B31,IE_B32,-1}
        ,{IE_NORMAL1,IE_NORMAL2,IE_SELECTED1,IE_SELECTED2,IE_OFFLINE,-1}
    };


//        int total=sizeof(tTemp)/sizeof(tTemp[0]);
//    while (!mImgs.isEmpty())
//    {
//        delete mImgs.takeFirst();
//    }
    mImgs.clear();
    for(int i=0;i<cTemparraysize;i++)
    {
        if(tTemp[type][i]==-1)
        {
            break;
        }
        QString imgstr=image_gener->getImageName(tTemp[type][i]);

//        QImage *img=new QImage(imgstr);
//        ASSERT(img);

        mImgs.append(imgstr);
        if(mImageName.isEmpty())
        {
            mImageName=imgstr;
        }

//        if(0 == i)
//        {
//            mImageName = imgstr;
//        }
    }

    const _t *cTypePara[]={cRouteMovieParas_B1,cRouteMovieParas_B3,cRouteMovieParas_Map};
    const int cTypeNum[]={cRouteNum_B1,cRouteNum_B3,cRouteNum_Map};

    QString stemp=image_gener->getRouterName();

    static int default_netgear_idx=-1;
    const int cTemp_type=1;
    if(default_netgear_idx==-1)
    {
        for(int i=0;i<cTypeNum[cTemp_type];i++)
        {
            if(QString("DEFAULT_NETGEAR")==cTypePara[cTemp_type][i].routename)
            {
                default_netgear_idx=i;
                break;

            }
        }
    }
    for(int i=0;i<cTypeNum[type];i++)
    {
       // if(stemp.contains(cTypePara[type][i].routename))
        if(stemp==(cTypePara[type][i].routename))
        {
            mMovieParaIdx=i;
            break;
        }

    }
    if(mMovieParaIdx <0 && default_netgear_idx >=0)
    {
        mMovieParaIdx=default_netgear_idx;
    }
    if(!mImgs.isEmpty())
    {
        mSize=QPixmap(mImgs[0]).size();
    }



    //qDebug()<<"map img idx is "<<mMovieParaIdx;
}

//void QGenieTopoNodePaintStrategy::paint_B3(QPainter *painter,bool mouseover,bool online)
//{
//    paint_B1_B3(painter,mouseover,online);
//}

void QGenieTopoNodePaintStrategy::paint_Map(QPainter *painter,bool mouseover,bool online)
{
    int idx=0;
    QPixmap temp_pix(mImgs[idx]);
    const int dx=-temp_pix.width()/2;
    const int dy=-temp_pix.height()/2;
    if(mMovieParaIdx <0 || !online)
      {
        static int si=0;
        si=(si+1)%2;

        idx=(!online)?4:((mouseover?2:0)+si);
        if(idx<mImgs.count())
        {
//            painter->drawImage(dx,dy,*(mImgs[idx]));
            painter->drawPixmap(dx,dy,QPixmap(mImgs[idx]));
        }

    }
    else
    {
        int lines=qrand();
        const _t &moviepara=cRouteMovieParas_Map[mMovieParaIdx];
        ASSERT(moviepara.linecount < MAX_LINE_PARA_COUNT);

        if(moviepara.linetype==0)
        {
            int lastpos=0;
            int H=QPixmap(mImgs[idx]).height();
            for(int i=0;i<moviepara.linecount+1;i++)
            {
                if(lastpos >= H)
                {
                    break;
                }
                int y2=(i==moviepara.linecount)?H:moviepara.lineparas[i];
                int tempidx=(mouseover?2:0)+
                ((lines & (1<<i))?1:0);
                ASSERT(0<=tempidx && tempidx <mImgs.count());
                int y=lastpos;
                int x=0;
                int h=y2-y;
                int w=QPixmap(mImgs[tempidx]).width();

//                ASSERT(mImgs[tempidx]);
//                painter->drawImage(dx,dy+y,*(mImgs[tempidx]),x,y,w,h);
                painter->drawPixmap(dx,dy+y,QPixmap(mImgs[tempidx]),x,y,w,h);
                lastpos=y2;
            }
        }
        else
        {
            //vertical split
            int lastpos=0;
            int W=QPixmap(mImgs[idx]).width();
            for(int i=0;i<moviepara.linecount+1;i++)
            {
                if(lastpos>=W)
                {
                    break;
                }
                int tempidx2=(mouseover?2:0)+
                ((lines & (1<<i))?1:0);
                ASSERT(0<=tempidx2 && tempidx2 <mImgs.count());
                int x2=(i==moviepara.linecount)?W:moviepara.lineparas[i];
                int y=0;
                int x=lastpos;
                int h=QPixmap(mImgs[tempidx2]).height();
                int w=x2-x;
//                ASSERT(mImgs[tempidx2]);
//                painter->drawImage(dx+x,dy,*(mImgs[tempidx2]),x,y,w,h);
                painter->drawPixmap(dx+x,dy,QPixmap(mImgs[tempidx2]),x,y,w,h);
                lastpos=x2;
            }
        }
    }
}

QGenieRouteWidget::QGenieRouteWidget(QWidget *parent)
    :QWidget(parent)
    ,mPaintStrategy(NULL)
    ,mType(AT_INVALID)
//    ,mInfoManager(NULL)
//    ,mLogManager(NULL)

{
//    mPaintStrategy=new QGenieTopoNodePaintStrategy(AT_BIG3);
    startTimer(ANIMATION_INTERVAL);
    QGenieIface2ShareObject::sSingleInstance->mRouterWidgets.append(this);
}

void QGenieRouteWidget::reloadImage()
{
//    if(mType != AT_INVALID && mInfoManager)
//    {
    GENIE2_SAFEDELETE(mPaintStrategy);

    mPaintStrategy=new QGenieTopoNodePaintStrategy(mType,this
                ,GENIE2_HANDLER_CMD(HCMD_ASK_SUBSCRIBED_MESSAGE,SCCMD_ROUTER_MODEL).toString());

//    }
//    if(mLogManager)
//    {
//        mLogManager->writeLog("QGenieRouteWidget::reloadImage()",mUuidForLog);
//    }
}

//void QGenieRouteWidget::setInfoManager(IInfoManager *infomanager)
//{
//    ASSERT(infomanager);
//    mInfoManager=infomanager;
//    mMutex.lock();
//    if(mPaintStrategy)
//    {
//        delete mPaintStrategy;
//        mPaintStrategy=NULL;
//    }
//    if(mType!= AT_INVALID)
//    {

//        mPaintStrategy=new QGenieTopoNodePaintStrategy(mType,infomanager,this);

//    }
//    mMutex.unlock();
//}

//void QGenieRouteWidget::setLogManager(ILogManager *logmanager,QUuid uuid)
//{
//    mLogManager=logmanager;
//    mUuidForLog=uuid;
//}

QGenieRouteWidget::~QGenieRouteWidget()
{
//    mMutex.lock();
    QGenieIface2ShareObject::sSingleInstance->mRouterWidgets.removeAll(this);
    if(mPaintStrategy)
    {
        delete mPaintStrategy;
    }
//    mMutex.unlock();
}

void QGenieRouteWidget::timerEvent(QTimerEvent *)
{
    update();
}

void QGenieRouteWidget::paintEvent(QPaintEvent * event)
{
	Q_UNUSED(event)
//    mMutex.lock();
    if(mPaintStrategy)
    {
        QPainter painter(this);
        mPaintStrategy->paint(&painter,false,true);
    }
//    mMutex.unlock();
}

QGenieRouteWidget_Map::QGenieRouteWidget_Map(QWidget *parent)
    :QGenieRouteWidget(parent)
{
    mType=AT_MAP;
    reloadImage();
//    mPaintStrategy=new QGenieTopoNodePaintStrategy(mType);
}

QGenieRouteWidget_B3::QGenieRouteWidget_B3(QWidget *parent)
    :QGenieRouteWidget(parent)
{
    mType=AT_BIG3;
    reloadImage();
//    mPaintStrategy=new QGenieTopoNodePaintStrategy(mType);
}




