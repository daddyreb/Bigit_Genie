#include "sendthread.h"
#include "udtproxy.h"

sendthread::sendthread(QObject *parent) :
    QThread(parent)
	,m_port(0)
	,m_type(0)
	,m_seq(0)
{
}

void sendthread::run()
{
	if (0 == m_type)
	{
		UDTProxy::GetInStance()->sendfile(m_strAddr.c_str(), 7000, m_pstrFileName.c_str(), m_strHostName.c_str(), "Windows", m_strRecvName.c_str(),m_strRecvType.c_str(),m_seq);
	}
	else if (1 == m_type)
	{
		UDTProxy::GetInStance()->sendMultiFiles(m_strAddr.c_str(),7000,m_strArray,m_strHostName.c_str(), "Windows", m_strRecvName.c_str(),m_strRecvType.c_str(),m_seq);
	}
	else 
	{
		UDTProxy::GetInStance()->sendFolderFiles(m_strAddr.c_str(),7000,m_pstrFileName.c_str(), m_strHostName.c_str(),"Windows", m_strRecvName.c_str(),m_strRecvType.c_str(),m_seq);
	}
}

void sendthread::sendFile(const char * pstrAddr, const int nPort, const wchar_t * pstrFileName, const char * pstrHostname, const char * pstrSendtype, const char * pstrRecvName, const char * pstrRecvType,int seq)
{
	m_strAddr = pstrAddr;
	m_strHostName = pstrHostname;
	m_strSendType = pstrSendtype;
	m_strRecvName = pstrRecvName;
	m_strRecvType = pstrRecvType;
	m_port = nPort;
	m_type = 0;
	m_seq = seq;
	m_pstrFileName = pstrFileName;
}

void sendthread::sendMultiFiles(const char * pstrAddr, const int nPort, const std::vector<std::wstring> strArray, const char * pstrHostName, const char * pstrSendtype, const char * pstrRecvName, const char * pstrRecvType, int seq)
{
	m_strAddr = pstrAddr;
	m_strHostName = pstrHostName;
	m_strSendType = pstrSendtype;
	m_strRecvName = pstrRecvName;
	m_strRecvType = pstrRecvType;
	m_port = nPort;
	m_type = 1;
	m_seq = seq;
	m_strArray = strArray;
}

void sendthread::sendFolderFiles(const char * pstrAddr, const int nPort, const wchar_t * pstrFolderName, const char * pstrHostName, const char * pstrSendtype, const char * pstrRecvName, const char * pstrRecvType, int seq)
{
	m_strAddr = pstrAddr;
	m_strHostName = pstrHostName;
	m_strSendType = pstrSendtype;
	m_strRecvName = pstrRecvName;
	m_strRecvType = pstrRecvType;
	m_port = nPort;
	m_type = 2;
	m_seq = seq;
	m_pstrFileName = pstrFolderName;
}