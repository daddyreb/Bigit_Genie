#ifndef SENDTHREAD_H
#define SENDTHREAD_H

#include <QThread>
#include <vector>
#include <string>

class sendthread : public QThread
{
    Q_OBJECT
public:
    explicit sendthread(QObject *parent = 0);

	void sendFile(const char * pstrAddr, const int nPort, const wchar_t * pstrFileName, const char * pstrHostname, const char * pstrSendtype, const char * pstrRecvName, const char * pstrRecvType,int seq);
	void sendMultiFiles(const char * pstrAddr, const int nPort, const std::vector<std::wstring> strArray, const char * pstrHostName, const char * pstrSendtype, const char * pstrRecvName, const char * pstrRecvType,int seq);
	void sendFolderFiles(const char * pstrAddr, const int nPort, const wchar_t * pstrFolderName, const char * pstrHostName, const char * pstrSendtype, const char * pstrRecvName, const char * pstrRecvType,int seq);
	void run();
signals:

public slots:

private:
	std::string m_strAddr;
	std::string m_strHostName;
	std::string m_strSendType;
	std::string m_strRecvName;
	std::string m_strRecvType;
	std::wstring m_pstrFileName;
	int m_port;
	int m_type;
	int m_seq;
	std::vector<std::wstring> m_strArray;
};

#endif // SENDTHREAD_H
