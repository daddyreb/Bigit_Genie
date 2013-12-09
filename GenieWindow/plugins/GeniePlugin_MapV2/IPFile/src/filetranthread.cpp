#include "filetranthread.h"
#include "IFMsg.h"
#include <QMap>
#include <QFileInfo>
#include "filetranstatus.h"

extern QMap<qint64, FileStatus> sendFileStatusMap;
extern QMutex sendFileStatusMapMutex;

CFileTranThread::CFileTranThread(SOCKET s)
{
	m_bStop = false;
	m_socket = s;
	bool flg = true;
	m_fileID = 0;
	::ioctlsocket(m_socket, FIONBIO, (unsigned long *)&flg);
}

void CFileTranThread::run()
{
	FileStatus fileStatus;
	if(!CheckFileFromID(&fileStatus)) {
		return ;
	}
	StartSendFile(fileStatus);
}

int CFileTranThread::StartSendFile(FileStatus fileStatus)
{
	qint64 sendSum = 0;
	qint64 a = 0;
	for(int i=0; i<fileStatus.fileList.size(); i++) {
		m_stopMutex.lock();
		if(m_bStop) {
			m_stopMutex.unlock();
			return 0;
		}
		m_stopMutex.unlock();

		QString sendFile = fileStatus.fileList.at(i);
		SOCKET_STREAM_FILE_INFO socketStreamFileInfo;
		memset(&socketStreamFileInfo, 0, sizeof(SOCKET_STREAM_FILE_INFO));
		QFileInfo fileInfo = QFileInfo(fileStatus.fileList.at(i));
		socketStreamFileInfo.fileLen = fileInfo.size();
		QString fileTitle = sendFile.remove((fileStatus.filePath));
		strcpy(socketStreamFileInfo.szFileTitle, fileTitle.toLocal8Bit().data());
		if(!SendData((char*)&socketStreamFileInfo, sizeof(SOCKET_STREAM_FILE_INFO))) {
			//网络出错 或者 暂停
			return 0;
		}

		qint64 broken = 0;
		if(!RecvData((char*)&broken, sizeof(qint64))) {
			//网络出错 或者 暂停
			return 0;
		}

		QFile file(fileStatus.fileList.at(i));  
		bool ret = file.open(QIODevice::ReadOnly);
		if(!ret) {
			//打开文件出错
			return 0;
		}
		file.seek(broken);
		qint64 sendLen = broken;
		sendSum += sendLen;
		while(sendLen < socketStreamFileInfo.fileLen) {
			//发送文件
			char buf[IPFILE_BUFFERLEN];
			memset(buf, 0, IPFILE_BUFFERLEN);
			qint64 readFileLen = socketStreamFileInfo.fileLen-sendLen;
			if(readFileLen > IPFILE_BUFFERLEN) {
				file.read(buf, IPFILE_BUFFERLEN);
				sendLen += IPFILE_BUFFERLEN;
				sendSum += IPFILE_BUFFERLEN;
				if(!SendData(buf, IPFILE_BUFFERLEN)) {
					//网络出错 或者 暂停
					return 0;
				}
			} else {
				file.read(buf, readFileLen);
				sendLen += readFileLen;
				sendSum += readFileLen;
				if(!SendData(buf, readFileLen)) {
					//网络出错 或者 暂停
					return 0;
				}
			}
			qint64 b = sendSum*100/fileStatus.fileSize;
			if(a != b) {
				a = b;
				emit SendProgressSignal(fileStatus.fileSize, sendSum);
			}
		}
		//发文件头信息函数
		//发文件写文件函数
		//校验文件
	}

	sendFileStatusMapMutex.lock();
	if(sendFileStatusMap.contains(m_fileID)) {
		sendFileStatusMap.remove(m_fileID);
	}
	sendFileStatusMapMutex.unlock();

	closesocket(m_socket);
	emit SendCompleteSignal();
	return 1;
}


