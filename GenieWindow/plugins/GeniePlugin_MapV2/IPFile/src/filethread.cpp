#include "filethread.h"
#include <WinSock2.h>
#include "FileTranThread.h"
#include <QList>
#include "IPFileMsg.h"

CFileThread::CFileThread(int port)
{
	m_port = port;
	m_bStop = false;
}

CFileThread::~CFileThread()
{
	
}

void CFileThread::run()
{	
	SOCKET listeningSocket;
	SOCKADDR_IN serverAddr;
	SOCKADDR_IN clientAddr;

	if((listeningSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET) {
		int err = WSAGetLastError();
		emit ErrorOccurSignal(QString::fromLocal8Bit("create socket failed, error code: %1").arg(err));
		return;
	}

	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(m_port);
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	if(SOCKET_ERROR == bind(listeningSocket, (SOCKADDR *)&serverAddr, sizeof(serverAddr))) {
		int err = WSAGetLastError();
		emit ErrorOccurSignal(QString::fromLocal8Bit("bind socket failed, error code: %1").arg(err));
		return;
	}

	if(SOCKET_ERROR == listen(listeningSocket, 10)) {
		int err = WSAGetLastError();
		emit ErrorOccurSignal(QString::fromLocal8Bit("listen socket failed, error code: %1").arg(err));
		return;
	}

	fd_set fdListen;
	timeval seltime;
	seltime.tv_sec = 0;
	seltime.tv_usec = 1000;

	
	while(true) {
		m_stopMutex.lock();
		if(m_bStop) {
			break;
		}
		m_stopMutex.unlock();
		FD_ZERO(&fdListen);
		FD_SET(listeningSocket, &fdListen);

		if(select(0, &fdListen, NULL, NULL, &seltime) <= 0 || !FD_ISSET(listeningSocket, &fdListen) )
			continue;

		int clientAddrLen = sizeof clientAddr;
		SOCKET acceptSocket = accept(listeningSocket, (SOCKADDR *)&clientAddr, &clientAddrLen);
		CFileTranThread * acceptThread = new CFileTranThread(acceptSocket);
		acceptThread->start();
	}
	m_stopMutex.unlock();

	closesocket(listeningSocket);
}

void CFileThread::Stop()
{
	m_stopMutex.lock();
	m_bStop = true;
	m_stopMutex.unlock();
}