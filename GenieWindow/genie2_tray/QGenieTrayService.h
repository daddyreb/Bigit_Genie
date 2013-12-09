#ifndef QGENIETRAYSERVICE_H
#define QGENIETRAYSERVICE_H

#include <QtCore>
#include <QLocalSocket>
class QGenieTrayManager;
class QGenieTray_Communication_Verify;

class QGenieTrayService : public QObject
{
    Q_OBJECT
public:
    explicit QGenieTrayService(QObject *parent = 0);
    ~QGenieTrayService();
    QLocalSocket mLocalSock;
    QTimer mTimer;
    QGenieTrayManager *mTrayManager;
#ifdef SECURE_LOCALSOCKET
    QGenieTray_Communication_Verify *mVerify;
#endif
    int mCurrentSession;
//    QList<int> mSessionCache;
signals:

public slots:
    void slot_SockConnected();
//    void slot_SockDisconnected();
    void slot_Read();
    void slot_ActionTrigged(const QString &objname);
//    void slot_SockError(QLocalSocket::LocalSocketError socketError);
};

#endif // QGENIETRAYSERVICE_H
