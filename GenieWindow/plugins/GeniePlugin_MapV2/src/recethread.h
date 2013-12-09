//add by caiyujun
#ifndef RECETHREAD_H
#define RECETHREAD_H

#include <QThread>
#include "core.h"
#include "api.h"
#include "udt.h"
#include <QString>

class recethread : public QThread
{
    Q_OBJECT
public:
    explicit recethread(QString strFileName, int handle, int iFolder, qint64 iSize, QObject *parent = 0);
    QString getActualFileName(QString strFileName);

protected:
    void run();

signals:
    void sendProgress(qint64 iPercent, double speed);
    void sendSuccess();
    void recvFileChange(QString strFile);
    void recvFail(QString recvError);

public slots:
    void onSendProgress(qint64 iSize, double iProgress);
    void slot_cancelReceive();  ///kai.yan add

private:
    uint64_t size;
    CUDT *udt;
    UDTSOCKET m_handle;
    QString m_strFileName;
    int m_iFolder;
    qint64 m_size;
    volatile bool m_closeFlag; ///kai.yan add for cancel receive file

};

#endif // RECETHREAD_H
