#include "LookUpThread.h"
#include <QTime>

LookUpThread::LookUpThread()
{
}

void LookUpThread::run()
{
	QTime hostInfoTime;
	hostInfoTime.start();
    QHostInfo hostinfo = QHostInfo::fromName("www.netgear.com");
	int ms = hostInfoTime.elapsed();
	if(ms<3000)
	{
		if(hostinfo.error() == QHostInfo::NoError)
		{
			emit SendStatus(true);
		}
		else
		{
			emit SendStatus(false);
		}
	}
}
