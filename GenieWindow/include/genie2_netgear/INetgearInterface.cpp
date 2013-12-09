#include "INetgearInterface.h"
#ifdef GENIE2_MAINFRAME
#include "genie2_interface.h"
#else
#define GENIE2_LOG(x)
#endif
#include "QGenieNetgearInterface.h"

IThreadInterface::IThreadInterface(QGenieNetgearInterface *inf)
    :mInterface(inf)
    ,mExitFlag(inf->mExitFlag)
//    ,mNode(NULL)
{

}

IThreadInterface::~IThreadInterface()
{
    wait();
}

//void IThreadInterface::startGetInfo(int cmd,const QStringList &paras,int session)
//{
//    GENIE2_LOG(QString("QGenieNetgearInfoThread::startGetInfo %1 ").arg(cmd));
//    mExit=false;
//    mCmd=cmd;
//    mReqParas=paras;
//    mSession=session;
//    QThread::start();
//}

void IThreadInterface::run()
{
    while(!mExitFlag)
    {
        if(mSemaphore.tryAcquire(1,2000))
        {
            if(mExitFlag)
            {
                break;
            }
            //step1 get node
            _SoapNode *node=NULL;
            int cmd;
            QStringList reqlist;
            mInterface->mMutex.lock();
            QList<_SoapNode *> &nodelist=mInterface->mNodeList;
            int c=nodelist.count();
            for(int i=0;i<c;i++)
            {
                _SoapNode *temp_node=nodelist[i];
                if(  !(temp_node->flag&_SoapNode::F_INPROCESS)
                    && canProcess(temp_node->cmd)
                    )
                {
                    temp_node->flag |=_SoapNode::F_INPROCESS;
                    node=temp_node;
                    cmd=node->cmd;
                    reqlist=node->paralist;
                    break;
                }
            }
/*			if(!node)
			{
				for(int i=0;i<c;i++)
				{
					_SoapNode *temp_node=nodelist[i];
					qDebug()<<temp_node->cmd;
				}
			}
*/
			//Q_ASSERT(node);
			
            mInterface->mMutex.unlock();

            //step2 get data

            if(node)
            {
                qDebug()<<"netgearthread"<<QThread::currentThreadId()
                       <<"processed cmd"<<node->cmd<<node->slot;
                QStringList retlist;
                getNetgearData(cmd,reqlist,retlist);
                emit signal_DataReturn(node,retlist);
            }



        }//tryAcquire
    }
}





