#ifndef UDTPROXY_H
#define UDTPROXY_H

#include <QObject>
#include <QString>
#include "UdtCore.h"
#include "filenotifymanager.h"
#include <vector>

class UDTProxy : public QObject, public CUDTCallBack
{
	Q_OBJECT
public:
	static UDTProxy * GetInStance();

	// �������񡢼����˿�
	int Init(const int nCtrlPort, const int nRcvPort);
	// �����ı���Ϣ
	void sendMessage(const char* pstrAddr, const int nPort, const char* pstrMessage, const char* pstrHostname, const char* pstrSendtype);
	// ���͵����ļ�
    void sendfile(const char* pstrAddr, const int nPort, const wchar_t * pstrFileName, const char* pstrHostname, const char *pstrSendtype, const char *pstrRecvName, const char *pstrRecvType,int seq = 0);
	// ���Ͷ���ļ�
	void sendMultiFiles(const char* pstrAddr, const int nPort, const std::vector<std::wstring> strArray, const char* pstrHostName, const char* pstrSendtype, const char *pstrRecvName, const char *pstrRecvType,int seq = 0);
	// �����ļ���
	void sendFolderFiles(const char* pstrAddr, const int nPort, const wchar_t * pstrFolderName, const char* pstrHostName, const char* pstrSendtype, const char *pstrRecvName, const char *pstrRecvType, int seq = 0);
	// ֹͣ���͡�ֹͣ����
	void stopTransfer(const int nType);
	// ֹͣ���񡢹رն˿�
	void stopListen();

	void TTSPing(const std::vector<std::string> vecIpAddress);

public slots:
	void onSaveResult(int sock,QString filepath);
	void onStopTransfer(int sock);

signals:
	void fileFinished(int sock,int nReturnCode);
	void transfer(int sock,QString fileName,double percent,double speed);
	void recvFile(int sock,int fileCount,QString fileName,QString recvDevice,QString sendType,QString fileType);
	void recvMessage(QString msg,QString ip,QString host);
	void sendFile(int seq,QString fileName,int fileCount,int filetype);
	void connResult(int sock,int seq,int result);
	void serviceOffline(QString strServiceIp,int type);

protected:
	// Call back
    //virtual void onAccept(const char* pstrAddr, const char* pstrFileName, int nFileCount, const char* recdevice, const char* rectype, const char* owndevice, const char* owntype, const char* SendType, const char* FileType, int sock);
    virtual void onAccept(const char* pstrAddr, const char* pstrFileName, int nFileCount, const int64_t nFileSize, const char* recdevice, const char* rectype, const char* owndevice, const char* owntype, const char* SendType, const char* FileType, int sock);
	virtual void onAcceptonFinish(const char* pstrAddr, const char* pFileName, int Type, int sock);
	virtual void onFinished(const char * pstrMsg, int Type, int sock);
	virtual void onTransfer(const int64_t nFileTotalSize, const int64_t nCurrent, const double dSpeed, const char* pstrFileName, int Type, int sock);
	virtual void onRecvMessage(const char* pstrMsg, const char* pIpAddr, const char* pHostName);
	virtual void onTTSPing(const char* pstrIp, int type);

private:
	UDTProxy();
	~UDTProxy();

private:
	static UDTProxy * m_pUdtProxy;
	CUdtCore * m_pUdt;
	int m_sock;
	int m_sendSeq;
	
};

#endif // UDTPROXY_H
