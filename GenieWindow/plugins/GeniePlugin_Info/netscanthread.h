#ifndef GETTHREAD_H
#define GETTHREAD_H

#include <QThread>
#include <QMap>
#include <QStringList>
#include <QLibrary>

//#include "getsystemdetails.h"


#define DLL_NetSCAN_FILE_NAME "./netscan.dll"
#define MAX_ADAPTER_NAME_LENGTH 256
typedef void (*GetAdapterInfo) (struct AdapterInfo *adapter);

struct AdapterInfo
{
    unsigned long m_index;
    char	  m_local_mac[18];
    char	  m_adapter_name[MAX_ADAPTER_NAME_LENGTH + 4];
    char    	  m_gateway[16];
    char 	  m_local_ip[16];
    char       	  m_mask[16];
};


class NetScanThread : public QThread
{
    Q_OBJECT
public:
    NetScanThread(QObject *parent=0);
    ~NetScanThread();

    void StartGetGatewayInfo(QMap<QString, QString> * *OutData);

protected:
     void run();


signals:
    void ThreadFinish();

public slots:
    void ThreadOver();

private:
    QString m_filename;
    QLibrary *m_myLib;
    GetAdapterInfo m_getadapterinfo;

    QMap<QString, QString> *mInfo;
    //struct AdapterInfo *adapterinfo;



};




#endif // GETTHREAD_H
