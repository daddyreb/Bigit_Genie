#ifndef FILETRANTHREAD_H
#define FILETRANTHREAD_H
#include <QThread>
#include <WinSock2.h>
#include <QMutex>
#include "IPFileMsg.h"
#ifndef DATA_HEAD
#define DATA_HEAD
struct GetFileInfoData{
	unsigned long command;
	qint64 fileID;
	int fileNum;
	qint64 fileLen;
};

typedef struct _SOCKET_STREAM_FILE_INFO {
	char   szFileTitle[1024];                   //文件的标题名
	qint64 fileLen;                            //文件长度
} SOCKET_STREAM_FILE_INFO, * PSOCKET_STREAM_FILE_INFO;
#endif

class CFileTranThread : public QThread
{
	Q_OBJECT
public:
    CFileTranThread(SOCKET s);
private:
	SOCKET m_socket;
	bool m_bStop;
	QMutex m_stopMutex;
	qint64 m_fileID;
protected:
	void run();
signals:
	void SendThreadErrorSignal(int);
	void PauseFileRecv();
	void SendProgressSignal(qint64, qint64);
	void SendCompleteSignal();
private:
	int RecvData(char *buf, int len);
	int SendData(char *buf, int len);
	int CheckFileFromID(FileStatus *fileStatus);
	int StartSendFile(FileStatus fileStatus);
public:
	void stop();
};

#endif // FILETRANTHREAD_H
