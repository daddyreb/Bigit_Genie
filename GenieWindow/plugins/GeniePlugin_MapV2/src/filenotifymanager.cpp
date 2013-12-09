#include "filenotifymanager.h"
#include "msgnotifywidget.h"
#include "udtproxy.h"
#include "newfilenotifywidget.h"

FileNotifyManager::FileNotifyManager(QObject *parent)
	: QObject(parent)
{
	fileWidget = NULL;
	m_sendThread = NULL;
	m_sendSeq = 1;
	m_bSendingStatus = false;
}

FileNotifyManager::~FileNotifyManager()
{

}


void FileNotifyManager::connResult(int sock,int seq)
{
	mutex.lock();
	NewFileNotifyWidget *fileWidget = NULL;
	if (sockFileMap.find(seq) != sockFileMap.end())
	{
		fileWidget = sockFileMap.value(seq);
	}
	if (NULL != fileWidget)
	{
		if (0 == sock)
		{
			fileWidget->closeWidget(0);
			m_bSendingStatus = false;
			sockFileMap.remove(seq);
		}
		else
		{
			seqSockMap.insert(sock,seq);
			if (sockFileMap.find(seq) != sockFileMap.end())
			{
				fileWidget = sockFileMap.value(seq);
				fileWidget->setSock(sock);
			}
		}
	}
	mutex.unlock();
}

void FileNotifyManager::sendFile(const char * pstrAddr, const int nPort, const wchar_t * pstrFileName, const char * pstrHostname, const char * pstrSendtype, const char * pstrRecvName, const char * pstrRecvType)
{
	mutex.lock();
	if (!m_bSendingStatus)
	{
		m_bSendingStatus = true;
		int seq = m_sendSeq;
		m_sendSeq++;

		fileWidget = new NewFileNotifyWidget(1,WC2CU(pstrFileName),seq,0,0,qobject_cast<QWidget*>(parent()));
		connect(fileWidget,SIGNAL(stopTransfer(int)),this,SLOT(onStopTransfer(int)));
		connect(fileWidget,SIGNAL(translateText(int,QString*)),this,SIGNAL(translateText(int,QString*)));
		fileWidget->notifyNewDevice(WC2CU(pstrFileName),1);

		m_sendThread = new sendthread();
		m_sendThread->sendFile(pstrAddr,nPort,pstrFileName,pstrHostname,pstrSendtype,pstrRecvName,pstrRecvType,seq);
		m_sendThread->start();
		m_bSendingStatus = true;
		sockFileMap.insert(seq,fileWidget);
	}
	else
	{
		int seq = m_sendSeq;
		m_sendSeq++;

		fileWidget = new NewFileNotifyWidget(1,WC2CU(pstrFileName),seq,0,0,qobject_cast<QWidget*>(parent()));
		connect(fileWidget,SIGNAL(stopTransfer(int)),this,SLOT(onStopTransfer(int)));
		connect(fileWidget,SIGNAL(translateText(int,QString*)),this,SIGNAL(translateText(int,QString*)));
		fileWidget->notifyNewDevice(WC2CU(pstrFileName),1);


		fileWidget->finishReuslt(113);
	}
	mutex.unlock();
}

