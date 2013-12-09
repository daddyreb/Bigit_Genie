#include "QGenieAboutThread.h"
#include "QGenie_qtNetTraffic.h"
#include "IInfoManager.h"
#include "info_defs.h"
#include <QObject>
#include <QtGlobal>

const int cThreshold=8;
const int cFrameTime=1000;

QGenieAboutThread::QGenieAboutThread()
    :mAbort(true)
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

        mDrawMutex.lock();
        //drawWater((QRgb*)mImage1.bits(), (QRgb*)mImage2.bits());
        getTrafficData();
        emit sendTraffic();

        mDrawMutex.unlock();

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

  //  mMutex.lock();

    try
    {


         //Q_ASSERT_X(false, "divide", "division by zero");



         double trafficIncomingSum=0.0;
         double delta1=0;
         double trafficOutgoingSum=0.0;
         double delta2=0;
         double trafficAlltrafficSum=0.0;
         QString strintemp;
         QString strouttemp;

         if (mw->bNetOk==false)
         {
            iThree=0;
         }
         iThree++;
         if (iThree<3)
         {
             return;
         }
         IInfoManager* iinfo;
         //mw->TrafficStats=(const TRAFFICENTRY*)(GeniePlugin_Internet::m_infomanager->getSharedMemory(PLUGIN_INF_SM_TRAFFIC_STATS));
         if (mw->sHandler)
         {
            iinfo=mw->sHandler->getInfoManager();
            if (iinfo)
            {
                mw->lbl_internet_input=(QLabel*)(iinfo->getSharedMemory(PLUGIN_INF_TEXT_DOWNLOAD));
                qDebug()<<"getSharedMemory(PLUGIN_INF_TEXT_DOWNLOAD)";
                qDebug()<<"mw->lbl_internet_input=="<<mw->lbl_internet_input;
                mw->lbl_internet_output=(QLabel*)(iinfo->getSharedMemory(PLUGIN_INF_TEXT_UPLOAD));
            }
         }

        //    QLabel* lbl_internet_input;
        //QLabel* lbl_internet_output;
         int iCount=mw->m_cTrafficClass1[0].Interfaces.count();


         for (int i=0;i<iCount;i++)
         {
             if (i<3)
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

        // double dblincoming=averageincoming(2,trafficIncomingSum);
        // trafficIncomingSum=dblincoming;
         char strIn[256];

         //double divisor = 1.0;
         if (trafficIncomingSum>80*1000*1024/8)
             trafficIncomingSum=80*1000*1024/8;
         if (trafficIncomingSum<0.01*1000*1024/8)
             trafficIncomingSum=0;

         delta1=trafficIncomingSum/1024.0;
         if (delta1<0.01)
         {
             delta1=0;
             trafficIncomingSum=0;
         }
         if (mw->iunit==0)
         {
             delta1=delta1*8.0;
         }
    /*
         if (delta1<1000.0000)
         {

             if (mw->iShowUnit==1 || mw->iMode==0)
             {
                 if (mw->iunit==0)
                 {
                     //delta1 = (double)(trafficIncomingSum * divisor)*8.0 / 1024.0;
                     sprintf(strIn,"%.1fkbps",delta1);
                 }
                 else
                 {
                     //delta1 = (double)(trafficIncomingSum * divisor) / 1024.0;
                     sprintf(strIn,"%.1fkBps",delta1);
                 }
             }
             else
             {
                 if (mw->iunit==0)
                 {
                     //delta1 = (double)(trafficIncomingSum * divisor)*8.0 / 1024.0;
                     sprintf(strIn,"%.1f",delta1);
                 }
                 else
                 {
                    // delta1 = (double)(trafficIncomingSum * divisor) / 1024.0;
                     sprintf(strIn,"%.1f",delta1);
                 }
             }
         }
         else
         {
             */
             delta1=delta1/1000.0;
             if (mw->iShowUnit==1 || mw->iMode==0)
             {
                 if (mw->iunit==0)
                 {
                     //delta1 = (double)(trafficIncomingSum * divisor)*8.0 / 1024.0/1000.0;
                     //QString
                     //char* strIntemp=new char[15];
                     sprintf(strIn,"%.2f",delta1);
                     strintemp=strIn;
                     if (strintemp.right(1)=="0")
                         strintemp=strintemp.left(strintemp.length()-1);

                     if (strintemp.right(1)=="0")
                         strintemp=strintemp.left(strintemp.length()-1);

                     if (strintemp.right(1)==".")
                         strintemp=strintemp.left(strintemp.length()-1);

                     strintemp=strintemp+"Mbps";
                     //sprintf(strIn,"%.2fMbps",delta1);

                 }
                 else
                 {
                     //delta1 = (double)(trafficIncomingSum * divisor) / 1024.0/1000.0;
                     sprintf(strIn,"%.2fMBps",delta1);
                 }
             }
             else
             {
                 if (mw->iunit==0)
                 {
                    // delta1 = (double)(trafficIncomingSum * divisor)*8.0 / 1024.0/1000.0;
                     sprintf(strIn,"%.2f",delta1);
                 }
                 else
                 {
                    // delta1 = (double)(trafficIncomingSum * divisor) / 1024.0/1000.0;
                     sprintf(strIn,"%.2f",delta1);
                 }
             }
         //}

        // QString strTxtInput;
        // QString strTxtOutput;

         //txtInput->setText(strTxtInput);
         //txtOutput->setText(strTxtOutput);

         mw->strTxtInput=strintemp;
        // txtInput->setText(CurrentTraffic);
         if (mw->lbl_internet_input)
            mw->lbl_internet_input->setText(strintemp);
         mw->txtInput->setText(strintemp);



         for (int i=0;i<iCount;i++)
         {
             if (i<3)
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

         char strOut[256];

        //     double dbloutgoing=averageoutgoing(2,trafficOutgoingSum);
        //   trafficOutgoingSum=dbloutgoing;

         if (trafficOutgoingSum>80*1000*1024/8)
             trafficOutgoingSum=80*1000*1024/8;

         if (trafficOutgoingSum<0.01*1000*1024/8)
             trafficOutgoingSum=0;

         delta2=trafficOutgoingSum/1024.0;
         if (delta2<0.01)
         {
             delta2=0;
             trafficOutgoingSum=0;
         }

         if (mw->iunit==0)
         {
             delta2=delta2*8.0;
         }
         /*
         if ((delta2)<1000.0)
         {
             if ( mw->iShowUnit==1 ||  mw->iMode==0)
             {
                 if ( mw->iunit==0)
                 {
                     //delta2 = (double)(trafficOutgoingSum * divisor)*8.0 / 1024.0;
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
                    // delta2 = (double)(trafficOutgoingSum * divisor)*8.0 / 1024.0;
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
         */
             delta2=delta2/1000.0;
             if ( mw->iShowUnit==1 ||  mw->iMode==0)
             {
                 if ( mw->iunit==0)
                 {
                    // delta2 = (double)(trafficOutgoingSum * divisor)*8.0 / 1024.0/1000.0;
                     //sprintf(strOut,"%.2fMbps",delta2);

                     sprintf(strOut,"%.2f",delta2);
                     strouttemp=strOut;
                     if (strouttemp.right(1)=="0")
                         strouttemp=strouttemp.left(strouttemp.length()-1);

                     if (strouttemp.right(1)=="0")
                         strouttemp=strouttemp.left(strouttemp.length()-1);

                     if (strouttemp.right(1)==".")
                         strouttemp=strouttemp.left(strouttemp.length()-1);

                     strouttemp=strouttemp+"Mbps";

                 }
                 else
                 {
                   //  delta2 = (double)(trafficIncomingSum * divisor) / 1024.0/1000.0;
                     sprintf(strOut,"%.2fMbps",delta2);
                 }
             }
             else
             {
                 if ( mw->iunit==0)
                 {
                    // delta2 = (double)(trafficOutgoingSum * divisor)*8.0 / 1024.0/1000.0;
                     sprintf(strOut,"%.2f",delta2);
                 }
                 else
                 {
                   //  delta2 = (double)(trafficIncomingSum * divisor) / 1024.0/1000.0;
                     sprintf(strOut,"%.2f",delta2);
                 }
             }
        // }

         //CurrentTraffic=strOut;
         //txtOutput->setText(CurrentTraffic);
         mw->strTxtOutput=strouttemp;

         //txtInput->setText(strTxtInput);
         //txtOutput->setText(strTxtOutput);

         if (mw->lbl_internet_output)
            mw->lbl_internet_output->setText(strouttemp);
         mw->txtOutput->setText(strouttemp);


         for (int i=0;i<iCount;i++)
         {
             if (i<3)
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

         //    double dbltotal=averagetotal(2,trafficAlltrafficSum);
         //  trafficAlltrafficSum=dbltotal;
         if (trafficAlltrafficSum<1)
             trafficAlltrafficSum=0;

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
         if (trafficAlltrafficSum>0)
         {
            mw->TrafficStats[ mw->TrafficEntries].value = trafficAlltrafficSum;
         }
         else
         {
             mw->TrafficStats[ mw->TrafficEntries].value = 0;
         }

         mw->TrafficStatsIncoming[ mw->TrafficEntries].connected = TRUE;
         if (trafficIncomingSum>0)
         {
            mw->TrafficStatsIncoming[ mw->TrafficEntries].value = trafficIncomingSum;
         }
         else
         {
             mw->TrafficStatsIncoming[ mw->TrafficEntries].value = 0;
         }



         mw->TrafficStatsOutgoing[ mw->TrafficEntries].connected = TRUE;
         if (trafficOutgoingSum>0)
         {
            mw->TrafficStatsOutgoing[ mw->TrafficEntries].value = trafficOutgoingSum;
         }
         else
         {
              mw->TrafficStatsOutgoing[ mw->TrafficEntries].value = 0;
         }

         mw->MaxTrafficAmount = 0.0;
         for(DWORD x=mw->TrafficEntries-mw->iDrawCount; x<mw->TrafficEntries; x++)
         {
             //if(mw->TrafficStats[x].value > mw->MaxTrafficAmount)
                     //mw->MaxTrafficAmount = mw->TrafficStats[x].value;

             if(mw->TrafficStatsIncoming[x].value > mw->MaxTrafficAmount)
                     mw->MaxTrafficAmount = mw->TrafficStatsIncoming[x].value;

             if(mw->TrafficStatsOutgoing[x].value > mw->MaxTrafficAmount)
                     mw->MaxTrafficAmount = mw->TrafficStatsOutgoing[x].value;
         }

    /*
         double delta4;
         divisor = 1.0;

         if (mw->iunit==0)
         {
             delta4 = (double)(mw->MaxTrafficAmount * divisor)*8.0/ 1024.0;
         }
         else
         {
             delta4 = (double)(mw->MaxTrafficAmount * divisor) / 1024.0 ;
         }
         //this->repaint();
    */
           // delete strIn;
           // delete strOut;

            //this->quit();
        //   mMutex.unlock();

     }
     catch (...)
     {
         mw->Genie_Log("QGenieAboutThread GetTrafficData() error!");
         qFatal("QGenieAboutThread GetTrafficData() error!" );

     }
}

double QGenieAboutThread::averageincoming(int icount,double dbltraffic)
{
    if (dbltraffic<90)
        return 0;
    double sum=0;
    double iSumCount=0;
    for (int i=0;i<icount;i++)
    {
        int iPos=mw->TrafficEntries-i;
        if (mw->TrafficStatsIncoming[ iPos].value>1)
        {
            sum=sum+mw->TrafficStatsIncoming[ iPos].value;
            iSumCount++;
        }
    }
    sum=sum+dbltraffic;
    iSumCount++;
    double dblreturn=sum/(icount+1);
    return dblreturn;
}

double QGenieAboutThread::averageoutgoing(int icount,double dbltraffic)
{

    if (dbltraffic<90)
        return 0;
    double sum=0;
    double iSumCount=0;
    for (int i=0;i<icount;i++)
    {
        int iPos=mw->TrafficEntries-i;
        if (mw->TrafficStatsOutgoing[ iPos].value>1)
        {
            sum=sum+mw->TrafficStatsOutgoing[ iPos].value;
            iSumCount++;
        }
    }
    sum=sum+dbltraffic;
    iSumCount++;
    double dblreturn=sum/(icount+1);
    return dblreturn;
}

double QGenieAboutThread::averagetotal(int icount,double dbltraffic)
{

    if (dbltraffic<90)
        return 0;
    double sum=0;
    double iSumCount=0;
    for (int i=0;i<icount;i++)
    {
        int iPos=mw->TrafficEntries-i;
        if (mw->TrafficStats[ iPos].value>1)
        {
            sum=sum+mw->TrafficStats[ iPos].value;
            iSumCount++;
        }
    }
    sum=sum+dbltraffic;
    iSumCount++;
    double dblreturn=sum/(icount+1);
    return dblreturn;
}
