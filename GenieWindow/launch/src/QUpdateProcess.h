#ifndef QUPDATEPROCESS_H
#define QUPDATEPROCESS_H

#include <QObject>
#include <QDialog>
#include <QHttp>
#include <QFile>
#include <QStringList>
#include <QList>
#include <QTimer>
#include <QDateTime>
#include "ILaunchManager.h"
#include "Singleton.h"



enum PROCESS_STATE
{
    PS_INIT,
    PS_GETDISP,
    PS_GETDB,
    PS_GETFILES,

    PS_FINISH_OK,

    PS_FINISH_ERROR_DIROP,
    PS_FINISH_ERROR_GETDISP,
    PS_FINISH_ERROR_GETDB,
    PS_FINISH_ERROR_GETFILES,
    PS_FINISH_ERROR_MD5,
    PS_FINISH_ERROR_ALREADYNEWEST,
    PS_FINISH_ERROR_USERCLICK,


    PS_UNKNOWN,
    PS_NUM

};




class QLaunchUpdateDialog;


class QUpdateProcess : public QObject,public Singleton<QUpdateProcess>
{
    Q_OBJECT
public:
    QUpdateProcess(QLaunchUpdateDialog *dialog);
    ~QUpdateProcess();
    QFile mReceiveFile;
    QHttp mHttp;
    QStringList mAllFiles;

    PROCESS_STATE mState;
    QString mHttpPath;

    void startup(UPDATE_RESON reson);
    void startGetDisp();
//    void startGetDB();
    void startGetAllFiles();
    bool deleteDirectory(QString DirName,int stack=0);
    void userCancel();
    void runExeFile(QString fpath);
protected:
    QLaunchUpdateDialog *mUpdateDialog;
//    QMap<int ,QString> mCmdIdAndFile;
#ifdef SHOW_REMAIN_TIME
    QTime mUpdateStamp;
    QTime mTextChangeTime;
    int mSizeStamp;
    static const int cCacheSize=60;
    int mSizeCache[cCacheSize];
    int mTimeIntervalCache[cCacheSize];
    int mCurCacheIdx;
    int mCommandStamp;
#endif
    void whenError();
    bool analyzeDBFile(QString &releasenote);

    bool md5Match(QString filename,QString md5str);

public:
    UPDATE_RESON mUpdateReson;
protected slots:

    void    slot_CommandStarted ( int id);
    void    slot_CommandFinished ( int id, bool error );
    void    slot_DataReadProgress ( int done, int total );
    void    slot_Done ( bool error );
#ifdef PLATFORM_MACOSX
    void    slot_startUpdateDMG();
#endif
};

#endif // QUPDATEPROCESS_H