void FileNotifyManager::sendMultiFiles(const char * pstrAddr, const int nPort, const std::vector<std::wstring> strArray, const char * pstrHostName, const char * pstrSendtype, const char * pstrRecvName, const char * pstrRecvType)
{
	mutex.lock();
	if (!m_bSendingStatus)
	{
		int seq = m_sendSeq;
		m_sendSeq++;

		fileWidget = new NewFileNotifyWidget(1,pstrHostName,seq,2,0,qobject_cast<QWidget*>(parent()));
		connect(fileWidget,SIGNAL(stopTransfer(int)),this,SLOT(onStopTransfer(int)));
		connect(fileWidget,SIGNAL(translateText(int,QString*)),this,SIGNAL(translateText(int,QString*)));
		fileWidget->notifyNewDevice(WC2CU(strArray.at(0).c_str()),strArray.size());

		m_sendThread = new sendthread();
		m_sendThread->sendMultiFiles(pstrAddr,nPort,strArray,pstrHostName,pstrSendtype,pstrRecvName,pstrRecvType,seq);
		m_sendThread->start();
		m_bSendingStatus = true;
		sockFileMap.insert(seq,fileWidget);
	}
	else
	{
		int seq = m_sendSeq;
		m_sendSeq++;

		fileWidget = new NewFileNotifyWidget(1,pstrHostName,seq,0,0,qobject_cast<QWidget*>(parent()));
		connect(fileWidget,SIGNAL(stopTransfer(int)),this,SLOT(onStopTransfer(int)));
		connect(fileWidget,SIGNAL(translateText(int,QString*)),this,SIGNAL(translateText(int,QString*)));
		fileWidget->notifyNewDevice(WC2CU(strArray.at(0).c_str()),1);

		fileWidget->finishReuslt(113);
	}
	mutex.unlock();
}

void FileNotifyManager::sendFolderFiles(const char * pstrAddr, const int nPort, const wchar_t * pstrFolderName, const char * pstrHostName, const char * pstrSendtype, const char * pstrRecvName, const char * pstrRecvType)
{
	mutex.lock();
	if (!m_bSendingStatus)
	{
		int seq = m_sendSeq;
		m_sendSeq++;

		fileWidget = new NewFileNotifyWidget(1,WC2CU(pstrFolderName),seq,1,0,qobject_cast<QWidget*>(parent()));
		connect(fileWidget,SIGNAL(stopTransfer(int)),this,SLOT(onStopTransfer(int)));
		connect(fileWidget,SIGNAL(translateText(int,QString*)),this,SIGNAL(translateText(int,QString*)));
		fileWidget->notifyNewDevice(WC2CU(pstrFolderName),1);

		m_sendThread = new sendthread();
		m_sendThread->sendFolderFiles(pstrAddr,nPort,pstrFolderName,pstrHostName,pstrSendtype,pstrRecvName,pstrRecvType,seq);
		m_sendThread->start();
		m_bSendingStatus = true;
		sockFileMap.insert(seq,fileWidget);
	}
	else
	{
		int seq = m_sendSeq;
		m_sendSeq++;

		fileWidget = new NewFileNotifyWidget(1,WC2CU(pstrFolderName),seq,0,0,qobject_cast<QWidget*>(parent()));
		connect(fileWidget,SIGNAL(stopTransfer(int)),this,SLOT(onStopTransfer(int)));
		connect(fileWidget,SIGNAL(translateText(int,QString*)),this,SIGNAL(translateText(int,QString*)));
		fileWidget->notifyNewDevice(WC2CU(pstrFolderName),1);

		fileWidget->finishReuslt(113);
	}
	mutex.unlock();
}



void FileNotifyManager::onConnResult(int sock,int seq,int result)
{
	mutex.lock();
	NewFileNotifyWidget *fileWidget = NULL;
	if (sockFileMap.find(seq) != sockFileMap.end())
	{
		fileWidget = sockFileMap.value(seq);
	}
	if (NULL != fileWidget)
	{
		if (0 == sock)
		{
			if (0 != result)
			{
				fileWidget->finishReuslt(result);	
			}		
			//fileWidget->closeWidget(0);
			m_bSendingStatus = false;
			sockFileMap.remove(seq);
		}
		else
		{
			seqSockMap.insert(sock,seq);
			if (sockFileMap.find(seq) != sockFileMap.end())
			{
				fileWidget = sockFileMap.value(seq);
				fileWidget->setSock(sock);
			}
		}
	}
	mutex.unlock();
}

void FileNotifyManager::onSendFile(int seq,QString fileName,int fileCount,int filetype)
{
	fileWidget = new NewFileNotifyWidget(1,fileName,seq,filetype,0,qobject_cast<QWidget*>(parent()));
	connect(fileWidget,SIGNAL(stopTransfer(int)),this,SLOT(onStopTransfer(int)));
	fileWidget->notifyNewDevice(fileName,fileCount);
	mutex.lock();
	sockFileMap.insert(seq,fileWidget);
	mutex.unlock();
}



