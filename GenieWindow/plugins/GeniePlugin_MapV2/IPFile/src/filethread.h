#ifndef FILETHREAD_H
#define FILETHREAD_H
#include <QThread>
#include <QMutex>


class CFileThread: public QThread
{
	Q_OBJECT
public:
    CFileThread(int port);
	~CFileThread();
private:
	int m_port;
	bool m_bStop;
	QMutex m_stopMutex;
protected:
	void run();
signals:
	void ErrorOccurSignal(QString err);
public:
	void Stop();
};

#endif // FILETHREAD_H
