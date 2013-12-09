#include "SvtTracer.h"
#include "defines.h"

extern quint32 sMapFlags;
#define TRACER_INTERVAL 1000
#define DATA_LENGTH 20
SVT_Tracer::SVT_Tracer(QObject *parent)
    :QObject(parent)
    ,mDbgW(NULL)
    ,mArpCounter(0)
    ,mDataIdx(0)
{
    Q_ASSERT(sMapFlags&MF_IN_DBG_MOD);
    connect(&mTimer,SIGNAL(timeout()),this,SLOT(slot_Timeout()));
    mData.resize(DATA_LENGTH);
}

SVT_Tracer::~SVT_Tracer()
{
    disconnect(&mTimer,SIGNAL(timeout()),NULL,NULL);
}

void SVT_Tracer::whenArp()
{
    mMutex.lock();
    mArpCounter++;
    mMutex.unlock();
}

void SVT_Tracer::slot_Timeout()
{
    mDataIdx=(mDataIdx+1)%DATA_LENGTH;
    mData[mDataIdx]=mArpCounter;

    mMutex.lock();
    mArpCounter=0;
    mMutex.unlock();
    mDbgW->update();
}

QWidget *SVT_Tracer::getUi()
{
    if(!mDbgW)
    {
        mDbgW=new QLabel(NULL);
        mDbgW->setWordWrap(true);
        mDbgW->installEventFilter(this);
        mTimer.start(TRACER_INTERVAL);
    }
    return mDbgW;
}

bool SVT_Tracer::eventFilter(QObject *o, QEvent *e)
{
    QEvent::Type type=e->type();
    QWidget *w=qobject_cast<QWidget *>(o);
    Q_ASSERT(w);
    if(QEvent::Paint==type)
    {
        QPainter p(w);
        p.setPen(qRgb(158,153,253));
        QPainterPath path;
        int max=0;
        for(int i=0;i<DATA_LENGTH;i++)
        {
            max=qMax(mData[i],max);
        }
        const int height=w->height();
        const int space=10;
        qreal k=qreal(4*height)/5/max;
        path.moveTo(0,height-k*mData[mDataIdx]);
        for(int i=1;i<DATA_LENGTH;i++)
        {
            path.lineTo(space*i,height-k*mData[(mDataIdx+DATA_LENGTH-i)%DATA_LENGTH]);
        }
        p.drawPath(path);
        p.setPen(Qt::red);
        p.drawText(20,20,QString::number(mData[mDataIdx]));
        p.drawText(40,20,QString::number(max));
    }
    return false;
}
