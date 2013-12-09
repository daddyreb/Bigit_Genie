#ifndef QGENIEINTERFACELOG_H
#define QGENIEINTERFACELOG_H

class QString;
struct QUuid;
#ifndef IN_STABLE_H
#include <QtCore>
#include "Singleton.h"
#include "ILogManager.h"
#endif


class QGenieInterfaceLogManager:public QObject,public ILogManager,public Singleton<QGenieInterfaceLogManager>
{
    Q_OBJECT
    struct _cache{
        QUuid uuid;
        QString str;
    };

public:
    QGenieInterfaceLogManager();
    ~QGenieInterfaceLogManager();
    virtual void writeLog(QString s,QUuid uuid);

    QString getFileName(QUuid uuid,bool *ok=NULL);
    void enableCache();
    void disableCache();
    void closeFile();
    void whichFile();

protected:
    QFile mLogfile;
    QMutex mMutex;
    bool mAvalable;
    QList<_cache> mCache;
    bool mUseCache;
    unsigned int mCheckThreadId;
signals:
    void signal_WriteLog(const QString &s);
protected slots:
    void slot_RealWriteLog(const QString &s);
};

#endif // QGENIEINTERFACELOG_H
