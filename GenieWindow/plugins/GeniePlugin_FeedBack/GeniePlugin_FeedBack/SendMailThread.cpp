#include "SendMailThread.h"

////////////////////

typedef bool (*PFNCreateSslsmtp)(char **, const char *, const char *, const char *, const char*, int);
PFNCreateSslsmtp pfnSmtp;
SendMailThread::SendMailThread()
{
	MyLib = new QLibrary("SslMailSend");
	pfnSmtp = (PFNCreateSslsmtp)MyLib->resolve("doMain");
	m_errInfo=NULL;
}
SendMailThread::~SendMailThread()
{
	wait();
	MyLib->unload();
	delete MyLib;
	delete[] m_errInfo;
	m_errInfo = NULL;
}

void SendMailThread::runSend(const char* subject, const char *Sender, const char* MailTo, const char* msg, int mailIdx)
{
	strncpy(m_subject,subject,sizeof(m_subject));
	strncpy(m_MailTo, MailTo, sizeof(m_MailTo));
	strncpy(m_msg, msg, sizeof(m_msg));
	strncpy(m_Sender, Sender, sizeof(m_Sender));
	m_mailIdx = mailIdx;
	start();
}

void SendMailThread::run()
{
	m_errInfo = new char[100];
	mRet = (pfnSmtp)(&m_errInfo, m_subject, m_Sender, m_MailTo, m_msg, 0);
	errInfo = QString(QLatin1String(m_errInfo));


	

}