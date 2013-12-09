#ifndef IPFILEMSG_H_
#define IPFILEMSG_H_
#include <WinSock2.h>
#include <QWidget>
#include <QString>
#include <QThread>
#include <windows.h>
#define  MAX_NAMEBUF 80
#define  MAX_UDPBUF 32768

struct RecvBuf {
	struct sockaddr_in	addr;
	int					addrSize;
	int					size;
	char				msgBuf[MAX_UDPBUF];
};

struct HostSub {
	char	userName[MAX_NAMEBUF];
	char	hostName[MAX_NAMEBUF];
	unsigned long addr;
	int		portNo;
};


struct MsgBuf {
	HostSub	hostSub;
	int		version;
	int		portNo;
	unsigned long	packetNo;
	unsigned long	command;
	char	msgBuf[MAX_UDPBUF];
	char	exBuf[MAX_UDPBUF];
	char	packetNoStr[256/8]; // for IV (256bit)

	MsgBuf() {
		*msgBuf = 0;
		*exBuf = 0;
		*packetNoStr = 0;
		memset(msgBuf, 0, MAX_UDPBUF);
		memset(exBuf, 0, MAX_UDPBUF);
	}

	void	Init(MsgBuf *org) {
		if (org == NULL) {
			memset(this, 0, (char *)&this->msgBuf - (char *)this);
			*msgBuf = 0;
			*exBuf = 0;
			*packetNoStr = 0;
			return;
		}
		memcpy(this, org, (char *)&this->msgBuf - (char *)this);
		strcpy(this->msgBuf, org->msgBuf);
		strcpy(this->exBuf, org->exBuf);
		strcpy(this->packetNoStr, org->packetNoStr);
	}
};

struct SendFileThread{
	QThread *sendThread;
};

struct FileStatus{
	qint64 fileID;
	unsigned long ip;
	QString fileName;
	QString filePath;
	qint64 fileSize;
	int fileType; //0文件，1文件夹
	int sendOrRecv;//0表示发送文件，1表示接受文件	
	QWidget* statusWidget;
	QWidget* userDlg;
	QList<QString> fileList;
	QString fileCheck;
	QThread* tranThread;
	SOCKET s;
};

enum {readyRecv, recving, endRecv};//准备接收， 正在接收， 接收完成

struct RecvStatus{
	int status; //
	QString filePath;
};

char *separate_token(char *buf, char separetor, char **handle=NULL);

#endif //IPFILEMSG_H_
