#ifndef _SENDMAILTHREAD_H
#define _SENDMAILTHREAD_H

#include <QtCore>
#include <QLibrary>
#include <QString>

class SendMailThread : public QThread
{
	Q_OBJECT
public:
	SendMailThread();
	~SendMailThread();
	void runSend(const char*, const char*, const char*, const char*, int);
	void run();
private:
	char *m_errInfo;
	char m_subject[256];
	char m_MailTo[128];
	char m_msg[1024];
	char m_Sender[256];
	int m_mailIdx;
	QLibrary *MyLib;
public:
	bool mRet;
	QString errInfo;
};


#endif