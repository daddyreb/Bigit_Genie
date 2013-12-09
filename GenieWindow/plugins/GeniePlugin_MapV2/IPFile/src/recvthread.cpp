#include "recvthread.h"
#include <WinSock2.h>
#include "IFMsg.h"
#include "userdlg.h"
#include "filetranstatus.h"
#include <QList>
#include <QMutex>
#include <QSettings>
#include <QFile>
#include <QFileInfo>
#include <QDir>

extern QMap<qint64, FileStatus> recvFileStatusMap;
extern QMutex recvFileStatusMapMutex;

extern QMap<QString, RecvStatus> recvStatusControlMap;
extern QMutex recvStatusControlMapMutex;

CRecvThread::CRecvThread(QString fileName, qint64 fileID, int cmd)
{
	m_fileID = fileID;
	m_fileName = fileName;
	m_cmd = cmd;
	m_bStop = false;
}

CRecvThread::~CRecvThread()
{

}

void CRecvThread::run()
{
	unsigned long addr;
	QMap<qint64, FileStatus>::iterator iter;
	recvFileStatusMapMutex.lock();	
	iter = recvFileStatusMap.find(m_fileID);
	if(iter == recvFileStatusMap.end()) {
		recvFileStatusMapMutex.unlock();
		return;
	}
	addr = iter->ip;
	recvFileStatusMapMutex.unlock();

	int socketRetn;
	SOCKADDR_IN  recvFileAddr;
	m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	recvFileAddr.sin_family = AF_INET;
	recvFileAddr.sin_port = htons(IPFILE_PORT);
	recvFileAddr.sin_addr.s_addr = addr;
	socketRetn = ::connect(m_socket, (SOCKADDR*)&recvFileAddr, sizeof(recvFileAddr));
	if(socketRetn == SOCKET_ERROR) {
		emit RecvThreadErrorSignal(WSAGetLastError());//网络连接失败信号
		closesocket(m_socket);
		return;
	}	

	bool flg = true;
	::ioctlsocket(m_socket, FIONBIO, (unsigned long *)&flg);

	GetFileInfoData fileInfoData;
	memset(&fileInfoData, 0, sizeof(GetFileInfoData));
	if(!GetFileFromID(m_fileID, &fileInfoData)) {
		return ;
	}

	if(fileInfoData.command == IPFILE_GETFILE) {
		StartRecvFile(fileInfoData);
	}
}

bool CRecvThread::GetFileFromID(qint64 m_fileID, GetFileInfoData *fileInfoData)
{
	GetFileInfoData fileData;
	memset(&fileData, 0, sizeof(GetFileInfoData));
	fileData.command = IPFILE_GETFILE;
	fileData.fileID = m_fileID;
	if(!SendData((char*)&fileData, sizeof fileData)) {
		//网络错误或者暂停
		return false;
	}

	if(!RecvData((char*)fileInfoData, sizeof(GetFileInfoData))) {
		//网络错误或者暂停
		return false;
	}

	return true;
}

