#include "udtproxy.h"
#include <QtDebug>

UDTProxy * UDTProxy::m_pUdtProxy = NULL;

UDTProxy::UDTProxy()
{
	m_sendSeq = 1;
	m_pUdt = new CUdtCore(this);
	
}


UDTProxy::~UDTProxy()
{
	delete m_pUdt;
	m_pUdt = NULL;
}

UDTProxy * UDTProxy::GetInStance()
{
	if (m_pUdtProxy == NULL)
	{
		m_pUdtProxy = new UDTProxy();
	}

	return m_pUdtProxy;
}


int UDTProxy::Init(const int nCtrlPort, const int nRcvPort)
{
	if (m_pUdt->StartListen(nCtrlPort, nRcvPort) < 0)
		return -1;

	return 0;
}

void UDTProxy::sendMessage(const char* pstrAddr, const int nPort, const char* pstrMessage, const char* pstrHostname, const char* pstrSendtype)
{
	std::vector<std::string> vecTexts;
	vecTexts.push_back(pstrMessage);
    //m_sock = m_pUdt->SendFiles(pstrAddr, vecTexts, pstrHostname, pstrHostname, pstrHostname, pstrHostname, pstrSendtype);
}

void UDTProxy::sendfile(const char* pstrAddr, const int nPort, const wchar_t * pstrFileName, const char* pstrHostname, const char *pstrSendtype, const char *pstrRecvName, const char *pstrRecvType,int seq)
{
    std::vector<std::wstring> vecTexts;
	vecTexts.push_back(pstrFileName);
	//int seq = m_sendSeq;
	//emit sendFile(m_sendSeq,QString::fromLocal8Bit(pstrFileName),1,0);
	//m_sendSeq++;
	int iResult = 0;
    m_sock = m_pUdt->SendFiles(pstrAddr, vecTexts, pstrHostname, pstrSendtype, pstrRecvName, pstrRecvType, "GENIETURBO", iResult);
	emit connResult(m_sock,seq,iResult);
}

void UDTProxy::sendMultiFiles(const char* pstrAddr, const int nPort, const std::vector<std::wstring> strArray, const char* pstrHostName, const char* pstrSendtype, const char *pstrRecvName, const char *pstrRecvType,int seq)
{
	//int seq = m_sendSeq;
	//emit sendFile(m_sendSeq,QString::fromLocal8Bit(strArray.at(0).c_str()),strArray.size(),2);
	//m_sendSeq++;
	int iResult = 0;
    m_sock = m_pUdt->SendFiles(pstrAddr, strArray, pstrHostName, pstrSendtype, pstrRecvName, pstrRecvType, "GENIETURBO", iResult);
	emit connResult(m_sock,seq,iResult);
}

void UDTProxy::sendFolderFiles(const char* pstrAddr, const int nPort, const wchar_t * pstrFolderName, const char* pstrHostName, const char* pstrSendtype, const char *pstrRecvName, const char *pstrRecvType,int seq)
{
	int iResult = 0;
	std::vector<std::wstring> vecTexts;
	vecTexts.push_back(pstrFolderName);
	//int seq = m_sendSeq;
	//emit sendFile(m_sendSeq,QString::fromLocal8Bit(pstrFolderName),1,1);
	//m_sendSeq++;
	m_sock = m_pUdt->SendFiles(pstrAddr, vecTexts, pstrHostName, pstrSendtype, pstrRecvName, pstrRecvType, "GENIETURBO", iResult);
	emit connResult(m_sock,seq,iResult);
}

void UDTProxy::stopTransfer(const int nType)
{
	m_pUdt->StopTransfer(m_sock, nType);
}

void UDTProxy::stopListen()
{
	m_pUdt->StopListen();
}

void UDTProxy::TTSPing(const std::vector<std::string> vecIpAddress)
{
    //m_pUdt->TTSPing(vecIpAddress);
}


//////////////////////////////////////////////////////////////////////////
// slot
void UDTProxy::onSaveResult(int sock,QString filepath)
{
	//QByteArray btFile = filepath.toUtf8();
	std::wstring wstrFilePath = filepath.toStdWString();
	//char *szFilePath = btFile.constData();
	
	m_pUdt->ReplyAccept(sock,wstrFilePath.c_str());
}


void UDTProxy::onStopTransfer(int sock)
{
	m_pUdt->StopTransfer(sock,1);
}

//////////////////////////////////////////////////////////////////////////
// Call Back
void UDTProxy::onAccept(const char* pstrAddr, const char* pstrFileName, int nFileCount, const int64_t nFileSize, const char* recdevice, const char* rectype, const char* owndevice, const char* owntype, const char* SendType, const char* FileType, int sock)
{
	Q_UNUSED(owntype);
	Q_UNUSED(owntype);
	Q_UNUSED(rectype);
	Q_UNUSED(nFileSize);
	Q_UNUSED(owndevice);
	Q_UNUSED(pstrAddr);

	std::cout<< "Accept file name:" << pstrFileName << std::endl;

	char path[MAX_PATH];
#if WIN32
	GetCurrentDirectoryA(MAX_PATH, path);
#else
	if (NULL == getcwd(path, MAX_PATH))
		Error("Unable to get current path");
#endif

	if ('\\' != path[strlen(path)-1])
	{
		strcat(path, ("\\"));
	}

	emit recvFile(sock,nFileCount,pstrFileName,recdevice,SendType,FileType);
}

void UDTProxy::onAcceptonFinish(const char* pstrAddr, const char* pFileName, int Type, int sock)
{
	Q_UNUSED(sock);
	Q_UNUSED(Type);
	Q_UNUSED(pstrAddr);
	Q_UNUSED(pFileName);
}

void UDTProxy::onFinished(const char * pstrMsg, int Type, int sock)
{
	Q_UNUSED(pstrMsg);

	emit fileFinished(sock,Type);
}

void UDTProxy::onTransfer(const int64_t nFileTotalSize, const int64_t nCurrent, const double dSpeed, const char* pstrFileName, int Type, int sock)
{
	Q_UNUSED(Type);

	double dTotal = (double)nFileTotalSize;
	double dPercent = nCurrent / dTotal;

	//if (1 == Type)
	//{
	//	QString strFile = QString::fromLocal8Bit(pstrFileName);
	//	emit transfer(sock,strFile,dPercent,dSpeed);
	//}
	//else	
	//{
		emit transfer(sock,pstrFileName,dPercent,dSpeed);
	//}
	
}

void UDTProxy::onRecvMessage(const char* pstrMsg, const char* pIpAddr, const char* pHostName)
{
	emit recvMessage(pstrMsg,pIpAddr,pHostName);
}

void UDTProxy::onTTSPing(const char* pstrIp, int type)
{
	emit serviceOffline(pstrIp,type);
}
