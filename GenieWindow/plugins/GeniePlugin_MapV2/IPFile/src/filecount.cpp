#include "filecount.h"
#include <QDir>
#include "md5c.h"
#include <QDateTime>

CFileCount::CFileCount(QString filePath, qint64 fileID)
{
	m_fileID = fileID;
	m_filePath = filePath;
	m_bStop = false;	
}

void CFileCount::run()
{
	qint64 fileSize = CountFileSize(m_filePath);
	m_stopMutex.lock();
	if(m_bStop) {
		m_stopMutex.unlock();
		return;
	}
	m_stopMutex.unlock();
	emit FileSizeSignal(m_fileID, fileSize, m_fileList, m_fileCheck);
}

qint64 CFileCount::CountFileSize(QString filePath)
{
	qint64 fileSize = 0;
	QList<QString> fileList;
	QFileInfo fileInfo(filePath);
	if(fileInfo.isDir()) {
		fileList.push_back(filePath);
	} else {
		m_fileList.push_front(filePath);
		fileSize = fileInfo.size();
		QString createTime = fileInfo.created().toString();
		QString modifiedTime = fileInfo.lastModified().toString();
		QString checkString = createTime + modifiedTime;
		checkString += filePath;
		m_fileCheck = MD5String(checkString.toLocal8Bit().data());
	}

	while(!fileList.isEmpty()){
		QString filePath = fileList.value(0);
		QDir dir(filePath);
		dir.setFilter(QDir::Dirs|QDir::Files|QDir::Hidden|QDir::System);
		dir.setSorting(QDir::DirsFirst);
		QFileInfoList list = dir.entryInfoList();
		QFileInfoList::iterator iter;
		for(iter=list.begin(); iter!=list.end(); iter++) {
			m_stopMutex.lock();
			if(m_bStop) {
				m_stopMutex.unlock();
				return -1;
			}
			m_stopMutex.unlock();
			if(iter->fileName() == "." || iter->fileName() == "..") {
				continue;
			}
			if(iter->isDir()) {
				fileList.push_back(iter->filePath());
			} else {
				fileSize += iter->size();
				m_fileList.push_front(iter->filePath());
			}
		}
		fileList.pop_front();
	}
	if(m_fileCheck.size() == 0) {
		QString checkString;
		QList<QString>::iterator iter;
		for(iter=m_fileList.begin(); iter!=m_fileList.end(); iter++) {
			checkString += (*iter);
		}
		m_fileCheck = MD5String(checkString.toLocal8Bit().data());
	}
	return fileSize;
}

void CFileCount::Stop()
{
	m_stopMutex.lock();
	m_bStop = true;
	m_stopMutex.unlock();
}

QString CFileCount::MD5String(char *str)
{
	unsigned char digest[16];
	MD5_CTX context;
	unsigned int len = strlen(str);

	MD5Init(&context);
	MD5Update(&context, str, len);
	MD5Final(digest, &context);

	char szDigest[33];
	memset(szDigest, 0, 33);
	for(int i=0; i<16; i++) {
		sprintf(szDigest+i*2, "%02x",	digest[i]);
	}
	return QString(szDigest);
}
