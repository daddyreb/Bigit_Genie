#include "QGenieAboutThread.h"
#include "QGenie_qtNetTraffic.h"
#include "QGenieRefreshStatusThread.h"

#include "IInfoManager.h"
#include "info_defs.h"
#include "Genieplugin_Internet.h"

//extern  IInfoManager                                         *m_infomanager;

const int cThreshold=8;
const int cFrameTime=1000;

QGenieAboutThread::QGenieAboutThread(QObject* parent)
    :QThread(parent),mAbort(true)
    ,mLocked(false)
    ,mCurTime(0)
{
    iThree=0;
    mTime=new QTime();
    mTime->start();
    this->setPriority(QThread::TimeCriticalPriority);
}

QGenieAboutThread::~QGenieAboutThread()
{
    //    mMutex.lock();
    //    mAbort = false;
    //    mMutex.unlock();
    qDebug() << "about thread ended";
}

void QGenieAboutThread::stopThread()
{
    if(!mLocked)
    {
        mLock.lock();
        mLocked=true;
    }
}

void QGenieAboutThread::startThread()
{
    if(mLocked)
    {
        //resetBG();
        mLock.unlock();
        mLocked=false;
    }
}

void QGenieAboutThread::resetBG()
{

}

void QGenieAboutThread::run()
{


    while(mAbort)
    {
        mMutex.lock();
        bool abort=mAbort;
        mMutex.unlock();
        if(!abort)
        {
            break;
        }
       // mDrawMutex.lock();
        //drawWater((QRgb*)mImage1.bits(), (QRgb*)mImage2.bits());
        getTrafficData();
        //bDrawOrNot
        if (mw->bNull)
        {

        }
        else
        {
            if ( mw->bDrawOrNot)
            {
                emit sendTraffic();
            }
        }


      //  mDrawMutex.unlock();

        msleep(1000);
    }
    delete mTime;
}
void QGenieAboutThread::selfstop()
{
    mMutex.lock();
    mAbort = false;
    //this->quit();
    mMutex.unlock();
}

