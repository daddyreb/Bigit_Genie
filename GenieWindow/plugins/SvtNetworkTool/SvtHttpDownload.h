#ifndef SVTHTTPDOWNLOAD_H
#define SVTHTTPDOWNLOAD_H

#include <QThread>
#include <QtCore>
#include "SvtNetworkTool.h"

class SVT_HttpDownloadPool;
class SVT_NETWORKTOOL_EXPORT SVT_HttpDownload : public QObject,public QRunnable
{
    Q_OBJECT
protected:
    explicit SVT_HttpDownload();
    ~SVT_HttpDownload();
public:
    void startGetFile(const QString &url
                      ,quint16 port
                      ,const QString &localfile
                      ,quint64 startPos=0);


protected:
    void run();
    bool getHttpFileProcess();
    inline void setExitFlag()
    {
        mbExitFlag=true;
    }
signals:
    void done(bool error,const QString& serror);
    void progress(quint64 done,quint64 total,quint32 speed);

public slots:
protected:
    bool mbExitFlag;
    QString mLocalFile;
    QString mUrl;
    quint16 mPort;
    quint64 mStartPos;
    QString mErrorStr;
    SVT_HttpDownloadPool *mPool;
    friend class SVT_HttpDownloadPool;


};

class SVT_NETWORKTOOL_EXPORT SVT_HttpDownloadPool:public QThreadPool
{
    Q_OBJECT
public:
    SVT_HttpDownloadPool();
    ~SVT_HttpDownloadPool();
    SVT_HttpDownload *newHttp();
    void setExitFlag(SVT_HttpDownload *http);
protected slots:
//    void slot_HttpDestroyed(QObject *obj);
protected:
    QSet<SVT_HttpDownload *> mSet;
    friend class SVT_HttpDownload;
};

#endif // SVTHTTPDOWNLOAD_H
