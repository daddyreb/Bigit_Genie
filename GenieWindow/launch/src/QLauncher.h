#ifndef QLAUNCHER_H
#define QLAUNCHER_H

#include <QPluginLoader>
//#include "ILaunchInf.h"
#include <QObject>
//#include "QLaunchUpdater.h"
//#include <Singleton.h>
// 
#include <QTimer>
//#include "QLaunchSemaphore.h"

//class QGenieSingleProcessHandler;

class QLauncher:public QObject
{
    Q_OBJECT
public:
    QLauncher();
    ~QLauncher();
    bool startPlugin();
//    void stopPlugin();
//    bool tryStartup();
//    QString cmd(QString para);
//	void debugRestart();
//    inline ILaunchInf *getLaunchInf()
//    {
//        return NULL;
//    }
//    QLaunchSemaphore mSem;
//    ILaunchInf *mLi;
protected:
//    QPluginLoader mLoader;

    //QLaunchUpdater mUpdater;
//    QGenieSingleProcessHandler * mSingleHandler;
	QTimer mDebugTimer;
        QPluginLoader loader;

protected slots:
//	void slot_Debug();
//     void slot_RealStartup();



};

#endif // QLAUNCHER_H
