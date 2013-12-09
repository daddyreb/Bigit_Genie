#include <QtGui>

#include "netscanthread.h"

NetScanThread::NetScanThread(QObject *parent)
        :QThread(parent)
{
    //adapterinfo = new  struct AdapterInfo;
    //memset(adapterinfo, 0, sizeof(struct AdapterInfo));
    m_filename = DLL_NetSCAN_FILE_NAME;
    m_getadapterinfo = NULL;
    if(!m_filename.isEmpty()){
        m_myLib = new QLibrary;
        if(m_myLib){
            m_myLib->setFileName(m_filename);
            m_getadapterinfo = (GetAdapterInfo) m_myLib->resolve("get_adapter_info");
        }
    }

    //connect(&getDetails, SIGNAL(GetDetailsFinish()), this, SIGNAL(ThreadFinish()));
    //connect(&getDetails, SIGNAL(GetDetailsFinish()), this, SLOT(ThreadOver()));
}

NetScanThread::~NetScanThread()
{
    if (m_myLib->isLoaded())
        m_myLib->unload();
    delete m_myLib;

    this->wait();
}

void NetScanThread::StartGetGatewayInfo(QMap<QString, QString>**OutData)
{
    if (*OutData==NULL){
        return;
    }
    mInfo = *OutData;
    mInfo->clear();

    this->start();
}

void NetScanThread::run()
{
    qDebug() << "Start Get Gateway's Details!";

    struct AdapterInfo *adapterinfo = new  struct AdapterInfo;
    memset(adapterinfo, 0, sizeof(struct AdapterInfo));

    if (adapterinfo != NULL && m_getadapterinfo != NULL){
        try {
            m_getadapterinfo(adapterinfo);
        }catch(std::exception &e){
            ;
        }

        QString mMac;
        QString mIp;
        QString localIp;
        //index = adapterinfo->m_index;
        mMac = QString("%1").arg(adapterinfo->m_local_mac);
        //adaptername = QString("%1").arg(adapterinfo->m_adapter_name);
        mIp = QString("%1").arg(adapterinfo->m_gateway);
        localIp = QString("%1").arg(adapterinfo->m_local_ip);
        //mask = QString("%1").arg(adapterinfo->m_mask);

        if(localIp.isEmpty()){
            localIp = "0.0.0.0";
            mIp = "0.0.0.0";
            //mask = "0.0.0.0";
        }

        mInfo->insert("MAC", mMac);
        mInfo->insert("IP", mIp);
        mInfo->insert("LOCALIP", localIp);

    }

    delete adapterinfo;

    emit ThreadFinish();

}

void NetScanThread::ThreadOver()
{
    qDebug() << "Get System's Details is Over!";
}
