#ifndef RECVTHREAD_H
#define RECVTHREAD_H
#include <QString>
#include <QThread>
#include <WinSock2.h>
#include <QMutex>

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
class CRecvThread : public QThread
{
    Q_OBJECT

public:
    CRecvThread(QString filePath, qint64 fileID, int cmd);
	~CRecvThread();
private:
	int m_cmd; //2 续传
	QString m_fileName;
	qint64 m_fileID;
	SOCKET m_socket;
	bool m_bStop;
	QMutex m_stopMutex;
		QMutex m_IniMutex;
private:
	bool GetFileFromID(qint64 fileID, GetFileInfoData *fileInfoData);
	int RecvData(char *buf, int len);
	int SendData(char *buf, int len);
	int StartRecvFile(GetFileInfoData fileInfoData);
public:
	void stop();
protected:
	void run();
signals:
	void RecvThreadErrorSignal(int err);
	void PauseFileRecv();
	void RecvProgressSignal(qint64, qint64);
	void RecvCompleteSignal();
	void RecvOpenFileErrorSignal();
};

#endif // RECVTHREAD_H