int CRecvThread::StartRecvFile(GetFileInfoData fileInfoData)
{
	qint64 recvSum = 0;
	qint64 a = 0;
	recvFileStatusMapMutex.lock();
	QMap<qint64, FileStatus>::iterator iter1 = recvFileStatusMap.find(m_fileID);
	QString check = iter1->fileCheck;
	QString path = iter1->filePath;
	recvFileStatusMapMutex.unlock();

	m_IniMutex.lock();
	QSettings *fileStatus = new QSettings(IPFILE_STATUSFILE,  QSettings::IniFormat);
	fileStatus->setIniCodec("UTF-8");
	QString key = QString("%1/status").arg(check);
	fileStatus->setValue(key, 2);
	key = QString("%1/path").arg(check);
	fileStatus->setValue(key, path);
	delete fileStatus;
	m_IniMutex.unlock();

	RecvStatus rStatus;
	rStatus.filePath = path;
	rStatus.status = 2;

	recvStatusControlMapMutex.lock();
	recvStatusControlMap.insert(check, rStatus);
	recvStatusControlMapMutex.unlock();

	for(int i=0; i<fileInfoData.fileNum; i++) {
		m_stopMutex.lock();
		if(m_bStop) {
			m_stopMutex.unlock();
			return 0;
		}
		m_stopMutex.unlock();

		QString recvFile;
		SOCKET_STREAM_FILE_INFO socketStreamFileInfo;
		memset(&socketStreamFileInfo, 0, sizeof(SOCKET_STREAM_FILE_INFO));
		if(!RecvData((char*)&socketStreamFileInfo, sizeof(SOCKET_STREAM_FILE_INFO))) {
			return 0;
		}

		QString fileName = path+QString::fromLocal8Bit(socketStreamFileInfo.szFileTitle);
		QString realFileName = fileName;
		fileName = fileName + ".tmp";
		QString tempFileName = fileName;
		QFileInfo fileInfo(fileName);
		QString dirString = tempFileName.remove(fileInfo.fileName());//如果目录不存在创建目录
		QDir *dir = new QDir;
		bool exist = dir->exists(dirString);
		if(!exist) {
			exist = dir->mkpath(dirString);
			if(!exist) {
				//文件操作失败
				emit RecvOpenFileErrorSignal();
				return 0;
			}
		}

		QFile pfile(fileName);
		QFile realFile(realFileName);
		qint64 read;
		bool ret;
		if(m_cmd == 2) {
			if(realFile.exists()) {
				ret = realFile.open(QIODevice::WriteOnly | QIODevice::Append);
				read = realFile.pos();
			} else {
				ret = pfile.open(QIODevice::WriteOnly | QIODevice::Append);
				read = pfile.pos();
			}
		} else {
			if(realFile.exists()) {
				realFile.remove();
			}
			ret = pfile.open(QIODevice::WriteOnly);
			read = pfile.pos();
		}
		if(!ret) {
			//文件打开失败
			emit RecvOpenFileErrorSignal();
			return 0;
		}

		qint64 readLen = read;
		recvSum += read;
		if(!SendData((char*)&readLen, sizeof(qint64))) {
			//网络出错或者接受暂停
			return 0;
		}
		while(read<socketStreamFileInfo.fileLen){
			//接受文件
			char buf[IPFILE_BUFFERLEN];
			memset(buf, 0, IPFILE_BUFFERLEN);
			qint64 writeFileLen = socketStreamFileInfo.fileLen - read;
			if(writeFileLen > IPFILE_BUFFERLEN) {
				if(!RecvData(buf, IPFILE_BUFFERLEN)) {
					//网络出错或者接受暂停
					return 0;
				}
				if(-1 == pfile.write(buf, IPFILE_BUFFERLEN)) {
					emit RecvOpenFileErrorSignal();
					return 0;
				}
				read += IPFILE_BUFFERLEN;
				recvSum += IPFILE_BUFFERLEN;
			} else {
				if(!RecvData(buf, writeFileLen)) {
					//网络出错或者接受暂停
					return 0;
				}
				if(-1 == pfile.write(buf, writeFileLen)) {
					emit RecvOpenFileErrorSignal();
					return 0;
				}
				read += writeFileLen;
				recvSum += writeFileLen;
			}
			qint64 b = recvSum*100/fileInfoData.fileLen;
			if(a != b) {
				a = b;
				emit RecvProgressSignal(fileInfoData.fileLen, recvSum);
			}
		}
		pfile.close();
		pfile.rename(realFileName);
		//校验文件
	}

	recvFileStatusMapMutex.lock();
	if(recvFileStatusMap.contains(m_fileID)) {
		recvFileStatusMap.remove(m_fileID);
	}
	recvFileStatusMapMutex.unlock();

	recvStatusControlMapMutex.lock();
	recvStatusControlMap.remove(check);
	recvStatusControlMapMutex.unlock();

	m_IniMutex.lock();
	fileStatus = new QSettings(IPFILE_STATUSFILE,  QSettings::IniFormat);
	fileStatus->setIniCodec("UTF-8");
	//QString key = QString(check);
	fileStatus->remove(check);
	delete fileStatus;
	m_IniMutex.unlock();

	closesocket(m_socket);
	emit RecvCompleteSignal();
	return 1;
}


int CRecvThread::RecvData(char *buf, int len)
{
	fd_set		fds;
	timeval		tv;

	int recvCount = 0;
	int waitCount = 0;
	for(waitCount=0; (recvCount<len)&&(waitCount<IPFILE_SOCKETTIMEOUT); waitCount++) {
		m_stopMutex.lock();
		if(m_bStop) {
			m_stopMutex.unlock();
			emit PauseFileRecv();
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
				emit RecvThreadErrorSignal(WSAGetLastError());
				return 0;
			} else {
				recvCount+=recv_ret;
			}
		}
	}

	if(waitCount >= IPFILE_SOCKETTIMEOUT) {
		emit RecvThreadErrorSignal(WSAGetLastError());
		return 0;
	}
	return len;
}

int CRecvThread::SendData(char *buf, int len)
{
	fd_set		fds;
	timeval		tv;

	int sendCount = 0;
	int waitCount = 0;
	for(waitCount=0; (sendCount<len)&&(waitCount<IPFILE_SOCKETTIMEOUT); waitCount++) {
		m_stopMutex.lock();
		if(m_bStop) {
			m_stopMutex.unlock();
			emit PauseFileRecv();
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
				emit RecvThreadErrorSignal(WSAGetLastError());
				return 0;
			} else {
				sendCount += send_ret;
			}
		}
	}
	if(waitCount >= IPFILE_SOCKETTIMEOUT) {
		emit RecvThreadErrorSignal(WSAGetLastError());
		return 0;
	}
	return len;
}

void CRecvThread::stop()
{
	m_stopMutex.lock();
	m_bStop = true;
	m_stopMutex.unlock();
}