///////////////////////////////////////////////////////
//slot
void FileNotifyManager::onFileFinished(int sock,int nReturnCode)
{
	mutex.lock();
	//if (114 == nReturnCode)
	//{
		m_bSendingStatus = false;
	//}
	NewFileNotifyWidget *fileWidget = NULL;
	if (seqSockMap.find(sock) != seqSockMap.end())
	{
		int iSock = seqSockMap.value(sock);
		seqSockMap.remove(sock);
		if (sockFileMap.find(iSock) != sockFileMap.end())
		{
			fileWidget = sockFileMap.value(iSock);
			fileWidget->finishReuslt(nReturnCode);
			seqSockMap.remove(iSock);
		}
	}
	else
	{
		if (sockFileMap.find(sock) != sockFileMap.end())
		{
			fileWidget = sockFileMap.value(sock);
			fileWidget->finishReuslt(nReturnCode);
			sockFileMap.remove(sock);
		}
	}
	mutex.unlock();
}
	
void FileNotifyManager::onTransfer(int sock,QString fileName,double percent,double speed)
{
	mutex.lock();
	NewFileNotifyWidget *fileWidget = NULL;
	if (seqSockMap.find(sock) != seqSockMap.end())
	{
		int iSock = seqSockMap.value(sock);
		if (sockFileMap.find(iSock) != sockFileMap.end())
		{
			fileWidget = sockFileMap.value(iSock);
			fileWidget->transferPercent(percent,speed,fileName);
		}
	}
	else
	{
		if (sockFileMap.find(sock) != sockFileMap.end())
		{
			fileWidget = sockFileMap.value(sock);
			fileWidget->transferPercent(percent,speed,fileName);
		}
	}
	mutex.unlock();
}

void FileNotifyManager::onRecvFile(int sock,int fileCount,QString fileName,QString recvDevice,QString sendType,QString fileType)
{
	if (!m_bSendingStatus)
	{
		int iFolder = 0;

		if ("D" == fileType)
		{
			iFolder = 1;
		}
		else
		{
			if (1 < fileCount)
			{
				iFolder = 2;
			}
		}

		fileWidget = new NewFileNotifyWidget(0,recvDevice,sock,iFolder,0,qobject_cast<QWidget*>(parent()));
		connect(fileWidget,SIGNAL(translateText(int,QString*)),this,SIGNAL(translateText(int,QString*)));
		connect(fileWidget,SIGNAL(saveResult(int,QString)),this,SLOT(onSaveResult(int,QString)));
		connect(fileWidget,SIGNAL(stopTransfer(int)),this,SLOT(onStopTransfer(int)));
		connect(fileWidget,SIGNAL(folderChange(QString)),this,SLOT(onFolderChange(QString)));

		if ("" == m_strLastFolder)
		{
			emit queryDefaultRecvFolder(&m_strLastFolder);
		}
		if ("" != m_strLastFolder)
		{
			fileWidget->setLastFolder(m_strLastFolder);
		}
		fileWidget->notifyNewDevice(fileName,fileCount);
		m_bSendingStatus = true;
		mutex.lock();
		sockFileMap.insert(sock,fileWidget);
		mutex.unlock();
	}
	else
	{
		emit saveResult(sock,"REJECTBUSY"); 
	}
}

void FileNotifyManager::onRecvMessage(QString msg,QString ip,QString host)
{
	if ("" == host)
	{
		host = ip;
	}

	msgnotifywidget *msgWidget = new msgnotifywidget(qobject_cast<QWidget*>(parent()));
	msgWidget->notifyNewDevice(msg,host);
}

void FileNotifyManager::onSaveResult(int sock,QString filepath)
{
	emit saveResult(sock,filepath);
}


void FileNotifyManager::onStopTransfer(int sock)
{
	m_bSendingStatus = false;
	emit stopTransfer(sock);
}

void FileNotifyManager::onFolderChange(QString strFolder)
{
	m_strLastFolder = strFolder;
	emit requestStoreDefaultRecvFolder(m_strLastFolder);
}