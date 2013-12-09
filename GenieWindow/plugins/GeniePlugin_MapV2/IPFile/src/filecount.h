#ifndef FILECOUNT_H
#define FILECOUNT_H

#include <QObject>
#include <QThread>
#include <QMutex>

class CFileCount : public QThread
{
    Q_OBJECT
public:
    explicit CFileCount(QString filePath, qint64 fileID);
private:
	qint64 m_fileID;
	QString m_filePath;
	bool m_bStop;
	QMutex m_stopMutex;
	QList<QString> m_fileList;
	QString m_fileCheck;

protected:
	void run();
private:
	qint64 CountFileSize(QString filePath);
	QString MD5String(char *str);
public:
	void Stop();
signals:
	void FileSizeSignal(qint64 fileID, qint64 fileSize, QList<QString> fileList, QString fileCheck);
public slots:

};

#endif // FILECOUNT_H
