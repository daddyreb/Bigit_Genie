//add by caiyujun
#ifndef RECEIVEFILE_H
#define RECEIVEFILE_H

#include <QThread>
#include "udt.h"
#include "api.h"
#include "newfilenotifywidget.h"

class receivefile : public QThread
{
    Q_OBJECT
public:
    explicit receivefile(QObject *parent = 0);

    void startThread();

    bool initSocket(QString strIp);

signals:
    void showReceiveFile(QString strIp, QString strFileName, int fhandle, int iIsFolder, qint64 size, int iCount);

protected:
    void run();

private:
    UDTSOCKET serv;
    UDTSOCKET fhandle;
    NewFileNotifyWidget   *m_ndnWidget;
    uint64_t size;
    //NewFileNotifyWidget *m_ndnWidget;

public slots:
    void onSaveFile(QString strFileName, int iFolder, int handle);
    void onRejectSaveFile(int iFolder, int handle);

    //void onNotifyCompleted(int);

};

#endif // RECEIVEFILE_H
