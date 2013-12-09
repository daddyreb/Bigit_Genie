#ifndef FILENOTIFYMANAGER_H
#define FILENOTIFYMANAGER_H

class UDTProxy;
class NewFileNotifyWidget;

#include <QObject>
#include <QMap>
#include <QMutex>
#include <vector>
#include <QString>
#include <string>
#include "sendthread.h"

class FileNotifyManager : public QObject
{
	Q_OBJECT

public:
	FileNotifyManager(QObject *parent);
	~FileNotifyManager();


	
	void connResult(int sock,int seq);

	void sendFile(const char * pstrAddr, const int nPort, const wchar_t * pstrFileName, const char * pstrHostname, const char * pstrSendtype, const char * pstrRecvName, const char * pstrRecvType);
	void sendMultiFiles(const char * pstrAddr, const int nPort, const std::vector<std::wstring> strArray, const char * pstrHostName, const char * pstrSendtype, const char * pstrRecvName, const char * pstrRecvType);
	void sendFolderFiles(const char * pstrAddr, const int nPort, const wchar_t * pstrFolderName, const char * pstrHostName, const char * pstrSendtype, const char * pstrRecvName, const char * pstrRecvType);

signals:
	void saveResult(int sock,QString filepath);
	void stopTransfer(int sock);
	void queryDefaultRecvFolder(QString *folder);
	void requestStoreDefaultRecvFolder(const QString& folder);
	void translateText(int idx ,QString *text);

public slots:
	void onSendFile(int seq,QString fileName,int fileCount,int filetype);
	void onConnResult(int sock,int seq,int connResult);
	void onFileFinished(int sock,int nReturnCode);
	void onTransfer(int sock,QString fileName,double percent,double speed);
	void onRecvFile(int sock,int fileCount,QString fileName,QString recvDevice,QString sendType,QString fileType);
	void onRecvMessage(QString msg,QString ip,QString host);
	void onSaveResult(int sock,QString filepath);


	void onStopTransfer(int sock);
	void onFolderChange(QString strFolder);
private:
	QMap<int,NewFileNotifyWidget *> sockFileMap;
	NewFileNotifyWidget *fileWidget;
	QMap<int,int> seqSockMap;
	QString m_strLastFolder;
	sendthread *m_sendThread;
	int m_sendSeq;
	bool m_bSendingStatus;
	QMutex mutex;
};

#endif // FILENOTIFYMANAGER_H
