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

	// 开启服务、监听端口
	int Init(const int nCtrlPort, const int nRcvPort);
	// 发送文本消息
	void sendMessage(const char* pstrAddr, const int nPort, const char* pstrMessage, const char* pstrHostname, const char* pstrSendtype);
	// 发送单个文件
    void sendfile(const char* pstrAddr, const int nPort, const wchar_t * pstrFileName, const char* pstrHostname, const char *pstrSendtype, const char *pstrRecvName, const char *pstrRecvType,int seq = 0);
	// 发送多个文件
	void sendMultiFiles(const char* pstrAddr, const int nPort, const std::vector<std::wstring> strArray, const char* pstrHostName, const char* pstrSendtype, const char *pstrRecvName, const char *pstrRecvType,int seq = 0);
	// 发送文件夹
	void sendFolderFiles(const char* pstrAddr, const int nPort, const wchar_t * pstrFolderName, const char* pstrHostName, const char* pstrSendtype, const char *pstrRecvName, const char *pstrRecvType, int seq = 0);
	// 停止发送、停止接收
	void stopTransfer(const int nType);
	// 停止服务、关闭端口
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
