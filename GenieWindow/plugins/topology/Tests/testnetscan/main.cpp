#include <stdio.h>
#include <QtCore>
#include <QApplication>
#include "topology.h"

int main(int argc,char ** argv)
{
    QApplication a(argc,argv);
    SVT_UnixProcessHelper helper;
    QStringList retlst;
  //  helper.doCmd("ls",retlst);
    helper.doCmd("./ifaceinfo.sh",retlst);

    qDebug()<<qApp->applicationDirPath();
    qDebug()<<UNIX_SCRIPTS_DIR<<retlst;
    return 0;
    QList<QNetworkInterface> list = QNetworkInterface::allInterfaces();
     foreach(QNetworkInterface iface,list)
     {
 #if 1
         qDebug() << "Device: "<<iface.humanReadableName();
         qDebug() << "Index: "<<iface.index();
         qDebug() << "Name: "<<iface.name();
         qDebug() << "HardwareAddress: "<<iface.hardwareAddress();
 #endif
//         mLocoalMacList << iface.hardwareAddress();

         QList<QNetworkAddressEntry> entryList = iface.addressEntries();

         foreach(QNetworkAddressEntry entry,entryList)
         {
             qDebug()<<"IP Address: "<<entry.ip().toString();

             qDebug()<<"netmask: " << entry.netmask().toString();

             qDebug()<<"Broadcast: "<<entry.broadcast().toString();

             qDebug()<<"Proxy: "<<entry.prefixLength();
             qDebug()<<" ";
         }
         qDebug()<<"///////////////////////////////////////// ";
     }

	//SVT_MacAddress mac;
	//mac.SetAddress((unsigned char *)"\x00\x0c\x29\x00\x00",6);
	//    const  char *macaddress=(char *)mac.asBytes();
	//bool test=(macaddress[0]==(unsigned char)'0x00');

	//test=(macaddress[1]==(char)'0x0c');
	//test=(macaddress[2]=='0x29');
	//printf(" %d %d",macaddress[1],'\x0c');
	////qDebug()<<'\x0c';	

	SVT_Core core;
	QList<SVT_IpAddress> testmap;
        QList<SVT_NetworkInterface> ret;
        core.mCoreUtils.netscanGetAllAdpInfo(ret);
   return 0;

#if 0
	SVT_IpAddress ip;
	QMap<int,QString> retmap;
    QString host,version,machost,macversion;
	unsigned long _ip=(192<<24)|(168<<16)|(9<<8)|(17);
	ip.Set(_ip);
	for(int i=0;i<10;i++)
	{
		core.mCoreUtils.netbiosGetDevInfo(ip,retmap);
		qDebug()<<retmap;
		Sleep(2000);
	}


#endif
#if 0
    QList<SVT_NetworkInterface> nlist;
    util.netscanGetAllAdpInfo(nlist);
    foreach(SVT_NetworkInterface nf,nlist)
    {
        qDebug()<<nf.getIp().toString()<<nf.getMask().toString()<<nf.getGateway().toString()
                  <<nf.getMac().toString();
    }
#endif
#if 0
    SVT_Core core;
    core.init();
    core.start();
#endif
	getchar();
   // Sleep(1000);


    return 0;
}
