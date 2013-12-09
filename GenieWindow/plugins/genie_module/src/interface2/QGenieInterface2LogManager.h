#ifndef QGENIEINTERFACELOG_H
#define QGENIEINTERFACELOG_H

class QString;
struct QUuid;

#include <QtCore>
#include "Singleton.h"


class QGenieInterface2LogManager:public QObject,public Singleton<QGenieInterface2LogManager>
{
    Q_OBJECT
    struct _cache{
        QUuid uuid;
        QString str;
    };

public:
    QGenieInterface2LogManager();
    ~QGenieInterface2LogManager();
    virtual void writeLog(const QString &s,int uuid);

//    QString getFileName(QUuid uuid,bool *ok=NULL);
//    void enableCache();
//    void disableCache();
    void closeFile();
    void whichFile();

protected:
    QFile mLogfile;
//    QMutex mMutex;
    bool mAvalable;
//    QList<_cache> mCache;
//    bool mUseCache;
//    unsigned int mCheckThreadId;
//signals:
//    void signal_WriteLog(const QString &s);
//protected slots:
    void slot_RealWriteLog(const QString &s);
};

#endif // QGENIEINTERFACELOG_H
