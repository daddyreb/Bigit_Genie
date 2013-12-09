//add by caiyujun
#ifndef SENDFILE_H
#define SENDFILE_H

#include <QThread>
#include "udt.h"
#include "api.h"
#include <string>
#include <ws2tcpip.h>
#include <QList>
#include <QUrl>
#include "core.h"

#include <QStringList>

class sendfile : public QThread
{
    Q_OBJECT
public:
    explicit sendfile(QString strIp, QString strFile, QString strDevName, QObject *parent = 0);
    sendfile(QString strIp, QStringList fileList, QString strDevName, QObject *parent = 0);
    sendfile(QString strIp, QString strFolder, QStringList files, QStringList emptyFolders, qint64 iSize, QString strDevName, QObject *parent = 0);

    bool initSocket(QString strIp, QList<QUrl> urls);
    void startThread();

    QString getActualFileName(QString strFileName);

    int sendfiles(UDTSOCKET m_fhandle);
    int sendMultiFiles(UDTSOCKET m_fhandle);
    int sendfolder(UDTSOCKET m_fhandle);
    void sendFailMessage(QString strErrorMessage);

signals:
    void sendSuccess();
    void sendProgress(qint64 iPercent, double speed);
    void rejectSaveFile();
    void unsupported();
    void fileChange(QString strFile);
    void sendFail(QString strErrorMessage);

protected:
    void run();

private:

    UDTSOCKET m_fhandle;
    QString m_strFileName;
    QStringList m_files;
    QStringList m_emptyFoldersList;
    QString m_strDeviceName;
    std::string strDestIp;
    CUDT                *m_udt;
    int m_iMode;
    qint64 m_iFolderSize;
    volatile bool closeFlag;

public slots:
    void onSendProgress(qint64 iSize, double iProgress);
    void slot_CancelSend(); ///kai.yan add

};

#endif // SENDFILE_H