int CFileTranThread::CheckFileFromID(FileStatus *fileStatus)
{
	GetFileInfoData fileInfoData;
	memset(&fileInfoData, 0, sizeof(GetFileInfoData));
	if(!RecvData((char*)&fileInfoData, sizeof(GetFileInfoData))) {
		//网络出错 或者 暂停
		return 0;
	}
	if(fileInfoData.command != IPFILE_GETFILE) {
		return 0;
	}
	m_fileID = fileInfoData.fileID;
	sendFileStatusMapMutex.lock();
	QMap<qint64, FileStatus>::iterator iter = sendFileStatusMap.find(m_fileID);
	if(iter == sendFileStatusMap.end()) {
		sendFileStatusMapMutex.unlock();
		fileInfoData.command = IPFILE_NOFILESEND;
		return 0;
	}
	fileInfoData.fileLen = iter->fileSize;
	fileInfoData.fileNum = iter->fileList.size();
	fileStatus->fileList = iter->fileList;
	fileStatus->fileName = iter->fileName;
	fileStatus->filePath = iter->filePath;
	fileStatus->fileSize = iter->fileSize;
	iter->tranThread = this;
	//FileTranStatus *status = (FileTranStatus *)fileStatus->statusWidget;
	connect(this, SIGNAL(SendProgressSignal(qint64, qint64)), iter->statusWidget, SLOT(PogressSlot(qint64, qint64)));
	connect(this, SIGNAL(SendCompleteSignal()), iter->statusWidget, SLOT(CompleteSlot()));
	connect(this, SIGNAL(SendThreadErrorSignal(int )), iter->statusWidget, SLOT(RecvThreadErrorSlot(int )));
	sendFileStatusMapMutex.unlock();
	
	if(!SendData((char*)&fileInfoData, sizeof(GetFileInfoData))) {
		//网络出错 或者 暂停
		return 0;
	}
	return 1;;
}

int CFileTranThread::RecvData(char *buf, int len)
{
	fd_set		fds;
	timeval		tv;

	int recvCount = 0;
	int waitCount = 0;
	for(waitCount=0; (recvCount<len)&&(waitCount<IPFILE_SOCKETTIMEOUT); waitCount++) {
		m_stopMutex.lock();
		if(m_bStop) {
			m_stopMutex.unlock();
			emit PauseFileRecv();// 暂停
			return 0;
		}
		m_stopMutex.unlock();

		FD_ZERO(&fds);
		FD_SET(m_socket, &fds);
		tv.tv_sec = 1, tv.tv_usec = 0;
		int sock_ret = ::select(0, &fds, NULL, NULL, &tv);
		if(sock_ret>0 && FD_ISSET(m_socket, &fds)) {
			waitCount = 0;
			int recv_ret = recv(m_socket, buf+recvCount, len-recvCount, 0);
			if(recv_ret == SOCKET_ERROR) {
				emit SendThreadErrorSignal(WSAGetLastError());//网络错误
				return 0;
			} else {
				recvCount+=recv_ret;
			}
		}
	}
	if(waitCount >= IPFILE_SOCKETTIMEOUT) {
		emit SendThreadErrorSignal(WSAGetLastError());
		return 0;
	}
	return len;
}

int CFileTranThread::SendData(char *buf, int len)
{
	fd_set		fds;
	timeval		tv;

	int sendCount = 0;
	int waitCount = 0;
	for(waitCount=0; (sendCount<len)&&(waitCount<IPFILE_SOCKETTIMEOUT); waitCount++) {
		m_stopMutex.lock();
		if(m_bStop) {
			m_stopMutex.unlock();
			emit PauseFileRecv();//暂停
			return 0;
		}
		m_stopMutex.unlock();

		FD_ZERO(&fds);
		FD_SET(m_socket, &fds);
		tv.tv_sec = 1, tv.tv_usec = 0;
		int sock_ret = ::select(0, NULL, &fds, NULL, &tv);
		if(sock_ret>0 && FD_ISSET(m_socket, &fds)) {
			waitCount = 0;
			int send_ret = send(m_socket, buf+sendCount, len-sendCount, 0);
			if(send_ret == SOCKET_ERROR) {
				emit SendThreadErrorSignal(WSAGetLastError());//网络错误
				return 0;
			} else {
				sendCount += send_ret;
			}
		}
	}
	if(waitCount >= IPFILE_SOCKETTIMEOUT) {
		emit SendThreadErrorSignal(WSAGetLastError());
		return 0;
	}
	return len;
}

void CFileTranThread::stop()
{
	m_stopMutex.lock();
	m_bStop = true;
	m_stopMutex.unlock();
}
