#ifndef QGENIEHTTPREQUESTMANAGER_H
#define QGENIEHTTPREQUESTMANAGER_H

#include <QtCore>
class QGenieHttpRequestManager;
class QGenieHttpRequestThread:public QThread
{
    Q_OBJECT
    QGenieHttpRequestThread(QGenieHttpRequestManager *manager);
    void run();
    QGenieHttpRequestManager *mManager;
signals:
    void signal_Done(int,bool,QString);
    friend class QGenieHttpRequestManager;
};

class QGenieHttpRequestManager : public QObject
{
    Q_OBJECT
public:
    explicit QGenieHttpRequestManager(QObject *parent = 0);
    ~QGenieHttpRequestManager();
    int addRequest(const QString &path);
signals:
    void signal_Done(int,bool,QString);
protected:
    QList<QString> mPathCache;
    QList<int>      mSessionCache;
    QString mAddress;
    quint16 mPort;
    int mSavedCurSession;
    QMutex mMutex;
    QGenieHttpRequestThread mThread;
    bool mExit;
    friend class QGenieHttpRequestThread;

};

#endif // QGENIEHTTPREQUESTMANAGER_H