void QGenieAboutThread::getTrafficData()
{
    //    AllTraffic		= 388,//总的流量
    //  IncomingTraffic	= 264,//输入流量
    // OutGoingTraffic	= 506 //输出流量

    if(mw->bNetOk == false)
    {
        iThree = 0;
    }

    iThree++;
    if (iThree<3)
    {
        return;
    }

    /*

       //m_infomanager

        //g_infomanager
        1.IInfoManager
        添加函数         virtual const void *getSharedMemory(QString para)=0;  给internet插件用
        2. info_defs 添加数据类型以及宏定义
        typedef struct _TRAFFIC_ENTRY_
        {
            double value;
            bool connected;
        } TRAFFICENTRY;
        #define PLUGIN_INF_SM_TRAFFIC_STATS "trstates"
        #define PLUGIN_INF_SM_TRAFFIC_INCOMING "traffinco"
        #define PLUGIN_INF_SM_TRAFFIC_OUTGOING "traffoutgo"
     */
    if (GeniePlugin_Internet::m_infomanager!=0)
    {
        mw->TrafficStats=(const TRAFFICENTRY*)(GeniePlugin_Internet::m_infomanager->getSharedMemory(PLUGIN_INF_SM_TRAFFIC_STATS));
        mw->TrafficStatsIncoming=(const TRAFFICENTRY*)(GeniePlugin_Internet::m_infomanager->getSharedMemory(PLUGIN_INF_SM_TRAFFIC_INCOMING));
        mw->TrafficStatsOutgoing=(const TRAFFICENTRY*)(GeniePlugin_Internet::m_infomanager->getSharedMemory(PLUGIN_INF_SM_TRAFFIC_OUTGOING));
    }
    else
    {
        mw->bNull=true;
        return;
    }



    //TrafficStats = new TRAFFICENTRY[TrafficEntries+1];

    //int iCount=mw->m_cTrafficClass1[0].Interfaces.count();

    if (mw->TrafficStatsIncoming==0)
    {

        mw->bNull=true;
        return;
    }

    mw->bNull=false;
    double trafficIncomingSum= mw->TrafficStatsIncoming[620].value;
    /*
     for (int i=0;i<iCount;i++)
     {
         if (i<5)
         {
             mw->m_cTrafficClass1[i].SetTrafficType(QGenie_MFNetTraffic::IncomingTraffic);
             if (mw->infChoose[i]==1)
             {
                 if (mw->bNetOk)
                 {
                    double traffic1 = mw->m_cTrafficClass1[i].GetTraffic(i);
                    trafficIncomingSum=trafficIncomingSum+traffic1;
                 }
             }
         }
     }

     */
    char strIn[256];
    double delta1;
    // double divisor = 1.0;

    delta1=trafficIncomingSum/1024.0;
    if (mw->iunit==0)
    {
        delta1=delta1*8;
    }


    if (delta1<1000.0000)
    {

        if (mw->iShowUnit==1 || mw->iMode==0)
        {
            if (mw->iunit==0)
            {
                //delta1 = (double)(trafficIncomingSum * divisor) / 1024.0;
                sprintf(strIn,"%.1fMbps",delta1);
            }
            else
            {
                //delta1 = (double)(trafficIncomingSum * divisor) / 1024.0;
                sprintf(strIn,"%.1fkBps",delta1);
            }
        }
        else
        {
            //
            if (mw->iunit==0)
            {
                //delta1 = (double)(trafficIncomingSum * divisor) / 1024.0;
                sprintf(strIn,"%.1f",delta1);
            }
            else
            {
                //delta1 = (double)(trafficIncomingSum * divisor) / 1024.0;
                sprintf(strIn,"%.1f",delta1);
            }
        }
    }
    else
    {

        delta1=delta1/1000.0;
        if (mw->iShowUnit==1 || mw->iMode==0)
        {
            if (mw->iunit==0)
            {
                // delta1 = (double)(trafficIncomingSum * divisor) / 1024.0/1000.0;
                sprintf(strIn,"%.2fMbps",delta1);
            }
            else
            {
                // delta1 = (double)(trafficIncomingSum * divisor) / 1024.0/1000.0;
                sprintf(strIn,"%.2fMBps",delta1);
            }
        }
        else
        {
            if (mw->iunit==0)
            {
                //delta1 = (double)(trafficIncomingSum * divisor) / 1024.0/1000.0;
                sprintf(strIn,"%.2f",delta1);
            }
            else
            {
                // delta1 = (double)(trafficIncomingSum * divisor) / 1024.0/1000.0;
                sprintf(strIn,"%.2f",delta1);
            }
        }
    }

    //  QString strTxtInput;
    // QString strTxtOutput;

    mw->strTxtInput=strIn;
    // txtInput->setText(CurrentTraffic);


    double trafficOutgoingSum= mw->TrafficStatsOutgoing[620].value;
    /*
     for (int i=0;i<iCount;i++)
     {
         if (i<5)
         {
             mw->m_cTrafficClass2[i].SetTrafficType(QGenie_MFNetTraffic::OutGoingTraffic);
             if (mw->infChoose[i]==1)
             {

                 if (mw->bNetOk)
                 {
                    double traffic1 = mw->m_cTrafficClass2[i].GetTraffic(i);
                    trafficOutgoingSum=trafficOutgoingSum+traffic1;
                 }
             }
         }
     }
    */
    char strOut[256];

    double delta2;


    delta2=trafficOutgoingSum/1024.0;

    if ( mw->iunit==0)
    {
        delta2=delta2*8.0;
    }
    if (delta2<1000.0)
    {
        if ( mw->iShowUnit==1 ||  mw->iMode==0)
        {
            if ( mw->iunit==0)
            {
                //delta2 = (double)(trafficOutgoingSum * divisor) / 1024.0;
                sprintf(strOut,"%.1fkbps",delta2);
            }
            else
            {
                // delta2 = (double)(trafficOutgoingSum * divisor)/ 1024.0;
                sprintf(strOut,"%.1fkBps",delta2);
            }
        }
        else
        {
            if ( mw->iunit==0)
            {
                // delta2 = (double)(trafficOutgoingSum * divisor) / 1024.0;
                sprintf(strOut,"%.1f",delta2);
            }
            else
            {
                // delta2 = (double)(trafficOutgoingSum * divisor)/ 1024.0;
                sprintf(strOut,"%.1f",delta2);
            }
        }
    }
    else
    {
        delta2=delta2/1000.0;
        if ( mw->iShowUnit==1 ||  mw->iMode==0)
        {
            if ( mw->iunit==0)
            {
                //delta2 = (double)(trafficOutgoingSum * divisor) / 1024.0/1000.0;
                sprintf(strOut,"%.2fMbps",delta2);
            }
            else
            {
                // delta2 = (double)(trafficIncomingSum * divisor) / 1024.0/1000.0;
                sprintf(strOut,"%.2fMBps",delta2);
            }
        }
        else
        {
            if ( mw->iunit==0)
            {
                // delta2 = (double)(trafficOutgoingSum * divisor) / 1024.0/1000.0;
                sprintf(strOut,"%.2f",delta2);
            }
            else
            {
                // delta2 = (double)(trafficIncomingSum * divisor) / 1024.0/1000.0;
                sprintf(strOut,"%.2f",delta2);
            }
        }
    }

    //CurrentTraffic=strOut;
    //txtOutput->setText(CurrentTraffic);
    mw->strTxtOutput=strOut;

    //double trafficAlltrafficSum=mw->TrafficStats[2000].value;
    /*
     for (int i=0;i<iCount;i++)
     {
         if (i<5)
         {
             mw->m_cTrafficClass[i].SetTrafficType(QGenie_MFNetTraffic::AllTraffic);
             if (mw->infChoose[i]==1)
             {

                 if (mw->bNetOk)
                 {
                    double traffic1 = mw->m_cTrafficClass[i].GetTraffic(i);
                    trafficAlltrafficSum=trafficAlltrafficSum+traffic1;
                 }
             }
         }
     }


     for(DWORD x=0; x<mw->TrafficEntries; x++)
     {
             mw->TrafficStats[x].connected = mw->TrafficStats[x+1].connected;
             mw->TrafficStats[x].value	= mw->TrafficStats[x+1].value;


             mw->TrafficStatsIncoming[x].connected = mw->TrafficStatsIncoming[x+1].connected;
             mw->TrafficStatsIncoming[x].value	= mw->TrafficStatsIncoming[x+1].value;


             mw->TrafficStatsOutgoing[x].connected = mw->TrafficStatsOutgoing[x+1].connected;
             mw->TrafficStatsOutgoing[x].value	= mw->TrafficStatsOutgoing[x+1].value;

     }

     mw->TrafficStats[ mw->TrafficEntries].connected = TRUE;
     mw->TrafficStats[ mw->TrafficEntries].value = trafficAlltrafficSum;


     mw->TrafficStatsIncoming[ mw->TrafficEntries].connected = TRUE;
     mw->TrafficStatsIncoming[ mw->TrafficEntries].value = trafficIncomingSum;


     mw->TrafficStatsOutgoing[ mw->TrafficEntries].connected = TRUE;
     mw->TrafficStatsOutgoing[ mw->TrafficEntries].value = trafficOutgoingSum;

     */
    mw->MaxTrafficAmount = 0.0;
    for(DWORD x=mw->TrafficEntries-mw->iDrawCount; x<mw->TrafficEntries; x++)
    {
        /*
         if (mw->bDrawTotal)
         {
            if(mw->TrafficStats[x].value > mw->MaxTrafficAmount )
                    mw->MaxTrafficAmount = mw->TrafficStats[x].value;
         }
         */
        if (mw->bDrawInput)
        {
            if(mw->TrafficStatsIncoming[x].value > mw->MaxTrafficAmount)
                mw->MaxTrafficAmount = mw->TrafficStatsIncoming[x].value;
        }
        if (mw->bDrawOutput)
        {
            if(mw->TrafficStatsOutgoing[x].value > mw->MaxTrafficAmount)
                mw->MaxTrafficAmount = mw->TrafficStatsOutgoing[x].value;
        }
    }

    /*
        iAlert=50;
        bAlert=true;
    */

    if (mw->bAlert)
    {
        int iAlertCount=0;
        for(DWORD x=mw->TrafficEntries-10; x<mw->TrafficEntries; x++)
        {
            double bj=0;
            if (mw->iunit==0)
            {
                bj=mw->iAlert*1000*1024/8.0;
            }
            else
            {
                bj=mw->iAlert*1000*1024;
            }
            if (bj<mw->TrafficStats[x].value)
            {
                iAlertCount++;
            }
        }
        if (iAlertCount>6)
        {
            //emit ();
            emit sendTrafficAlert(true);
        }
        if (iAlertCount<3)
        {
            emit sendTrafficAlert(false);
        }
    }

  //  delete [] strOut;
   // delete [] strIn;
}
