#ifndef QGENIETRAY_COMMUNICATION_VERIFY_H
#define QGENIETRAY_COMMUNICATION_VERIFY_H
#ifdef SECURE_LOCALSOCKET
#include <QObject>
#include <QLocalSocket>
#include <QtCore>

class QGenieTray_Communication_Verify : public QObject
{
    Q_OBJECT
public:
    enum ROLE
    {
        SENDER,
        RECEIVER,

        UNKNOW
    };
public:
    explicit QGenieTray_Communication_Verify(QObject *parent = 0);
    ~QGenieTray_Communication_Verify();

    //send data
    void setLocalSocket(QLocalSocket *localSock);
    int addNewData(const QString newData, ROLE role);
    void uiCommand(const QString &);

    //other
    void setCacheTableLen(int len = 10);
    bool delData(int session);
    void setWaitTime(int sec = 3000);
    void setRetryCount(int count = 3);
    void clearCacheTable();
    void initData();
    int getCmdAndPara(QString &para, int role = 0);
    int getRole(QString &para);
    int getSession(QString &para);

    //recieve data
    bool chkCurId(int session);
    int respond(int session);

protected:
    void uiCommand();


private:
    int mCacheLen;
    int mRetryCount;
    int mWaitTime;
    int mRetSession;
    QTimer mTimer;
    QList<QVariant> mCacheList;      //cache table:command,retry,session,·½Ïò("sender" or "receiver")
    QLocalSocket *mLocalSock;
	//QList<int> mRespondList;


signals:

public slots:
    void slot_timeout();

};
#endif

#endif // QGENIETRAY_COMMUNICATION_VERIFY_H
